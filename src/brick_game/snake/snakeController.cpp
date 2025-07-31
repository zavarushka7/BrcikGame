#include "snakeController.h"

using namespace s21;
Game SnakeController::getSelectedGame() const { return selected_game_; }

State SnakeController::getGameState() const {
  if (selected_game_ == Snake) {
    return model_->getGameState();
  } else {
    return tetris_state_;
  }
}

bool SnakeController::shouldExit() const { return model_->shouldExit(); }

void SnakeController::updateGame() {
  if (selected_game_ == Snake) {
    model_->updateGame();
  }
}

void SnakeController::processUserInput(int key) {
  if (key != ERR) {
    UserAction_t action = convertKeyToAction(key);
    userInput(action, false);
  }
}

unsigned long long SnakeController::getTimeUntilStep() const {
  if (selected_game_ == Snake) {
    return model_->getTimeUntilStep();
  } else {
    return ::getTimeUntilStep();
  }
}

void SnakeController::setCurrentGame(Game game) {
  selected_game_ = game;
  if (game == Tetris) {
    ::userInput(Start, false);
    tetris_state_ = SelectGame;
  } else {
    model_->initializeState();
  }
}

GameInfo_t SnakeController::getCurrentState() {
  if (selected_game_ == Snake) {
    return model_->updateCurrentState();
  } else {
    return ::updateCurrentState();
  }
}

void SnakeController::switchGame() {
  selected_game_ = (selected_game_ == Tetris) ? Snake : Tetris;
  setCurrentGame(selected_game_);
}

void SnakeController::userInputSnake(UserAction_t action, bool hold) {
  (void)hold;
  switch (model_->getGameState()) {
    case SelectGame:
      if (action == Start) {
        model_->startGame();
        model_->resetTimer();
      } else if (action == Right || action == Left) {
        switchGame();
      }
      break;
    case StartGame:
      if (action == Pause) {
        model_->pauseGame();
      } else if (!model_->isPaused()) {
        if (action == Left || action == Right || action == Up ||
            action == Down || action == Action) {
          model_->processMovement(action);
        }
      }
      break;
    case EndGame:
    case WinGame:
      if (action == Start) {
        model_->startGame();
        model_->resetTimer();
      }
      break;
    default:
      break;
  }
}

void SnakeController::userInputTetris(UserAction_t action, bool hold) {
  if (tetris_state_ == SelectGame) {
    if (action == Start) {
      ::userInput(Start, false);
      tetris_state_ = StartGame;
    } else if (action == Right || action == Left) {
      switchGame();
    }
  } else {
    ::userInput(action, hold);
    GameInfo_t tetris_info = ::updateCurrentState();
    switch (tetris_info.pause) {
      case SelectGame:
        tetris_state_ = SelectGame;
        break;
      case StartGame:
        tetris_state_ = StartGame;
        break;
      case PauseGame:
        tetris_state_ = PauseGame;
        break;
      case EndGame:
      case RetryGame:
        tetris_state_ = EndGame;
        break;
      case WinGame:
        tetris_state_ = WinGame;
        break;
      default:
        tetris_state_ = SelectGame;
        break;
    }
    if (tetris_info.field) {
      for (int i = 0; i < HEIGHT; i++) {
        delete[] tetris_info.field[i];
      }
      delete[] tetris_info.field;
    }
    if (tetris_info.next) {
      for (int i = 0; i < 4; i++) {
        delete[] tetris_info.next[i];
      }
      delete[] tetris_info.next;
    }
  }
}

void SnakeController::userInput(UserAction_t action, bool hold) {
  if (action == Terminate) {
    model_->requestExit();
    model_->saveHighScore();
    if (selected_game_ == Tetris) {
      ::userInput(Terminate, false);
    }

  } else {
    if (selected_game_ == Snake) {
      userInputSnake(action, hold);
    } else {
      userInputTetris(action, hold);
    }
  }
}

UserAction_t SnakeController::convertKeyToAction(int key) const {
  switch (key) {
    case KEY_UP:
      return Up;
    case KEY_DOWN:
      return Down;
    case KEY_LEFT:
      return Left;
    case KEY_RIGHT:
      return Right;
    case ' ':
      return Action;
    case '\n':
      return Start;
    case PAUSE:
      return Pause;
    case TERMINATE:
      return Terminate;
    default:
      return Start;
  }
}