#include "backend.h"

void updateGame() {
  State_t *state = getCurrentState();
  if (state->status == Moving && !state->pause && getTimeUntilStep() == 0) {
    shift();
  }
}

GameInfo_t updateCurrentState() {
  State_t *state = getCurrentState();
  updateGame();
  GameInfo_t info = {0};
  int high_score = 0;
  FILE *file = fopen(".tetris_high_score.txt", "r");
  if (file) {
    if (fscanf(file, "%d", &high_score)) {
      fclose(file);
    }
  }
  info.high_score = high_score;
  int **field = createField(HEIGHT, WIDTH);
  if (!field) {
    info.field = NULL;
    return info;
  }
  copyField(field, state->field, HEIGHT, WIDTH);
  for (int i = 0; i < state->block_size; i++) {
    for (int j = 0; j < state->block_size; j++) {
      int new_x = state->x - i;
      int new_y = state->y + j;
      if (state->block[i][j] == 1 && new_x >= 0 && new_y >= 0 &&
          new_y < WIDTH) {
        field[new_x][new_y] = 1;
      }
    }
  }
  info.speed = state->speed;
  info.level = state->level;
  info.field = field;
  info.score = state->score;
  int **next = createField(4, 4);
  if (!next) {
    freeField(field, HEIGHT);
    info.field = NULL;
    info.next = NULL;
    return info;
  }
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      next[i][j] = 0;
    }
  }
  int offset_x = (4 - state->next_block_size) / 2;
  int offset_y = (4 - state->next_block_size) / 2;
  for (int i = 0; i < state->next_block_size; i++) {
    for (int j = 0; j < state->next_block_size; j++) {
      next[offset_x + i][offset_y + j] = state->next_block[i][j];
    }
  }
  info.next = next;
  info.pause = state->pause ? PauseGame : StartGame;
  if (state->status == Ending) {
    info.pause = RetryGame;
  }
  return info;
}

void userInput(UserAction_t action, bool hold) {
  State_t *state = getCurrentState();
  if (action == Terminate) {
    endGame();
  } else if (action == Start) {
    if (state->status == Starting || state->status == Ending) {
      initializeState();
      startGame();
    }
  } else {
    switch (action) {
      case Left:
        if (state->status == Moving) moveLeft();
        break;
      case Right:
        if (state->status == Moving) moveRight();
        break;
      case Down:
        if (state->status == Moving) {
          while (isBlockContacted() == 0) {
            state->x++;
          }
          state->status = Contacting;
          contact();
        }
        break;
      case Pause:
        if (state->status != Starting) pauseGame();
        break;
      case Action:
        if (state->status == Moving) rotateBlock();
        break;
      default:
        break;
    }
  }
}

int **createField(int h, int w) {
  int **field = (int **)malloc(h * sizeof(int *));
  for (int i = 0; i < h; i++) {
    int *row = (int *)malloc(w * sizeof(int));
    field[i] = row;
    for (int j = 0; j < w; j++) {
      field[i][j] = 0;
    }
  }
  return field;
}

void copyField(int **dest, int **src, int h, int w) {
  if (src != NULL && dest != NULL) {
    for (int i = 0; i < h; i++) {
      for (int j = 0; j < w; j++) {
        dest[i][j] = src[i][j];
      }
    }
  }
}

void freeField(int **field, int size) {
  for (int i = 0; i < size; i++) {
    free(field[i]);
  }
  free(field);
}

unsigned long long getTimeUntilStep() {
  State_t *state = getCurrentState();
  unsigned long long result = -1;

  if (state->status == Moving && !state->pause) {
    unsigned long long current_time = getCurrentTime();
    unsigned long long elapsed = current_time - state->start_time;

    if (elapsed >= state->time_until_step) {
      state->time_until_step = state->speed;
      state->start_time = current_time;
      result = 0;
    } else {
      result = state->time_until_step - elapsed;
      state->time_until_step = result;
      state->start_time = current_time;
    }
  }
  return result;
}

State_t *getCurrentState() {
  static State_t state;
  return &state;
}

unsigned long long getCurrentTime() {
  struct timespec ts;
  timespec_get(&ts, TIME_UTC);
  return ts.tv_sec * 1000ULL + ts.tv_nsec / 1000000ULL;
}

