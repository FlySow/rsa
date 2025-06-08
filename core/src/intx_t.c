#include "intx_t.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <limits.h>

intx_t intx_from_64(int64_t n) {
	uint32_t* wordx = malloc(sizeof(uint32_t)*INT_SIZE);
	
	wordx[0] = (uint32_t) n;
	wordx[1] = n >> 32;

	for(int i = 2; i < INT_SIZE; i++)
		wordx[i] = n < 0 ? -1 : 0;

	return wordx;
}


intx_t intx_copy(intx_t n) {
	intx_t res = malloc(sizeof(uint32_t)*INT_SIZE);
	for(int i = 0; i < INT_SIZE; i++) res[i] = n[i];
	return res;
}


intx_t intx_transfer(intx_t in, intx_t out) {
	for(int i = 0; i < INT_SIZE; i++) out[i] = in[i];
	return out;
}


void intx_set_digit(intx_t n, uint32_t i, bool digit) {
	assert(i < INT_SIZE*32);
	
	n[i/32] = (n[i/32] & ~(1 << i % 32)) + ((digit ? 1 : 0) << i % 32);
}


bool intx_get_digit(intx_t n, uint32_t i) {
	assert(i < INT_SIZE*32);

	return n[i/32] & (1 << i % 32);
}


void intx_print_h(intx_t n) {
	for(int i = INT_SIZE/2-1; i >= 0; i--)
		printf("%08x", n[i]);
	printf("\n");
}


intx_t intx_push32(intx_t in, uint32_t v, intx_t out) {
	uint32_t carry = 0;
	if(in[0] > UINT_MAX - v) carry = 1;
	out[0]+= v;
	for(int i = 1; i < INT_SIZE; i++) {
		uint32_t temp = 0;
		if(in[i] > UINT_MAX - carry) temp = 1;
		out[i]+= carry;
		carry = temp;
	}
	return out;
}


intx_t intx_add(intx_t a, intx_t b, intx_t out) {
	uint32_t carry = 0;
	for(int i = 0; i < INT_SIZE; i++) {
		uint32_t r = a[i] + b[i] + carry;
		if((a[i] & 0x80000000 && b[i] & 0x80000000) || (!(r & 0x80000000) && (a[i] & 0x80000000 || b[i] & 0x80000000))) carry = 1;
		else carry = 0;
		out[i] = r;
	}
	
	return out;
}


intx_t intx_not(intx_t in, intx_t out) {
	for(int i = 0; i < INT_SIZE; i++) out[i] = ~in[i];
	return out;
}


intx_t intx_two_complement(intx_t in, intx_t out) {
	return intx_push32(intx_not(in, out), 1, out);
}


intx_t intx_sub(intx_t a, intx_t b, intx_t out) {
	uint32_t temp[INT_SIZE];
	intx_add(a, intx_two_complement(b, temp), out);
	return out;
}


intx_t intx_lshift(intx_t in, int p, intx_t out) {
	assert(p >= 0);
	
	int chunk_offset = p / 32;
	int shift_offset = p % 32;

	for(int i = INT_SIZE-1; i > chunk_offset && i >= 0; i--) 
		out[i] = (in[i-chunk_offset] << shift_offset) + (shift_offset != 0 ? in[i-chunk_offset-1] >> (32-shift_offset) : 0);
	if(chunk_offset < INT_SIZE) out[chunk_offset] = in[0] << shift_offset;
	for(int i = 0; i < chunk_offset && i < INT_SIZE; i++) out[i] = 0;
	return out;
}


intx_t intx_rshift(intx_t in, int p, intx_t out) {
	assert(p >= 0);

	int chunk_offset = p / 32;
	int shift_offset = p % 32;
	
	for(int i = 0; i < INT_SIZE-chunk_offset-1; i++) 
		out[i] = (in[i+chunk_offset] >> shift_offset) + (shift_offset != 0 ? in[i+chunk_offset+1] << (32-shift_offset) : 0);
	if(chunk_offset < INT_SIZE) out[INT_SIZE-chunk_offset-1] = in[INT_SIZE-1] >> shift_offset;
	for(int i = INT_SIZE-chunk_offset; i < INT_SIZE; i++) out[i] = 0;
	return out;
}


intx_t intx_mul(intx_t a, intx_t b, intx_t out) {
	bool t_comp = false; //Check wether or not we need to get the two's complement of the result number in the end.

	uint32_t a_temp[INT_SIZE];
	uint32_t b_temp[INT_SIZE];
	if(intx_is_negative(a) && intx_is_negative(b)) {
		a = intx_two_complement(a, a_temp);
		b = intx_two_complement(b, b_temp);
	}else if(intx_is_negative(a)) {
		t_comp = true;
		a = intx_two_complement(a, a_temp);
		b = intx_transfer(b, b_temp);
	}else if(intx_is_negative(b)) {
		t_comp = true;
		a = intx_transfer(a, a_temp);
		b = intx_two_complement(b, b_temp);
	} else {
		a = intx_transfer(a, a_temp);
		b = intx_transfer(b, b_temp);
	}
	
	uint64_t product[INT_SIZE][INT_SIZE];

	uint32_t shift_int[INT_SIZE];

	for(int i = 0; i < INT_SIZE; i++) {
		for(int j = 0; j < INT_SIZE; j++) product[i][j] = (uint64_t) a[i] * (uint64_t) b[j];
		out[i] = 0;
	}

	for(int i = 0; i < INT_SIZE; i++) {
		for(int j = 0; j < INT_SIZE; j++) {
			intx_lshift(shift_int, INT_SIZE*32, shift_int);
			intx_push32(shift_int, product[i][j] >> 32, shift_int);
			intx_lshift(shift_int, 32, shift_int);
			intx_push32(shift_int, (uint32_t) product[i][j], shift_int);
			intx_lshift(shift_int, 32*(i+j), shift_int);
			intx_add(out, shift_int, out);
		}
	}

	uint32_t temp1[INT_SIZE*32];
	uint32_t temp2[INT_SIZE*32];

	if(t_comp) {
	       	return intx_two_complement(out, out);
	}
	return out;
}


