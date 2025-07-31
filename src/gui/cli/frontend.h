#ifndef FRONTEND_H_
#define FRONTEND_H_

#include <ncurses.h>

#include "../../brick_game/brick_game.h"

#define ENTER '\n'

#define FIELD_H 20
#define FIELD_W 10
#define FIELD_OFFSET_X 5
#define FIELD_OFFSET_Y 5
#define INFO_OFFSET_X (FIELD_OFFSET_X + FIELD_W * 2 + 4)

void initializeGUI();
void initColors();
void cleanupGUI();
void printControls();
void printGameField(GameInfo_t game_info);
void printGameInfo(GameInfo_t game_info);
void printPauseMessage();
void renderGUI(GameInfo_t game_info);

#endif