void initializeState() {
  State_t *state = getCurrentState();
  state->status = Starting;
  int **field = createField(HEIGHT, WIDTH);
  state->field = field;
  state->score = 0;
  state->level = 1;
  state->speed = SPEED;
  state->time_until_step = state->speed;
  state->pause = false;
  state->x = 0;
  state->y = 4;
  state->start_time = getCurrentTime();
  state->terminate_requested = false;
  int next_block_size;
  int **next_block = generateNewBlock(&next_block_size);
  int **block = createField(next_block_size, next_block_size);
  copyField(block, next_block, next_block_size, next_block_size);
  state->next_block_size = next_block_size;
  state->next_block = next_block;
  state->block_size = next_block_size;
  state->block = block;
}

void startGame() {
  State_t *state = getCurrentState();
  state->status = Spawning;
  state->pause = false;
  state->start_time = getCurrentTime();
  state->time_until_step = state->speed;
  spawn();
}

void pauseGame() {
  State_t *state = getCurrentState();
  if (state->pause) {
    state->pause = 0;
    state->start_time = getCurrentTime();
  } else {
    state->pause = 1;
    unsigned long long elapsed = getCurrentTime() - state->start_time;
    state->time_until_step = (elapsed < state->time_until_step)
                                 ? state->time_until_step - elapsed
                                 : 0;
  }
}

void endGame() {
  State_t *state = getCurrentState();
  if (state->block) {
    freeField(state->block, state->block_size);
  }
  if (state->next_block) {
    freeField(state->next_block, state->next_block_size);
  }
  if (state->field) {
    freeField(state->field, HEIGHT);
  }
  initializeState();
}

void requestTermination() {
  State_t *state = getCurrentState();
  state->terminate_requested = true;
  endGame();
}

void rotate(int **new_block, int **old_block, int size) {
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      new_block[i][j] = 0;
    }
  }
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      new_block[j][size - 1 - i] = old_block[i][j];
    }
  }
}

int **generateNewBlock(int *block_size) {
  static const struct {
    int size;
    int coords[4][2];
  } BLOCKS[7] = {[I_BLOCK] = {4, {{0, 0}, {0, 1}, {0, 2}, {0, 3}}},
                 [L_BLOCK] = {3, {{1, 0}, {0, 0}, {0, 1}, {0, 2}}},
                 [J_BLOCK] = {3, {{1, 2}, {0, 0}, {0, 1}, {0, 2}}},
                 [O_BLOCK] = {2, {{0, 0}, {0, 1}, {1, 0}, {1, 1}}},
                 [Z_BLOCK] = {3, {{0, 0}, {0, 1}, {1, 1}, {1, 2}}},
                 [T_BLOCK] = {3, {{0, 0}, {0, 1}, {0, 2}, {1, 1}}},
                 [S_BLOCK] = {3, {{1, 0}, {1, 1}, {0, 1}, {0, 2}}}};

  int **block = NULL;
  int type = rand() % 7;
  *block_size = BLOCKS[type].size;
  block = createField(*block_size, *block_size);
  for (int i = 0; i < 4; i++) {
    int x = BLOCKS[type].coords[i][0];
    int y = BLOCKS[type].coords[i][1];
    if (x < *block_size && y < *block_size) {
      block[x][y] = 1;
    }
  }

  return block;
}

void spawn() {
  State_t *state = getCurrentState();
  if (state->block) {
    freeField(state->block, state->block_size);
  }
  state->block = state->next_block;
  state->block_size = state->next_block_size;
  state->x = 0;
  state->y = (state->block_size == 2) ? 4 : 3;
  state->next_block = generateNewBlock(&state->next_block_size);
  state->status = Moving;
  state->start_time = getCurrentTime();
  state->time_until_step = state->speed;
}

void moveLeft() {
  State_t *state = getCurrentState();
  bool can_move = true;
  for (int i = 0; can_move && i < state->block_size; i++) {
    for (int j = 0; can_move && j < state->block_size; j++) {
      if (state->block[i][j]) {
        int new_y = state->y + j - 1;
        int new_x = state->x - i;

        if (new_y < 0 || (new_x >= 0 && state->field[new_x][new_y])) {
          can_move = false;
        }
      }
    }
  }
  if (can_move) {
    state->y--;
  }
  state->status =
      can_move ? (isBlockContacted() ? Contacting : Moving) : Moving;
}

void moveRight() {
  State_t *state = getCurrentState();
  bool can_move = true;
  for (int i = 0; can_move && i < state->block_size; i++) {
    for (int j = 0; can_move && j < state->block_size; j++) {
      if (state->block[i][j]) {
        int new_y = state->y + j + 1;
        int new_x = state->x - i;

        if (new_y >= WIDTH ||
            (new_x >= 0 && new_y >= 0 && state->field[new_x][new_y])) {
          can_move = false;
        }
      }
    }
  }
  if (can_move) {
    state->y++;
  }
  state->status =
      can_move ? (isBlockContacted() ? Contacting : Moving) : Moving;
}

