#ifndef CONSOLEVIEW_H
#define CONSOLEVIEW_H

#include "../../gui/cli/frontend.h"
#include "ncurses.h"
#include "snakeController.h"

namespace s21 {
class ConsoleView {
 public:
  ConsoleView(SnakeController *c) : controller_(c) {};
  void brickGame();

 private:
  SnakeController *controller_;
  void displayGameSelectionMenu();
  void displayGameOverScreen(GameInfo_t state);
};

}  // namespace s21

#endif