#include <gtest/gtest.h>

#include <fstream>

#include "../brick_game/brick_game.h"
#include "../brick_game/snake/consoleView.h"
#include "../brick_game/snake/snakeController.h"
#include "../brick_game/snake/snakeModel.h"

namespace s21 {
class SnakeModelTest : public ::testing::Test {
 protected:
  void SetUp() override { model = std::make_unique<SnakeModel>(); }

  void TearDown() override { model.reset(); }

  std::unique_ptr<SnakeModel> model;
};

TEST(SnakeControllerTest, UserInputkmTetrisAllStates) {
  SnakeModel model;
  SnakeController controller(&model);
  controller.setCurrentGame(Tetris);

  auto createGameInfo = [](State pause_state) -> GameInfo_t {
    GameInfo_t info = {};
    info.pause = pause_state;
    info.field = new int*[HEIGHT];
    for (int i = 0; i < HEIGHT; ++i) {
      info.field[i] = new int[WIDTH]();
    }
    info.next = new int*[4];
    for (int i = 0; i < 4; ++i) {
      info.next[i] = new int[4]();
    }
    return info;
  };

  auto cleanupGameInfo = [](GameInfo_t& info) {
    if (info.field) {
      for (int i = 0; i < HEIGHT; i++) delete[] info.field[i];
      delete[] info.field;
    }
    if (info.next) {
      for (int i = 0; i < 4; i++) delete[] info.next[i];
      delete[] info.next;
    }
  };

  controller.userInputTetris(Start, false);
  EXPECT_EQ(controller.getGameState(), StartGame);
  controller.userInputTetris(Left, false);
  controller.setCurrentGame(Tetris);
  controller.userInputTetris(Right, false);
  EXPECT_EQ(controller.getSelectedGame(), Snake);
  controller.setCurrentGame(Tetris);

  controller.userInputTetris(Start, false);
  controller.userInputTetris(Pause, false);
  EXPECT_EQ(controller.getGameState(), PauseGame);
  controller.setCurrentGame(Tetris);
  controller.userInputTetris(Left, false);
  EXPECT_EQ(controller.getGameState(), SelectGame);

  controller.userInputTetris(Start, false);
  controller.userInputTetris(Start, false);

  GameInfo_t info = createGameInfo(EndGame);

  controller.userInputTetris(Action, false);
  controller.userInputTetris(Start, false);
  info = createGameInfo(EndGame);
  controller.userInputTetris(Action, false);
  cleanupGameInfo(info);

  info = createGameInfo(RetryGame);
  controller.userInputTetris(Action, false);
  cleanupGameInfo(info);

  info = createGameInfo(WinGame);
  controller.userInputTetris(Action, false);
  cleanupGameInfo(info);

  info = createGameInfo(static_cast<State>(999));
  controller.userInputTetris(Action, false);
  cleanupGameInfo(info);
  EXPECT_EQ(controller.getGameState(), SelectGame);

  info = createGameInfo(StartGame);
  controller.userInputTetris(Action, false);
  cleanupGameInfo(info);
}
TEST(SnakeControllerTest, UserInputSnakeAllActions) {
  SnakeModel model;
  SnakeController controller(&model);

  controller.userInputSnake(Start, false);
  EXPECT_EQ(model.getGameState(), StartGame);

  controller.userInputSnake(Right, false);
  controller.switchGame();
  controller.userInputSnake(Left, false);
  EXPECT_EQ(controller.getSelectedGame(), Tetris);

  controller.userInputSnake(Start, false);
  controller.userInputSnake(Pause, false);
  EXPECT_TRUE(model.isPaused());

  controller.userInputSnake(Up, false);
  EXPECT_EQ(model.getDirection(), RIGHT);

  controller.userInputSnake(Pause, false);
  controller.userInputSnake(Up, false);
  EXPECT_EQ(model.getDirection(), UP);

  controller.userInputSnake(Left, false);
  EXPECT_EQ(model.getDirection(), LEFT);

  controller.userInputSnake(Action, false);
}

TEST(SnakeControllerTest, UserInputSnakeSwitchGame) {
  SnakeModel model;
  SnakeController controller(&model);

  controller.userInputSnake(Right, false);
  EXPECT_EQ(controller.getSelectedGame(), Tetris);

  controller.userInputSnake(Left, false);
  EXPECT_EQ(controller.getSelectedGame(), Snake);
}

TEST(SnakeControllerTest, UserInputTetrisAllStates) {
  SnakeModel model;
  SnakeController controller(&model);
  controller.setCurrentGame(Tetris);

  controller.userInputTetris(Start, false);
  EXPECT_EQ(controller.getGameState(), StartGame);

  controller.setCurrentGame(Tetris);
  controller.userInputTetris(Left, false);
  EXPECT_EQ(controller.getSelectedGame(), Snake);

  controller.setCurrentGame(Tetris);
  controller.userInputTetris(Start, false);
  controller.userInputTetris(Pause, false);

  controller.userInputTetris(Up, false);
  controller.userInputTetris(Down, false);
  controller.userInputTetris(Left, false);
  controller.userInputTetris(Right, false);
  controller.userInputTetris(Action, false);

  GameInfo_t info = controller.getCurrentState();
  info.field = new int*[HEIGHT];
  for (int i = 0; i < HEIGHT; ++i) {
    info.field[i] = new int[WIDTH]();
  }
  info.next = new int*[4];
  for (int i = 0; i < 4; ++i) {
    info.next[i] = new int[4]();
  }

  info.pause = SelectGame;
  controller.userInputTetris(Start, false);

  info.pause = StartGame;
  controller.userInputTetris(Start, false);

  info.pause = EndGame;
  controller.userInputTetris(Start, false);

  info.pause = RetryGame;
  controller.userInputTetris(Start, false);

  info.pause = WinGame;
  controller.userInputTetris(Start, false);

  info.pause = static_cast<State>(999);
  controller.userInputTetris(Start, false);

  if (info.field) {
    for (int i = 0; i < HEIGHT; i++) delete[] info.field[i];
    delete[] info.field;
  }
  if (info.next) {
    for (int i = 0; i < 4; i++) delete[] info.next[i];
    delete[] info.next;
  }
}

TEST(SnakeControllerTest, TerminateAction) {
  SnakeModel model;
  SnakeController controller(&model);

  controller.userInput(Terminate, false);
  EXPECT_TRUE(controller.shouldExit());

  controller.setCurrentGame(Tetris);
  controller.userInput(Terminate, false);
  EXPECT_TRUE(controller.shouldExit());
}

TEST(SnakeControllerTest, Initialization) {
  SnakeModel model;
  SnakeController controller(&model);

  EXPECT_EQ(controller.getSelectedGame(), Snake);
  EXPECT_EQ(controller.getGameState(), SelectGame);
  EXPECT_FALSE(controller.shouldExit());
}

TEST(SnakeControllerTest, GameSelection) {
  SnakeModel model;
  SnakeController controller(&model);

  controller.switchGame();
  EXPECT_EQ(controller.getSelectedGame(), Tetris);

  controller.switchGame();
  EXPECT_EQ(controller.getSelectedGame(), Snake);
}

TEST(SnakeControllerTest, UserInputProcessing) {
  SnakeModel model;
  SnakeController controller(&model);

  controller.userInput(Start, false);
  EXPECT_EQ(controller.getGameState(), StartGame);

  controller.userInput(Pause, false);
  EXPECT_TRUE(model.isPaused());
}

TEST(SnakeControllerTest, GameStateManagement) {
  SnakeModel model;
  SnakeController controller(&model);

  controller.userInput(Start, false);
  EXPECT_EQ(controller.getGameState(), StartGame);

  controller.userInput(Pause, false);
  EXPECT_EQ(controller.getGameState(), StartGame);

  model.requestExit();
  EXPECT_TRUE(controller.shouldExit());
}

TEST(SnakeControllerTest, TimeManagement) {
  SnakeModel model;
  SnakeController controller(&model);

  unsigned long long time = controller.getTimeUntilStep();

  controller.userInput(Start, false);
  unsigned long long new_time = controller.getTimeUntilStep();
  EXPECT_NE(time, new_time);
}

TEST(SnakeControllerTest, TimeManagementTetris) {
  SnakeModel model;
  SnakeController controller(&model);
  controller.setCurrentGame(Tetris);

  unsigned long long time = controller.getTimeUntilStep();
  EXPECT_GE(time, 0);
}

TEST(SnakeControllerTest, UpdateGameTetris) {
  SnakeModel model;
  SnakeController controller(&model);
  controller.setCurrentGame(Tetris);

  controller.updateGame();
}

TEST(SnakeControllerTest, View) {
  SnakeModel model;
  SnakeController controller(&model);
  EXPECT_EQ(model.getGameState(), SelectGame);
  controller.processUserInput(KEY_DOWN);
  controller.processUserInput(KEY_LEFT);
  controller.processUserInput(KEY_RIGHT);
  controller.processUserInput(' ');
  controller.processUserInput('\n');
  controller.processUserInput(PAUSE);
  controller.processUserInput(TERMINATE);
}

TEST(SnakeControllerTest, UpdateGame) {
  SnakeModel model;
  SnakeController controller(&model);
  controller.updateGame();
  controller.processUserInput(KEY_UP);
}

TEST_F(SnakeModelTest, Initialization) {
  EXPECT_EQ(model->getGameState(), SelectGame);
  EXPECT_FALSE(model->isPaused());
  EXPECT_FALSE(model->shouldExit());
  EXPECT_EQ(model->getDirection(), RIGHT);
  EXPECT_EQ(model->getSnakeBodyForTest().size(), 4);
}

TEST_F(SnakeModelTest, GameStartAndReset) {
  model->startGame();
  EXPECT_EQ(model->getGameState(), StartGame);
  model->resetGame();
  EXPECT_EQ(model->getGameState(), SelectGame);
  EXPECT_EQ(model->getSnakeBodyForTest().size(), 4);
  EXPECT_EQ(model->getCurrentDirectionForTest(), RIGHT);
}

TEST_F(SnakeModelTest, Pause) {
  model->startGame();
  EXPECT_FALSE(model->isPaused());
  model->pauseGame();
  EXPECT_TRUE(model->isPaused());
  model->pauseGame();
  EXPECT_FALSE(model->isPaused());
}

TEST_F(SnakeModelTest, Move) {
  model->startGame();
  EXPECT_EQ(model->getDirection(), RIGHT);
  model->processMovement(Up);
  EXPECT_EQ(model->getDirection(), UP);
  model->processMovement(Right);
  EXPECT_EQ(model->getDirection(), RIGHT);
  model->processMovement(Down);
  EXPECT_EQ(model->getDirection(), DOWN);
  model->processMovement(Left);
  EXPECT_EQ(model->getDirection(), LEFT);
  model->processMovement(Down);
  EXPECT_EQ(model->getDirection(), DOWN);
  model->processMovement(Right);
  EXPECT_EQ(model->getDirection(), RIGHT);
  model->processMovement(Left);
  EXPECT_EQ(model->getDirection(), RIGHT);
}

TEST_F(SnakeModelTest, AppleSpawn) {
  model->startGame();
  auto state = model->updateCurrentState();

  bool appleFound = false;
  for (int i = 0; i < HEIGHT; i++) {
    for (int j = 0; j < WIDTH; j++) {
      if (state.field[i][j] == 2) {
        appleFound = true;
        break;
      }
    }
    if (appleFound) break;
  }
  EXPECT_TRUE(appleFound);

  if (state.field) {
    for (int i = 0; i < HEIGHT; i++) delete[] state.field[i];
    delete[] state.field;
  }
  if (state.next) {
    for (int i = 0; i < 4; i++) delete[] state.next[i];
    delete[] state.next;
  }
}

TEST_F(SnakeModelTest, UpdateGame_PausedNoMovement) {
  model->startGame();
  model->pauseGame();

  GameInfo_t initial_state = model->updateCurrentState();
  model->setAccelerate(true);
  while (model->getTimeUntilStep() > 0) {
  }

  model->updateGame();

  GameInfo_t new_state = model->updateCurrentState();
  for (int i = 0; i < HEIGHT; ++i) {
    for (int j = 0; j < WIDTH; ++j) {
      EXPECT_EQ(new_state.field[i][j], initial_state.field[i][j]);
    }
  }

  if (initial_state.field) {
    for (int i = 0; i < HEIGHT; i++) delete[] initial_state.field[i];
    delete[] initial_state.field;
  }
  if (initial_state.next) {
    for (int i = 0; i < 4; i++) delete[] initial_state.next[i];
    delete[] initial_state.next;
  }
  if (new_state.field) {
    for (int i = 0; i < HEIGHT; i++) delete[] new_state.field[i];
    delete[] new_state.field;
  }
  if (new_state.next) {
    for (int i = 0; i < 4; i++) delete[] new_state.next[i];
    delete[] new_state.next;
  }
}

TEST_F(SnakeModelTest, MoveSnakeUpdatesPositionCorrectly) {
  model->startGame();
  model->processMovement(Right);
  EXPECT_EQ(model->getCurrentDirectionForTest(), RIGHT);

  auto initial_body = model->getSnakeBodyForTest();
  auto initial_head = initial_body.front();
  model->testMoveSnake();

  auto new_body = model->getSnakeBodyForTest();
  auto new_head = new_body.front();

  EXPECT_EQ(new_head.first, initial_head.first);
  EXPECT_EQ(new_head.second, initial_head.second + 1);
  EXPECT_EQ(new_body.size(), initial_body.size());

  model->testUpdateLevel();
  model->testUpdateScore();
}

TEST_F(SnakeModelTest, TimeUntilStep) {
  model->startGame();
  auto initial_time = model->getTimeUntilStep();
  model->setAccelerate(true);
  auto accelerated_time = model->getTimeUntilStep();
  EXPECT_LE(accelerated_time, initial_time);
}

TEST_F(SnakeModelTest, ShouldUpdateGame) {
  model->startGame();
  while (model->getTimeUntilStep() > 0) {
  }

  model->pauseGame();
  EXPECT_FALSE(model->shouldUpdateGame());

  model->requestExit();
  EXPECT_FALSE(model->shouldUpdateGame());
}

}  // namespace s21

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}