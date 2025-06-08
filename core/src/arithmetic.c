#include "arithmetic.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

intx_t euclidean(intx_t a, intx_t b, intx_t gcd) {
	uint32_t r_prev_temp[INT_SIZE];
	uint32_t r_temp[INT_SIZE];
	intx_t r = r_temp;
	intx_t r_prev = r_prev_temp;

	intx_transfer(a, r_prev);
	intx_transfer(b, r);

	uint32_t trash[INT_SIZE];

	intx_div(r_prev, r, trash, r_prev);
	intx_t temp = r_prev;
	r_prev = r;
	r = temp;

	while(!intx_is_zero(r)) {	
		intx_div(r_prev, r, trash, r_prev);
		intx_t temp = r_prev;
		r_prev = r;
		r = temp;
	}

	return intx_transfer(r_prev, gcd);
}


intx_t extended_euclidean(intx_t a, intx_t b, intx_t u, intx_t v, intx_t gcd) {
	uint32_t r_prev_temp[INT_SIZE];
	uint32_t r_temp[INT_SIZE];
	intx_t r_prev = r_prev_temp;
	intx_t r = r_temp;

	intx_transfer(b, r);
	intx_transfer(a, r_prev);

	uint32_t q[INT_SIZE];

	intx_div(r_prev, r, q, r_prev);

	uint32_t s_temp[INT_SIZE];
	uint32_t t_temp[INT_SIZE];
	intx_t s = s_temp;
	intx_t t = t_temp;
	intx_t s_prev = u;
	intx_t t_prev = v;

	intx_lshift(s, 32*INT_SIZE, s);
	intx_lshift(t, 32*INT_SIZE, t);
	intx_lshift(s_prev, 32*INT_SIZE, s_prev);
	intx_lshift(t_prev, 32*INT_SIZE, t_prev);

	s_prev[0] = 1;
	t[0] = 1;

	uint32_t trash[INT_SIZE];

	while(!intx_is_zero(r_prev)) {
		intx_t temp = r_prev;
		r_prev = r;
		r = temp;

		intx_sub(s_prev, intx_mul(s, q, trash), s_prev);
		intx_sub(t_prev, intx_mul(t, q, trash), t_prev);

		intx_t temp_s = s_prev;
		s_prev = s;
		s = temp_s;

		intx_t temp_t = t_prev;
		t_prev = t;
		t = temp_t;

		intx_div(r_prev, r, q, r_prev);
	}

	intx_transfer(s, u);
	intx_transfer(t, v);

	return intx_transfer(r, gcd);
}


bool fermat_test(intx_t p, int k) {
	uint32_t one[INT_SIZE];
	uint32_t trash[INT_SIZE];
	uint32_t a[INT_SIZE];

	for(int i = 0; i < INT_SIZE; i++) one[i] = 0;
	one[0] = 1;

	for(int i = 0; i < k; i++) {
		intx_rand(a, true, INT_SIZE/2);

		intx_div(a, p, trash, a); //To make sure a isn't a multiple of p

		if(intx_is_zero(a)) a[0] |= 1;

		intx_pow_mod(a, intx_sub(p, one, trash), p, a);

		if(intx_cmp(a, one) != 0) return false;
	}

	return true;
}


intx_t generate_prime(bool (* primality_test)(intx_t p, int k), int k, intx_t out, uint64_t mask) {
	intx_rand(out, true, INT_SIZE/4);
	out[0] |= 1;
	
	out[0] &= mask;
	
	while(out[0] == 1) {
		intx_rand(out, true, INT_SIZE/4);
		out[0] |= 1;
		out[0] &= mask;
	}

	//printf("%d\n", out[0]);

	while(!primality_test(out, k)) intx_push32(out, 2, out);
	
	//intx_print_h(out);

	return out;
}

intx_t inv_mod(intx_t a, intx_t n, intx_t out) {
	//intx_print_h(n);
	assert(!intx_is_zero(a) && !intx_is_zero(n));
	
	uint32_t gcd[INT_SIZE];
	uint32_t trash[INT_SIZE];
	uint32_t inv[INT_SIZE];

	extended_euclidean(a, n, inv, trash, gcd);
	gcd[0]--;
	assert(intx_is_zero(gcd));
	
	if(intx_is_negative(inv)) intx_add(inv, n, inv);

	//intx_div(inv, n, trash, inv);

	intx_transfer(inv, out);

	return out;
}


intx_t select_random_coprime(intx_t n, intx_t out) {
	intx_rand(out, true, INT_SIZE/2);

	uint32_t trash[INT_SIZE];
	uint32_t gcd[INT_SIZE];
	uint32_t one[INT_SIZE];

	intx_lshift(one, 32*INT_SIZE, one);
	one[0] = 1;

	euclidean(n, out, gcd);
	while(intx_cmp(gcd, one) != 0){
		//if(intx_cmp(gcd, n) == 0 || intx_cmp(gcd, out) == 0) intx_rand(out, true, INT_SIZE/2);
		//else 
		intx_div(out, gcd, out, trash);
		euclidean(n, out, gcd);
	}

	return out;
}
