#include "snakeModel.h"

using namespace s21;

SnakeModel::SnakeModel() { initializeState(); }

void SnakeModel::resetGame() {
  cleanupField();
  cleanupNext();
  game_state_ = SelectGame;
  snake_body.clear();
  current_state_.score = 0;
  current_state_.level = 1;
  current_state_.speed = SPEED;
  current_state_.pause = false;
  timer_state_.accelerate = false;
  exit_requested_ = false;
  initializeState();
}

void SnakeModel::initializeState() {
  cleanupField();
  cleanupNext();
  game_state_ = SelectGame;
  snake_body = {{5, 5}, {5, 4}, {5, 3}, {5, 2}};
  current_direction_ = RIGHT;
  current_state_.score = 4;
  current_state_.level = 1;
  current_state_.speed = SPEED;
  current_state_.pause = false;
  timer_state_.start_time = getCurrentTime();
  timer_state_.time_until_step = current_state_.speed;
  timer_state_.accelerate = false;

  current_state_.field = new int *[HEIGHT];
  for (int i = 0; i < HEIGHT; ++i) {
    current_state_.field[i] = new int[WIDTH]{0};
  }
  current_state_.next = new int *[4];
  for (int i = 0; i < 4; ++i) {
    current_state_.next[i] = new int[4]{0};
  }

  for (const auto &segment : snake_body) {
    if (segment.first >= 0 && segment.first < HEIGHT && segment.second >= 0 &&
        segment.second < WIDTH) {
      current_state_.field[segment.first][segment.second] = 1;
    }
  }
  spawnApple();

  std::ifstream file(".snake_high_score.txt");
  if (file.is_open()) {
    file >> current_state_.high_score;
    file.close();
  } else {
    current_state_.high_score = 0;
  }
}

void SnakeModel::cleanupField() {
  if (current_state_.field != nullptr) {
    for (int i = 0; i < HEIGHT; ++i) {
      delete[] current_state_.field[i];
    }
    delete[] current_state_.field;
    current_state_.field = nullptr;
  }
}

void SnakeModel::cleanupNext() {
  if (current_state_.next != nullptr) {
    for (int i = 0; i < 4; ++i) {
      delete[] current_state_.next[i];
    }
    delete[] current_state_.next;
    current_state_.next = nullptr;
  }
}

SnakeModel::~SnakeModel() {
  cleanupField();
  cleanupNext();
  saveHighScore();
}

GameInfo_t SnakeModel::updateCurrentState() {
  GameInfo_t info = current_state_;
  info.field = new int *[HEIGHT];
  for (int i = 0; i < HEIGHT; ++i) {
    info.field[i] = new int[WIDTH]();
  }
  info.next = new int *[4];
  for (int i = 0; i < 4; ++i) {
    info.next[i] = new int[4]();
  }

  for (int i = 0; i < HEIGHT; ++i) {
    for (int j = 0; j < WIDTH; ++j) {
      info.field[i][j] = current_state_.field[i][j];
    }
  }

  info.score = current_state_.score;
  info.high_score = current_state_.high_score;
  info.level = current_state_.level;
  info.speed = current_state_.speed;

  if (game_state_ == SelectGame) {
    info.pause = SelectGame;
  } else if (current_state_.pause) {
    info.pause = PauseGame;
  } else if (game_state_ == EndGame || game_state_ == WinGame) {
    info.pause = EndGame;
  } else {
    info.pause = StartGame;
  }

  updateGame();
  return info;
}

bool SnakeModel::isPaused() const { return current_state_.pause; }

void SnakeModel::setAccelerate(bool accelerate) {
  timer_state_.accelerate = accelerate;
}

bool SnakeModel::shouldUpdateGame() {
  return getTimeUntilStep() == 0 && !current_state_.pause && !exit_requested_;
}

void SnakeModel::updateGame() {
  if (!current_state_.pause && game_state_ == StartGame && shouldUpdateGame()) {
    moveSnake();
    updateScore();
  }
}

void SnakeModel::updateLevel() {
  int new_level = current_state_.score / SNAKE_LEVEL_THRESHOLD + 1;
  if (new_level > SNAKE_MAX_LEVEL) {
    new_level = SNAKE_MAX_LEVEL;
  }
  current_state_.speed = (SPEED - (new_level - 1) * SNAKE_SPEED_STEP > 10)
                             ? SPEED - (new_level - 1) * SNAKE_SPEED_STEP
                             : 10;
  current_state_.level = new_level;
}

void SnakeModel::updateScore() {
  if (current_state_.score > current_state_.high_score) {
    current_state_.high_score = current_state_.score;
    saveHighScore();
  }
}

