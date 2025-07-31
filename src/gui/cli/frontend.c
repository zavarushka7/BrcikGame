#include "frontend.h"

void initializeGUI() {
  initscr();
  cbreak();
  noecho();
  curs_set(0);
  keypad(stdscr, TRUE);
  nodelay(stdscr, TRUE);
  start_color();
  initColors();

  refresh();
}

void initColors() {
  start_color();
  init_pair(1, COLOR_WHITE, COLOR_BLACK);  // границы
  init_pair(2, COLOR_GREEN, COLOR_GREEN);  // блоки/ змейка
  init_pair(3, COLOR_WHITE, COLOR_BLUE);   // пауза
  init_pair(4, COLOR_WHITE, COLOR_RED);    //  конец игры
  init_pair(5, COLOR_GREEN, COLOR_BLACK);  // текст
  init_pair(6, COLOR_RED, COLOR_RED);      // яблока
}

void cleanupGUI() { endwin(); }

void printControls() {
  int start_y = FIELD_OFFSET_Y + FIELD_H + 2;
  attron(COLOR_PAIR(5) | A_BOLD);
  mvprintw(start_y, FIELD_OFFSET_X, "CONTROLS:");
  attroff(COLOR_PAIR(5) | A_BOLD);
  mvprintw(start_y + 1, FIELD_OFFSET_X, "Start:    Enter");
  mvprintw(start_y + 2, FIELD_OFFSET_X, "Pause:    P");
  mvprintw(start_y + 3, FIELD_OFFSET_X, "Action:   Space");
  mvprintw(start_y + 4, FIELD_OFFSET_X, "Left:     <-");
  mvprintw(start_y + 5, FIELD_OFFSET_X, "Right:    ->");
  mvprintw(start_y + 6, FIELD_OFFSET_X, "Drop:     v");
  mvprintw(start_y + 7, FIELD_OFFSET_X, "Exit:     Esc");
}

void printGameField(GameInfo_t game_info) {
  attron(COLOR_PAIR(1));
  for (int i = 0; i <= FIELD_H + 1; i++) {
    mvprintw(FIELD_OFFSET_Y + i - 1, FIELD_OFFSET_X - 1, "|");
    mvprintw(FIELD_OFFSET_Y + i - 1, FIELD_OFFSET_X + FIELD_W * 2, "|");
  }
  for (int j = 0; j <= FIELD_W * 2; j++) {
    mvprintw(FIELD_OFFSET_Y - 1, FIELD_OFFSET_X + j - 1, "-");
    mvprintw(FIELD_OFFSET_Y + FIELD_H, FIELD_OFFSET_X + j - 1, "-");
  }
  attroff(COLOR_PAIR(1));
  for (int i = 0; i < FIELD_H; i++) {
    for (int j = 0; j < FIELD_W; j++) {
      if (game_info.field[i][j] == 1) {
        attron(COLOR_PAIR(2));
        mvprintw(FIELD_OFFSET_Y + i, FIELD_OFFSET_X + j * 2, "[]");
        attroff(COLOR_PAIR(2));
      } else if (game_info.field[i][j] == 2) {
        attron(COLOR_PAIR(6));
        mvprintw(FIELD_OFFSET_Y + i, FIELD_OFFSET_X + j * 2, "**");
        attroff(COLOR_PAIR(6));
      } else {
        mvprintw(FIELD_OFFSET_Y + i, FIELD_OFFSET_X + j * 2, "  ");
      }
    }
  }
}

void printGameInfo(GameInfo_t game_info) {
  attron(COLOR_PAIR(5) | A_BOLD);
  mvprintw(FIELD_OFFSET_Y, INFO_OFFSET_X, "GAME INFO");
  attroff(COLOR_PAIR(5) | A_BOLD);
  if (game_info.next != NULL) {
    mvprintw(FIELD_OFFSET_Y + 2, INFO_OFFSET_X, "Next:");
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        if (game_info.next[i][j] == 1) {
          attron(COLOR_PAIR(2));
          mvprintw(FIELD_OFFSET_Y + 4 + i, INFO_OFFSET_X + j * 2, "[]");
          attroff(COLOR_PAIR(2));
        } else {
          mvprintw(FIELD_OFFSET_Y + 4 + i, INFO_OFFSET_X + j * 2, "  ");
        }
      }
    }
  }
  mvprintw(FIELD_OFFSET_Y + 9, INFO_OFFSET_X, "High Score: %6d",
           game_info.high_score);
  mvprintw(FIELD_OFFSET_Y + 11, INFO_OFFSET_X, "Score:     %6d",
           game_info.score);
  mvprintw(FIELD_OFFSET_Y + 13, INFO_OFFSET_X, "Level:     %6d",
           game_info.level);
  mvprintw(FIELD_OFFSET_Y + 15, INFO_OFFSET_X, "Speed:     %6d",
           game_info.speed);
}

void printPauseMessage() {
  attron(COLOR_PAIR(3));
  mvprintw((FIELD_OFFSET_Y + FIELD_H / 2), FIELD_OFFSET_X + FIELD_W - 2,
           "PAUSE");
  attroff(COLOR_PAIR(3));
}

void renderGUI(GameInfo_t game_info) {
  clear();
  printGameField(game_info);
  printGameInfo(game_info);
  printControls();
  if (game_info.pause == PauseGame) {
    printPauseMessage();
  } else if (game_info.pause == EndGame) {
    if (game_info.score >= SNAKE_MAX_LENGTH) {
      attron(COLOR_PAIR(7));
      mvprintw(FIELD_OFFSET_Y + FIELD_H / 2, FIELD_OFFSET_X + FIELD_W - 2,
               "YOU WIN!");
      attroff(COLOR_PAIR(7));
    }
  }

  refresh();
}