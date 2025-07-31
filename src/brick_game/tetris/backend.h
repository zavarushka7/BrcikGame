#ifndef S21_BRICK_GAME_TETRIS_BACKEND_H_
#define S21_BRICK_GAME_TETRIS_BACKEND_H_

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../brick_game.h"

#define SPEED_STEP 30
#define NEW_LEVEL_THRESHOLD 600
#define MAX_LEVEL 10

typedef enum {
  I_BLOCK,
  L_BLOCK,
  J_BLOCK,
  O_BLOCK,
  Z_BLOCK,
  T_BLOCK,
  S_BLOCK
} Block_t;

typedef enum {
  Starting,
  Spawning,
  Moving,
  Contacting,
  Paused,
  Ending
} Status_t;

typedef struct {
  Status_t status;
  unsigned long long start_time;
  unsigned long long time_until_step;
  int **field;
  int block_size;
  int x;
  int y;
  int **block;
  int score;
  int level;
  int speed;
  int next_block_size;
  int pause;
  int **next_block;
  bool terminate_requested;
} State_t;

int **createField(int h, int w);
void copyField(int **dest, int **src, int h, int w);
void freeField(int **field, int size);
unsigned long long getCurrentTime(void);
void initializeState(void);
void startGame(void);
void pauseGame(void);
void endGame(void);
void rotate(int **new_block, int **old_block, int size);
int **generateNewBlock(int *block_size);
void spawn(void);
void moveLeft(void);
void moveRight(void);
void shift(void);
int isBlockContacted(void);
void contact(void);
int canRotateBlock(int **new_block);
void rotateBlock(void);
void saveHighScore(void);
void updateLevel(void);
void deleteLines(void);
void updateGame(void);
State_t *getCurrentState(void);

#ifdef __cplusplus
extern "C" {
#endif
unsigned long long getTimeUntilStep();
#ifdef __cplusplus
}
#endif

#endif  // S21_BRICK_GAME_TETRIS_BACKEND