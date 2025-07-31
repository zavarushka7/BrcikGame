#include "snake/consoleView.h"
using namespace s21;
int main() {
  SnakeModel model;
  SnakeController controller(&model);
  ConsoleView view(&controller);
  view.brickGame();
  return 0;
}