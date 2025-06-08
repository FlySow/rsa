#include "arithmetic.h"
#include "intx_t.h"
#include "algorithms.h"
#include <stdlib.h>
#include <stdio.h>


void generate_keypair(public_key* puk, private_key* prk, uint64_t mask) {
	uint32_t one[INT_SIZE];

	intx_lshift(one, 32*INT_SIZE, one);
	one[0] = 1;

	uint32_t p[INT_SIZE];
	uint32_t q[INT_SIZE];

	generate_prime(fermat_test, 25, p, mask);
	generate_prime(fermat_test, 25, q, mask);

	intx_mul(p, q, puk->n);
	intx_transfer(puk->n, prk->n);

	intx_sub(p, one, p);
	intx_sub(q, one, q);

	uint32_t phi[INT_SIZE];

	intx_mul(p, q, phi);

	select_random_coprime(phi, puk->e);
	inv_mod(puk->e, phi, prk->d);

}


intx_t encrypt(intx_t in, public_key puk, intx_t out) {
	return intx_pow_mod(in, puk.e, puk.n, out);
}


intx_t decrypt(intx_t in, private_key prk, intx_t out) {
	return intx_pow_mod(in, prk.d, prk.n, out);
}

/**intx_t hastad_broadcast_attack(public_key puks[], int length, intx_t encrypted) {
	for(int i = 0; i < length; i++) 
		if(puks[i].e > length) 
}**/