intx_t intx_div(intx_t n, intx_t d, intx_t q, intx_t r) {
	bool t_comp_q = false;
	bool t_comp_r = false;
	
	uint32_t d_temp[INT_SIZE];
	uint32_t n_temp[INT_SIZE];
	
	if(intx_is_negative(n) && intx_is_negative(d)) {
		n = intx_two_complement(n, n_temp);
		d = intx_two_complement(d, d_temp);
		t_comp_r = true;
	}else if(intx_is_negative(n)) {
		t_comp_q = true;
		t_comp_r = true;
		n = intx_two_complement(n, n_temp);
		d = intx_transfer(d, d_temp);
	}else if(intx_is_negative(d)) {
		t_comp_q = true;
		d = intx_two_complement(d, d_temp);
		n = intx_transfer(n, n_temp);
	} else {
		n = intx_transfer(n, n_temp);
		d = intx_transfer(d, d_temp);
	}

	intx_lshift(r, INT_SIZE*32, r);
	intx_lshift(q, INT_SIZE*32, q);

	
	for(int i = INT_SIZE*32-1; i >= 0; i--) {
		intx_lshift(r, 1, r);

		intx_set_digit(r, 0, intx_get_digit(n_temp, i));

		//printf("\n");
		//intx_print_h(r);
		//intx_print_h(d_temp);

		if(intx_cmp(r, d_temp) >= 0) {
			//intx_print_h(r);
			intx_sub(r, d_temp, r);
			intx_set_digit(q, i, true);
		}

		/**intx_set_digit(r, 0, intx_get_digit(n, i));
		intx_sub(r, d, r);

		if(!intx_is_negative(r)) {
			intx_set_digit(q, i, true);
		} else {
			intx_add(r, d, r);
		}**/
	}

	if(t_comp_r) intx_two_complement(r, r);
	if(t_comp_q) intx_two_complement(q, q);

	return q;
}


bool intx_is_zero(intx_t n) {
	for(int i = 0; i < INT_SIZE; i++) {
		if(n[i] != 0) return false;
	}
	return true;
}


int intx_cmp(intx_t a, intx_t b) {
	int i = INT_SIZE-1;
	while(i >= 0 && (uint32_t) a[i] == (uint32_t) b[i]) i--;
	if(i < 0) return 0;
	if(i == INT_SIZE-1) return (int32_t) a[i] > (int32_t) b[i] ? 1 : -1;
	else return (uint32_t) a[i] > (uint32_t) b[i] ? 1 : -1;
}


bool intx_is_negative(intx_t n) {
	return (int32_t) n[INT_SIZE-1] < 0;
}


intx_t intx_pow_mod(intx_t a, intx_t p, intx_t n, intx_t out) {
	assert(!intx_is_negative(p));

	/**if(p[0] == 1) return out;

	uint32_t pow[INT_SIZE];
	intx_transfer(p, pow);

	uint32_t trash[INT_SIZE];

	uint32_t acc[INT_SIZE];

	intx_mul(a, a, acc);

	intx_div(acc, n, trash, acc);

	if(pow[0] & 1) {
		intx_rshift(p, 1, pow);
		intx_div(intx_mul(acc, intx_pow_mod(acc, pow, n, trash), trash), n, trash, out);
		
		return out;
	}else {
		intx_rshift(p, 1, pow);
		intx_pow_mod(acc, pow, n, out);
		return out;
	}**/
	
	uint32_t acc[INT_SIZE];
	intx_transfer(a, acc);

	uint32_t power[INT_SIZE];
	intx_transfer(p, power);

	uint32_t mod[INT_SIZE];
	intx_transfer(n, mod);

	uint32_t trash[INT_SIZE]; //Just to put as a throwaway argument

	intx_lshift(out, INT_SIZE*32, out); //Clears out what's actually on the array
	intx_set_digit(out, 0, true);

	intx_div(acc, mod, trash, acc);

	while(!intx_is_zero(power)) {
		if(power[0] & 1) intx_div(intx_mul(out, acc, out), mod, trash, out);

		//intx_print_h(out);

		intx_div(intx_mul(acc, acc, acc), mod, trash, acc);
		intx_rshift(power, 1, power);
	}
	return out;
}
			

intx_t intx_rand(intx_t out, bool positive, uint16_t chunk_size) {
	assert(chunk_size <= INT_SIZE);

	for(int i = 0; i < chunk_size; i++) out[i] = (rand() << 1) + rand();
	out[chunk_size-1] = positive ? out[chunk_size-1] & 0x7fffffff : out[chunk_size-1];
	for(int i = chunk_size; i < INT_SIZE; i++) out[i] = positive ? 0 : -1;
	out[INT_SIZE-1] = positive ? out[INT_SIZE-1] : 0x80000000 | out[INT_SIZE-1];

	return out;
}


/**int main(void) {

	intx_t a = intx_from_64(0);
	intx_t b = intx_from_64(1);

	while(!intx_is_zero(a)) {
		intx_print_h(a);
		intx_add(a, b, a)
	}
}**/
