/*
 * stream_process.c
 *
 *  Created on: Mar 13, 2025
 *      Author: lam
 */

#include "stream_process.h"
#include <string.h>

#define BOARD_SIZE 8

int x_center = (480 / 2) - (4 * 24);
int y_center = (272 / 2) - (4 * 24);
int lm_col_from, lm_row_from, lm_col_to, lm_row_to;

static char previous_board[BOARD_SIZE][BOARD_SIZE];

void draw_board(const char piece, int row, int col) {
	if (previous_board[row][col] == piece)
		return;
	BSP_LCD_SetFont(PIECE_FONT);

	// Définir la taille des cases
	const int SQUARE_SIZE = PIECE_FONT_SIZE;

	// Déterminer la couleur de la case
	uint32_t square_color;

	if (row == lm_row_from && col == lm_col_from)
		square_color = LCD_COLOR_DARKGREEN;
	else if (row == lm_row_to && col == lm_col_to)
		square_color = LCD_COLOR_GREEN;
	else
		square_color =
				((row + col) % 2 == 0) ? LCD_COLOR_LIGHTGRAY : LCD_COLOR_BROWN;

	// Dessiner le carré de la case
	BSP_LCD_SetBackColor(square_color);
	BSP_LCD_FillRect(col * SQUARE_SIZE + x_center, row * SQUARE_SIZE + y_center,
			SQUARE_SIZE, SQUARE_SIZE);

	// Déterminer la couleur de la pièce
	char display_piece = piece;
	if (islower(piece)) {
		BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
		display_piece = toupper(piece);
	} else {
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	}

	// Afficher la pièce
	char piece_str[2] = { display_piece, '\0' };
	BSP_LCD_DisplayStringAt(col * SQUARE_SIZE + x_center,
			row * SQUARE_SIZE + y_center, (uint8_t*) piece_str, LEFT_MODE);
	previous_board[row][col] = piece;
}

void parse_fen(const char *fen) {
	int row = 0, col = 0;

	while (*fen && *fen != ' ') {
		if (*fen == '/') {
			row++;
			col = 0;
		} else if (*fen >= '1' && *fen <= '8') {
			int empty_squares = *fen - '0';
			for (int i = 0; i < empty_squares; i++) {
				draw_board(' ', row, col);
				col++;
			}
		} else {
			draw_board(*fen, row, col);
			col++;
		}
		fen++;
	}
}

