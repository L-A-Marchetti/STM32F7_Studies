/*
 * debug.h
 *
 *  Created on: Mar 12, 2025
 *      Author: lam
 */

#ifndef INC_DEBUG_H_
#define INC_DEBUG_H_

#define DEBUG_FONT &Font12
#define DEBUG_FONT_SIZE 12

#include "stdint.h"
#include "stm32746g_discovery_lcd.h"

void debug_log(uint8_t new_page, const char *message, uint8_t error);

#endif /* INC_DEBUG_H_ */
