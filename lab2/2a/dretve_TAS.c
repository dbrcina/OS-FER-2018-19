#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdatomic.h>

#define TAS(ZASTAVICA) __atomic_test_and_set (&ZASTAVICA, __ATOMIC_SEQ_CST)

int A;
int N;
int M;
char ZASTAVICA = 0;

void udji_u_ko(int *thread) {
	while (TAS(ZASTAVICA) == 1) continue;
	ZASTAVICA = 1;
	A++;
	printf("Dretva no.%3d se nalazi u KO i postavlja vrijednost varijable A na %d\n",(*thread)+1,A);
}

void izadji_iz_ko() {
	ZASTAVICA = 0;
}


void *dretva(void *rbr) {
	int i;
	for (i = 0; i < M; i++) { 
		udji_u_ko(rbr);
		izadji_iz_ko();
	}
}

int main(int argc, char *argv[]) {
	if (argc != 3) {
		printf("Nije unesen tocan broj parametara za funkciju main!\n");
		exit(1);
	}
	A = 0;
	N = atoi(argv[1]);
	M = atoi(argv[2]);
	int broj[N];
	pthread_t tId[N];
	int i;
	for (i = 0; i < N; i++) {
		broj[i] = i;
		if (pthread_create(tId + i, NULL, dretva, &broj[i]) != 0) {
			printf("Ne mogu stvoriti novu dretvu!\n");
			exit(1);
		}
	}
	for (i = 0; i < N; i++) {
		if (pthread_join(tId[i], NULL) != 0) {
			printf("Dogodila se greska pri izvrsenju funkcije pthread_join!\n");
			exit(1);
		}
	}

	printf("A = %d\n",A);
	return 0;
}
