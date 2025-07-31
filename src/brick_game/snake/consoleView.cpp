#include "consoleView.h"

using namespace s21;

void ConsoleView::displayGameSelectionMenu() {
  clear();
  mvprintw(0, 0, "Use <-/-> to choose a game");
  mvprintw(1, 0, "Selected game: %s",
           (controller_->getSelectedGame() == Tetris) ? "Tetris" : "Snake");
  mvprintw(2, 0, "Press Enter to start a game");
  refresh();
}

void ConsoleView::displayGameOverScreen(GameInfo_t state) {
  clear();
  mvprintw(0, 0, "Game Over!");
  mvprintw(1, 0, "Score: %d", state.score);
  mvprintw(2, 0, "High Score: %d", state.high_score);
  mvprintw(3, 0, "Press Enter to restart or Esc to exit");
  refresh();
}

void ConsoleView::brickGame() {
  initializeGUI();
  while (!controller_->shouldExit()) {
    State game_state = controller_->getGameState();

    if (game_state == SelectGame) {
      displayGameSelectionMenu();
    } else {
      GameInfo_t state = controller_->getCurrentState();
      if (game_state == EndGame || game_state == WinGame) {
        displayGameOverScreen(state);
      } else {
        renderGUI(state);
        controller_->updateGame();
      }

      if (state.field) {
        for (int i = 0; i < HEIGHT; i++) delete[] state.field[i];
        delete[] state.field;
      }
      if (state.next) {
        for (int i = 0; i < 4; i++) delete[] state.next[i];
        delete[] state.next;
      }
    }

    unsigned long long time_left = controller_->getTimeUntilStep();
    timeout(time_left == (long long unsigned int)-1
                ? -1
                : std::max(0, static_cast<int>(time_left)));
    int key = getch();
    controller_->processUserInput(key);
  }
  cleanupGUI();
}