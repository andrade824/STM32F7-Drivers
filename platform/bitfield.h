/**
 * @author Devon Andrade
 * @created 12/22/2016
 *
 * Macros for manipulating and creating bitfields.
 */
#pragma once

#include <stdint.h>

/**
 * Generate inline functions used for setting/getting bitfields.
 *
 * @param name    Name of the bitfield.
 * @param lsb_pos Position of the least significant bit for this field.
 * @param mask    The bit mask of the field.
 */
#define BIT_FIELD(name, lsb_pos, mask) \
	static inline uint32_t GET_##name(volatile uint32_t reg) { return (reg & (mask)) >> (lsb_pos); } \
	static inline uint32_t SET_##name(uint32_t value) { return (value << (lsb_pos)) & (mask); } \
	static inline uint32_t name() { return (mask); }

/**
 * Generate inline functions used for setting/getting bitfields. This BIT_FIELD
 * variant generates the mask based on the LSB and MSB of the field.
 *
 * @param name    Name of the bitfield.
 * @param lsb_pos Position of the least significant bit for this field.
 * @param msb_pos Position of the most significant bit for this field.
 */
#define BIT_FIELD2(name, lsb_pos, msb_pos) \
	static inline uint32_t name() { return (uint32_t)((1ULL << (msb_pos + 1)) - 1) ^ ((1UL << lsb_pos) - 1); } \
	static inline uint32_t GET_##name(volatile uint32_t reg) { return (reg & (name())) >> (lsb_pos); } \
	static inline uint32_t SET_##name(uint32_t value) { return (value << (lsb_pos)) & (name()); }

/**
 * Perform a read-modify-write operation that clears out the selected fields.
 *
 * @param reg   The register to have its fields cleared.
 * @param masks The mask(s) of the bit fields to clear out. These are generated
 *              by the BIT_FIELD() macro for each defined field.
 */
#define CLEAR_FIELD(reg,masks) (reg &= ~(masks))

/**
 * Perform a read-modify-write operation that sets the selected fields to all
 * ones. This is particularly useful when setting multiple 1-bit wide fields.
 *
 * @param reg   The register to have its fields cleared.
 * @param masks The mask(s) of the bit fields to clear out. These are generated
 *              by the BIT_FIELD() macro for each defined field.
 */
#define SET_FIELD(reg,values) (reg |= (values))
