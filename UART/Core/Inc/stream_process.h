/*
 * stream_process.h
 *
 *  Created on: Mar 13, 2025
 *      Author: lam
 */

#ifndef INC_STREAM_PROCESS_H_
#define INC_STREAM_PROCESS_H_

#define PIECE_FONT &Font24
#define PIECE_FONT_SIZE 24

#include "stdint.h"
#include "stm32746g_discovery_lcd.h"

void parse_fen(const char* fen);
void process_stream(const char* buffer);
void draw_board(const char piece, int row, int col);
void init_board_state(void);

#endif /* INC_STREAM_PROCESS_H_ */
