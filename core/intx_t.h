#ifndef __INTX__
#define __INTX__


#define INT_SIZE 32

#include <stdint.h>
#include <stdbool.h>

typedef uint32_t* intx_t;

// Converts int64_t to intx_t, needs to be freed
intx_t intx_from_64(int64_t n);

// Prints intx_t as hex
void intx_print_h(intx_t n);

// Adds two intx_t
intx_t intx_add(intx_t a, intx_t b, intx_t out);

// Flips all of the bits of an intx_t integer
intx_t intx_not(intx_t in, intx_t out);

// Substract intx_t integer b from intx_t intger a
intx_t intx_sub(intx_t a, intx_t b, intx_t out);

// Adds a 64bits value to n (permanently affects n)
intx_t intx_push32(intx_t in, uint32_t v, intx_t out);

// Shifts intx_t integer to the left by i bits
intx_t intx_lshift(intx_t in, int p, intx_t out);

// Shifts intx_t integer to the right by i bits
intx_t intx_rshift(intx_t in, int p, intx_t out);

// Detects if a intx_t is negative
bool intx_is_negative(intx_t n);

// Detects if a intx_t is equal to zero
bool intx_is_zero(intx_t n);

// Takes the 2's complement of n
intx_t intx_two_complement(intx_t in, intx_t out);

// Multiply two intx_t together
intx_t intx_mul(intx_t a, intx_t b, intx_t out);

// Divide intx_t int n(umerator) by intx_t int d(enominator)
intx_t intx_div(intx_t n, intx_t d, intx_t q, intx_t r);

// Creates a new memory space that needs to be freed
intx_t intx_copy(intx_t n);

intx_t intx_transfer(intx_t in, intx_t out);

// Set a 0 (false) or 1 (true) at pos i (i = 0 => LSB, i = INT_SIZE*32-1 => MSB) (permanently affects n)
void intx_set_digit(intx_t n, uint32_t i, bool digit);

// Get digit at position i (i = 0 => LSB)
bool intx_get_digit(intx_t n, uint32_t i);

// Compares to intx_t integers, returns 0 if they're both equal, returns 1 if a is bigger then b else -1
int intx_cmp(intx_t a, intx_t b);

// Returns b^p mod n
intx_t intx_pow_mod(intx_t b, intx_t p, intx_t n, intx_t out);

// Returns a new memory address containing a random intx integer
intx_t intx_rand(intx_t out, bool positive, uint16_t chunk_size);

#endif
