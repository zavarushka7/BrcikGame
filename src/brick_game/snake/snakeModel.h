#ifndef SNAKEMODEL_H
#define SNAKEMODEL_H

#include <ctime>
#include <fstream>
#include <memory>
#include <random>
#include <vector>

#include "../brick_game.h"

namespace s21 {
enum Direction { UP, DOWN, LEFT, RIGHT };
class SnakeModel {
 public:
  SnakeModel();
  ~SnakeModel();

  void cleanupField();
  void cleanupNext();
  void resetTimer();
  bool shouldUpdateGame();
  GameInfo_t updateCurrentState();
  void updateGame();
  State getGameState() const;
  void startGame();
  void pauseGame();
  void requestExit();
  bool shouldExit() const;
  void resetGame();
  unsigned long long getTimeUntilStep();
  unsigned long long getCurrentTime();
  void saveHighScore();
  void processMovement(UserAction_t action);
  bool isPaused() const;
  void initializeState();
  void setAccelerate(bool hold);

  Direction getDirection() const { return current_direction_; }

#ifdef TESTING
  Direction getCurrentDirectionForTest() const { return current_direction_; }
  const auto& getSnakeBodyForTest() const { return snake_body; }
  const auto& getApplePositionForTest() const { return apple_position; }
  void testMoveSnake() { moveSnake(); }
  void testUpdateLevel() { updateLevel(); }
  void testUpdateScore() { updateScore(); }
#endif

 private:
  Direction current_direction_;
  struct TimerState {
    bool accelerate = false;
    unsigned long long start_time = 0;
    unsigned long long time_until_step = 0;
    unsigned long long speed = SPEED;
  };
  TimerState timer_state_;
  GameInfo_t current_state_{};
  bool exit_requested_ = false;
  void moveSnake();
  void updateScore();
  void spawnApple();
  void updateLevel();
  std::vector<std::pair<int, int>> snake_body;
  std::pair<int, int> apple_position;
  State game_state_ = SelectGame;
  bool should_exit_ = false;
};
}  // namespace s21

#endif