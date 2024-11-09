#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "intx_t.h"
#include "arithmetic.h"
#include "algorithms.h"
#include <time.h>
#include <string.h>
#include <stdbool.h>


int main(int argc, char** argv) {
	srand(time(NULL));

	bool running = true;

	intx_t msg = intx_from_64(47);
	intx_t encrypted = intx_from_64(0);
	intx_t decrypted = intx_from_64(0);

	public_key puk;
	private_key prk;

	intx_print_h(msg);

				generate_keypair(&puk, &prk);

				printf("public key\n");
				printf("e: ");
				intx_print_h(puk.e);
				printf("n: ");
				intx_print_h(puk.n);
				printf("private key: ");
				intx_print_h(prk.d);

				encrypt(msg, puk, encrypted);
				printf("\nEncrypted message\n");
				intx_print_h(encrypted);

				decrypt(encrypted, prk, decrypted);
				printf("\nDecrypted message\n");
				intx_print_h(decrypted);

	free(msg);
	free(encrypted);
	free(decrypted);

	return 0;
}

