#ifndef SNAKECONTROLLER_H
#define SNAKECONTROLLER_H

#ifdef __cplusplus
extern "C" {
#endif
#include "../brick_game.h"
#ifdef __cplusplus
}
#endif

#include "../tetris/backend.h"
#include "ncurses.h"
#include "snakeModel.h"

#define ENTER '\n'
#define TERMINATE 27
#define PAUSE 'p'
#define ACTION ' '

namespace s21 {
class SnakeController {
 public:
  SnakeController(SnakeModel *m) : model_(m), tetris_state_(SelectGame) {}
  void updateGame();
  Game getSelectedGame() const;
  State getGameState() const;
  bool shouldExit() const;
  void processUserInput(int key);
  void userInput(UserAction_t action, bool hold);
  GameInfo_t getCurrentState();
  unsigned long long getTimeUntilStep() const;
  void setCurrentGame(Game game);
  void switchGame();
  void userInputSnake(UserAction_t action, bool hold);
  void userInputTetris(UserAction_t action, bool hold);

 private:
  SnakeModel *model_;
  Game selected_game_ = Snake;
  State tetris_state_;
  UserAction_t convertKeyToAction(int key) const;
};
}  // namespace s21

#endif