void SnakeModel::moveSnake() {
  std::pair<int, int> new_head = snake_body.front();
  switch (current_direction_) {
    case UP:
      new_head.first--;
      break;
    case DOWN:
      new_head.first++;
      break;
    case LEFT:
      new_head.second--;
      break;
    case RIGHT:
      new_head.second++;
      break;
  }
  if (new_head.first < 0 || new_head.first >= HEIGHT || new_head.second < 0 ||
      new_head.second >= WIDTH) {
    game_state_ = EndGame;
    saveHighScore();
    return;
  }
  for (size_t i = 1; i < snake_body.size(); ++i) {
    if (new_head == snake_body[i]) {
      game_state_ = EndGame;
      saveHighScore();
      return;
    }
  }
  for (int i = 0; i < HEIGHT; ++i) {
    for (int j = 0; j < WIDTH; ++j) {
      if (current_state_.field[i][j] == 1) {
        current_state_.field[i][j] = 0;
      }
    }
  }
  snake_body.insert(snake_body.begin(), new_head);
  if (new_head == apple_position) {
    current_state_.score++;
    updateLevel();
    spawnApple();
  } else {
    snake_body.pop_back();
  }
  for (const auto &segment : snake_body) {
    if (segment.first >= 0 && segment.first < HEIGHT && segment.second >= 0 &&
        segment.second < WIDTH) {
      current_state_.field[segment.first][segment.second] = 1;
    }
  }
  if (snake_body.size() >= SNAKE_MAX_LENGTH) {
    game_state_ = WinGame;
    saveHighScore();
  }
}

void SnakeModel::resetTimer() {
  timer_state_.start_time = getCurrentTime();
  timer_state_.time_until_step = current_state_.speed;
  timer_state_.accelerate = false;
}

void SnakeModel::startGame() {
  initializeState();
  game_state_ = StartGame;
  timer_state_.start_time = getCurrentTime();
  timer_state_.time_until_step = current_state_.speed;
}

void SnakeModel::pauseGame() {
  current_state_.pause = !current_state_.pause;
  if (!current_state_.pause) {
    timer_state_.start_time = getCurrentTime();
    timer_state_.time_until_step = current_state_.speed;
  } else {
    unsigned long long elapsed = getCurrentTime() - timer_state_.start_time;
    timer_state_.time_until_step = elapsed < timer_state_.time_until_step
                                       ? timer_state_.time_until_step - elapsed
                                       : 0;
  }
}

void SnakeModel::processMovement(UserAction_t action) {
  switch (action) {
    case Action:
      if (!current_state_.pause && game_state_ == StartGame) {
        moveSnake();
        updateScore();
        resetTimer();
      }
      break;
    case Up:
      if (current_direction_ != DOWN) current_direction_ = UP;
      break;
    case Down:
      if (current_direction_ != UP) current_direction_ = DOWN;
      break;
    case Left:
      if (current_direction_ != RIGHT) current_direction_ = LEFT;
      break;
    case Right:
      if (current_direction_ != LEFT) current_direction_ = RIGHT;
      break;
    default:
      break;
  }
}

unsigned long long SnakeModel::getCurrentTime() {
  timespec ts;
  timespec_get(&ts, TIME_UTC);
  return ts.tv_sec * 1000ULL + ts.tv_nsec / 1000000ULL;
}

State SnakeModel::getGameState() const { return game_state_; }

bool SnakeModel::shouldExit() const { return should_exit_; }

void SnakeModel::requestExit() { should_exit_ = true; }

unsigned long long SnakeModel::getTimeUntilStep() {
  unsigned long long res = -1;
  if (game_state_ != SelectGame) {
    unsigned long long current_time = getCurrentTime();
    unsigned long long elapsed = current_time - timer_state_.start_time;
    if (elapsed >= timer_state_.time_until_step) {
      timer_state_.time_until_step = current_state_.speed;
      timer_state_.start_time = current_time;
      res = 0;
    } else {
      timer_state_.time_until_step -= elapsed;
      timer_state_.start_time = current_time;
      res = timer_state_.time_until_step;
    }
  }
  return res;
}

void SnakeModel::spawnApple() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis_x(0, HEIGHT - 1);
  std::uniform_int_distribution<> dis_y(0, WIDTH - 1);

  bool valid = false;
  while (!valid) {
    apple_position = {dis_x(gen), dis_y(gen)};
    valid = true;
    for (const auto &segment : snake_body) {
      if (segment == apple_position) {
        valid = false;
        break;
      }
    }
  }
  current_state_.field[apple_position.first][apple_position.second] = 2;
}

void SnakeModel::saveHighScore() {
  std::ofstream file(".snake_high_score.txt");
  if (!file.is_open()) {
    return;
  }
  file << current_state_.high_score;
  file.close();
}