void process_stream(const char *buffer) {
	const char *lm_start = strstr(buffer, "lm\":\"");
	if (lm_start) {
		lm_start += 5;
		const char *lm_end = strchr(lm_start, '"');
		if (lm_end) {
			char lm[128];
			int lm_length = lm_end - lm_start;
			strncpy(lm, lm_start, lm_length);
			lm[lm_length] = '\0';
			//debug_log(0, (char*)lm, 0);
			if (strlen(lm) == 4) { // Vérifier que la chaîne contient exactement 4 caractères
				if (lm_row_from >= 0 && lm_col_from >= 0) {
					draw_board('!', lm_row_from, lm_col_from);
				}
				if (lm_row_to >= 0 && lm_col_to >= 0) {
					draw_board('!', lm_row_to, lm_col_to);
				}
				lm_col_from = lm[0] - 'a';
				lm_row_from = 7 - (lm[1] - '1');
				lm_col_to = lm[2] - 'a';
				lm_row_to = 7 - (lm[3] - '1');
				//draw_board('f', file_from, rank_from);

			} else {
				debug_log(0, "Error : invalid move", 1);
			}
		}
	}
	const char *fen_start = strstr(buffer, "\"fen\":\"");
	if (fen_start) {
		fen_start += 7; // Sauter "fen":"
		const char *fen_end = strchr(fen_start, '"');
		if (fen_end) {
			char fen[128];
			int fen_length = fen_end - fen_start;
			strncpy(fen, fen_start, fen_length);
			fen[fen_length] = '\0';
			//debug_log(0, (char*)fen, 0);
			parse_fen(fen);
		}
	}

	const char *w_turn = strstr(buffer, " w ");
	const char *b_turn = strstr(buffer, " b ");

	char wc[128] = "";
	char bc[128] = "";

	// Extraction du timer des blancs ("wc")
	const char *wc_start = strstr(buffer, "\"wc\":");
	if (wc_start) {
	    wc_start += 5;
	    const char *wc_end = strchr(wc_start, ',');
	    if (wc_end) {
	        int wc_length = wc_end - wc_start;
	        strncpy(wc, wc_start, wc_length);
	        wc[wc_length] = '\0';
	    }
	}

	// Extraction du timer des noirs ("bc")
	const char *bc_start = strstr(buffer, "\"bc\":");
	if (bc_start) {
	    bc_start += 5;
	    const char *bc_end = strchr(bc_start, '}');
	    if (bc_end) {
	        int bc_length = bc_end - bc_start;
	        strncpy(bc, bc_start, bc_length);
	        bc[bc_length] = '\0';
	    }
	}

	// Affichage en fonction du joueur actif
	if (w_turn) {
	    BSP_LCD_SetTextColor(LCD_COLOR_DARKGRAY);
	    BSP_LCD_FillRect(0, (272 / 2) - 50, 100, 100);
	    BSP_LCD_SetBackColor(LCD_COLOR_DARKGRAY);
	    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	    BSP_LCD_DisplayStringAt(24, (272 / 2) - 12, (uint8_t*)wc, LEFT_MODE);

	    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	    BSP_LCD_FillRect(380, (272 / 2) - 50, 100, 100);
	    BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
	    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	    BSP_LCD_DisplayStringAt(24, (272 / 2) - 12, (uint8_t*)bc, RIGHT_MODE);
	}
	else if (b_turn) {
	    BSP_LCD_SetTextColor(LCD_COLOR_DARKGRAY);
	    BSP_LCD_FillRect(380, (272 / 2) - 50, 100, 100);
	    BSP_LCD_SetBackColor(LCD_COLOR_DARKGRAY);
	    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	    BSP_LCD_DisplayStringAt(24, (272 / 2) - 12, (uint8_t*)bc, RIGHT_MODE);

	    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	    BSP_LCD_FillRect(0, (272 / 2) - 50, 100, 100);
	    BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
	    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	    BSP_LCD_DisplayStringAt(24, (272 / 2) - 12, (uint8_t*)wc, LEFT_MODE);
	}



	const char *w_player_start = strstr(buffer,
			"[{\"color\":\"white\",\"user\":{\"name\":\"");
	if (w_player_start) {
		w_player_start += 34;
		const char *w_player_end = strchr(w_player_start, '"');
		if (w_player_end) {
			char w_player[128];
			int w_player_length = w_player_end - w_player_start;
			strncpy(w_player, w_player_start, w_player_length);
			w_player[w_player_length] = '\0';
			BSP_LCD_SetFont(&Font12);
			BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
			BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
			BSP_LCD_DisplayStringAt(0, 0, (uint8_t*) w_player, LEFT_MODE);
		}
	}

	const char *b_player_start = strstr(buffer,
			"},{\"color\":\"black\",\"user\":{\"name\":\"");
	if (b_player_start) {
		b_player_start += 35; // Sauter jusqu'au début du nom
		const char *b_player_end = strchr(b_player_start, '"');
		if (b_player_end) {
			char b_player[128];
			int b_player_length = b_player_end - b_player_start;
			strncpy(b_player, b_player_start, b_player_length);
			b_player[b_player_length] = '\0';
			// Affichage du nom du joueur noir
			BSP_LCD_SetFont(&Font12);
			BSP_LCD_SetBackColor(LCD_COLOR_BROWN);
			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DisplayStringAt(0, 0, (uint8_t*) b_player, RIGHT_MODE);
		}
	}
}

void init_board_state() {
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			previous_board[i][j] = ' ';
			draw_board('!', i, j);
		}
	}
}

