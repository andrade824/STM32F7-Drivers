/**
 * @author Devon Andrade
 * @created 12/24/2018
 *
 * Font data for various sizes of the Courier New font.
 */
#pragma once

#ifdef INCLUDE_FONT_TABLE

#include "config.h"

#include <stdint.h>

/**
 * The size of the font in pixels.
 *
 * Only 6pt, 9pt, and 12pt fonts are currently supported.
 */
#if FONT_CONFIG == 6
	#define FONT_WIDTH 5U
	#define FONT_HEIGHT 8U
#elif FONT_CONFIG == 9
	#define FONT_WIDTH 7U
	#define FONT_HEIGHT 12U
#elif FONT_CONFIG == 12
	#define FONT_WIDTH 11U
	#define FONT_HEIGHT 16U
#else
	#error Unsupported font size selected in config file.
#endif /* FONT_CONFIG selection */

/* The total number of characters in the font table. */
#define FONT_TOTAL_CHARS 95U

/* The number of 8-bit chunks that make up a single line of a font character. */
#define FONT_LINE_LENGTH ((FONT_WIDTH + 7) / 8)

/**
 * The font table supports rendering ASCII characters 32 through 126. When
 * converting ASCII characters to an index into this table, subtract this value.
 */
#define FONT_ASCII_OFFSET 32U

/**
 * Given an ASCII value, generate an index into the font table that points to
 * the first byte that represents that character.
 */
#define FONT_ASCII_INDEX(x) ((x - FONT_ASCII_OFFSET) * FONT_LINE_LENGTH * FONT_HEIGHT)

extern const uint8_t font_table[];

#endif /* INCLUDE_FONT_TABLE */
