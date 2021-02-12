#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <unistd.h>

int ShmId;
int *A;
int *BROJ; //dodijeljeni broj (0 kada dretva ne trazi ulaz u KO)
int *TRAZIM; //stiti se dodijela broja
int N;
int M;

int getMax(int *p) {
	int i,max;
	max = p[0];
	for (i = 1; i < N; i++) {
		if (p[i] > max)
			max = p[i];
	}
	return max;
}

void udji_u_ko(int i) {
	int j;
	TRAZIM[i] = 1;
	BROJ[i] = getMax(BROJ) + 1;
	TRAZIM[i] = 0;

	for (j = 0; j < N; j++) {
		//ceka se da proces j dobije broj ako je u postupku dobivanja
		while (TRAZIM[j] != 0) continue;

		//cekaj ako j ima prednost
		while (BROJ[j] != 0 && (BROJ[j] < BROJ[i] || (BROJ[j] == BROJ[i] && j < i))) continue;
	}
	(*A)++;
	printf("Trenutno je proces no.%d u KO i postavlja vrijednost A na %d\n",i+1,*A);
	//sleep(1);
}

void izadji_iz_ko(int i) {
	BROJ[i] = 0;
}

void obrisi(int sig) {
	if (shmdt((char *)A) == -1) {
		printf(" Doslo je do greske kod otpustanja segmenta!\n");
		exit(1);
	}
	if (shmctl(ShmId, IPC_RMID, NULL) == -1) {
		printf(" Doslo je do greske kod unistavanja segmenta!\n");
		exit(1);
	}
	exit(0);
}

void procesi(int i) {
	do {
		udji_u_ko(i);
		izadji_iz_ko(i);
		M--;
	} while (M > 0);
}

int main(int argc, char *argv[]) {
	if (argc != 3) {
		printf("Nije unesen tocan broj argumenata preko komandne ljuske!\n");
		exit(1);
	}

	sigset(SIGINT, obrisi);
	N = atoi(argv[1]);
	M = atoi(argv[2]);
	int i;

	ShmId = shmget(IPC_PRIVATE, (2 * N + 1) * sizeof(int), 0600);
	if (ShmId == -1) {
		printf("Doslo je do greske prilikom alociranja novog zajednickog segmenta!\n");
		exit(1);
	}

	A = (int *)shmat(ShmId, NULL, 0);
	if ((*A) == -1) {
		printf("Doslo je do greske prilikom vezanja segmenta na odredenu adresu!\n");
		exit(1);
	}
	BROJ = (int *)(A + N);
	TRAZIM = (int *)(BROJ + N);		
	*A = 0;

	for (i = 0; i < N; i++) BROJ[i] = 0;

	for (i = 0; i < N; i++) {
		switch(fork()) {
			case 0:
				procesi(i);
				exit(0);
			case -1:
				printf("Dogodila se greska kod kreiranja djete procesa!\n");
				exit(1);
			default:
				break;
		}
	}

	while (i--) wait(NULL);
	
	printf("A = %d\n",*A);
	obrisi(0);

	return 0;
}
	
