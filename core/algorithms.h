#ifndef __ALGORITHMS_H__
#define __ALGORITHMS_H__

#include "intx_t.h"

typedef struct public_key_s {
	uint32_t e[INT_SIZE];
	uint32_t n[INT_SIZE];
} public_key;

typedef struct private_key_s {
	uint32_t d[INT_SIZE];
	uint32_t n[INT_SIZE];
} private_key;

void generate_keypair(public_key* puk, private_key* prk);

intx_t encrypt(intx_t in, public_key puk, intx_t out);

intx_t decrypt(intx_t in, private_key prk, intx_t out);

#endif
