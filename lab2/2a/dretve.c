#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

int A;
int N;
int M;

void *dretva(void *rbr) {
	int i;
	int *d = rbr;
	for (i = 0; i < M; i++) {
		A++;
		printf("Obrada dretve no.%d %d/%d\n",(*d)+1,i+1,M);
		//sleep(1);
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