void shift() {
  State_t *state = getCurrentState();
  if (isBlockContacted() == 0) {
    state->x++;
    state->start_time = getCurrentTime();
    state->time_until_step = state->speed;
  } else {
    state->status = Contacting;
    contact();
  }
}

int isBlockContacted() {
  const State_t *state = getCurrentState();
  int result = 0;
  for (int i = 0; !result && i < state->block_size; i++) {
    for (int j = 0; !result && j < state->block_size; j++) {
      if (state->block[i][j]) {
        int x = state->x + 1 - i;
        int y = state->y + j;

        result = (x >= HEIGHT) || (x >= 0 && (y < 0 || y >= WIDTH)) ||
                 (x >= 0 && state->field[x][y]);
      }
    }
  }
  return result;
}

void contact() {
  State_t *state = getCurrentState();
  bool game_over = false;
  for (int i = 0; i < state->block_size; i++) {
    for (int j = 0; j < state->block_size; j++) {
      int x = state->x - i;
      int y = state->y + j;

      if (state->block[i][j]) {
        if (x >= 0 && x < HEIGHT && y >= 0 && y < WIDTH) {
          state->field[x][y] = 1;
        } else if (x < 0) {
          game_over = true;
        }
      }
    }
  }
  if (!game_over) {
    deleteLines();
    for (int y = 0; y < WIDTH; y++) {
      if (state->field[0][y] == 1) {
        game_over = true;
        break;
      }
    }
  }
  if (game_over) {
    state->status = Ending;
    state->pause = RetryGame;
    saveHighScore();
  } else {
    state->status = Spawning;
    spawn();
  }
}

int canRotateBlock(int **new_block) {
  State_t *state = getCurrentState();
  int result = 1;
  int **original_block = state->block;
  state->block = new_block;
  for (int i = 0; result && i < state->block_size; i++) {
    for (int j = 0; result && j < state->block_size; j++) {
      int x = state->x - i;
      int y = state->y + j;

      if (new_block[i][j]) {
        result = !(y < 0 || y >= WIDTH || (x >= 0 && state->field[x][y]));
      }
    }
  }
  state->block = original_block;
  return result;
}

void rotateBlock() {
  State_t *state = getCurrentState();
  int **new_block = createField(state->block_size, state->block_size);
  rotate(new_block, state->block, state->block_size);
  if (canRotateBlock(new_block) == 1) {
    freeField(state->block, state->block_size);
    state->block = new_block;
  } else {
    freeField(new_block, state->block_size);
  }
  int attached = isBlockContacted();
  if (attached == 0) {
    state->status = Moving;
  } else {
    state->status = Contacting;
  }
}

void saveHighScore() {
  const State_t *state = getCurrentState();
  int current_high_score = 0;
  FILE *file = fopen(".tetris_high_score.txt", "r+");
  if (file) {
    if (fscanf(file, "%d", &current_high_score)) {
      fclose(file);
    }
  }
  if (state->score > current_high_score) {
    file = fopen(".tetris_high_score.txt", "w");
    if (file) {
      fprintf(file, "%d", state->score);
      fclose(file);
    }
  }
}

void updateLevel() {
  State_t *state = getCurrentState();
  int new_level = state->score / NEW_LEVEL_THRESHOLD + 1;
  if (new_level > MAX_LEVEL) {
    new_level = MAX_LEVEL;
  }
  state->speed -= (new_level - state->level) * SPEED_STEP;
  state->level = new_level;
}

void deleteLines() {
  State_t *state = getCurrentState();
  int lines_cleared = 0;
  const int points[] = {0, 100, 300, 700, 1500};
  for (int y = HEIGHT - 1; y > 0; y--) {
    int filled = 0;
    for (int x = 0; x < WIDTH; x++) filled += state->field[y][x];

    if (filled == WIDTH) {
      for (int z = y; z > 0; z--) {
        memcpy(state->field[z], state->field[z - 1], WIDTH * sizeof(int));
      }
      y++;
      lines_cleared++;
    }
  }
  if (lines_cleared > 0 && lines_cleared <= 4) {
    state->score += points[lines_cleared];
    saveHighScore();
    updateLevel();
  }
}