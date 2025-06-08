#ifndef __INTX_ARITHMETIC__
#define __INTX_ARITHMETIC__

#include "intx_t.h"
#include <stdbool.h>

// extended euclidean algorithm to get greatest common divisor and the coefficient of the Bezout's decomposition
intx_t extended_euclidean(intx_t a, intx_t b, intx_t u, intx_t v, intx_t gcd);

// Tests to see if the given intx_t integer is probable prime number
bool fermat_test(intx_t p, int k);

// Generates a intx_t integer prime number
intx_t generate_prime(bool (* primality_test) (intx_t p, int k), int k, intx_t out, uint64_t mask);

// Returns b as ab = 1 mod n if a != 0 and n != 0 and a^b = 1
intx_t inv_mod(intx_t a, intx_t n, intx_t out);

intx_t euclidean(intx_t a, intx_t b, intx_t gcd);

intx_t select_random_coprime(intx_t n, intx_t out);

#endif
