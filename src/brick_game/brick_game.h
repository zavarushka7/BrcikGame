#ifndef BRICKGAME_H
#define BRICKGAME_H

#define WIDTH 10
#define HEIGHT 20
#define SPEED 300
#define SNAKE_LEVEL_THRESHOLD 5
#define SNAKE_MAX_LEVEL 10
#define SNAKE_SPEED_STEP 30
#define SNAKE_MAX_LENGTH 200
#include <stdbool.h>

typedef enum {
  SelectGame,
  StartGame,
  PauseGame,
  EndGame,
  WinGame,
  RetryGame
} State;

typedef enum { Tetris, Snake } Game;

typedef enum {
  Start,
  Pause,
  Terminate,
  Left,
  Right,
  Up,
  Down,
  Action
} UserAction_t;

typedef struct {
  int **field;
  int **next;
  int score;
  int high_score;
  int level;
  int speed;
  int pause;
} GameInfo_t;

#ifdef __cplusplus
extern "C" {
#endif

void userInput(UserAction_t action, bool hold);
GameInfo_t updateCurrentState();

#ifdef __cplusplus
}
#endif

#endif