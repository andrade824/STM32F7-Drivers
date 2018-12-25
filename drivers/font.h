/**
 * @author Devon Andrade
 * @created 12/24/2018
 *
 * Font data for Courier New 12pt.
 *
 * The Font data is 5 pixels wide by 8 pixels tall. There will usually be a one
 * pixel gap added horizontally and vertically (so 6 pixels by 9 pixels).
 */
#if defined(INCLUDE_FONT_TABLE)
#ifndef FONT_H
#define FONT_H

#include <stdint.h>

/* The size of the font in pixels. */
#define FONT_WIDTH 5U
#define FONT_HEIGHT 8U

/* The total number of characters in the font table. */
#define FONT_TOTAL_CHARS 95U

/**
 * The font table supports rendering ASCII characters 32 through 126. When
 * converting ASCII characters to an index into this table, subtract this value.
 */
#define FONT_ASCII_OFFSET 32U

/**
 * Given an ASCII value, generate an index into the font table that points to
 * the first byte that represents that character.
 */

#define FONT_ASCII_INDEX(x) ((x - FONT_ASCII_OFFSET) * FONT_HEIGHT)

extern const uint8_t font_table[FONT_TOTAL_CHARS * FONT_HEIGHT];

#endif
#endif
