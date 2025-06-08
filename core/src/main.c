#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "intx_t.h"
#include "arithmetic.h"
#include "algorithms.h"
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>


int main(int argc, char** argv) {
	srand(time(NULL));

	bool running = true;

	intx_t msg = intx_from_64(47);
	intx_t encrypted = intx_from_64(0);
	intx_t decrypted = intx_from_64(0);

	public_key puk;
	private_key prk;

	clock_t t;

	t = clock();


	printf("Entier de taille: %d bits\n", 32*INT_SIZE/2); 
	
	printf("Message initial\n");

	intx_print_h(msg);

				generate_keypair(&puk, &prk, 0xFFFFFFFF);

				printf("Clé publique\n");
				printf("e: ");
				intx_print_h(puk.e);
				printf("n: ");
				intx_print_h(puk.n);
				printf("Clé privée\n");
				printf("d: ");
				intx_print_h(prk.d);

				encrypt(msg, puk, encrypted);
				printf("\nMessage chiffré\n");
				intx_print_h(encrypted);

				decrypt(encrypted, prk, decrypted);
				printf("\nMessage déchiffré\n");
				intx_print_h(decrypted);

	t = clock() - t;

	printf("\nTemps pris: %f ms\n", ((double) t)/CLOCKS_PER_SEC*1000);
	
	free(msg);
	free(encrypted);
	free(decrypted);

	return 0;
}

