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
int N;
int M;

void procesi(int rbr) {
	int i;
	for (i = 0; i < M; i++) {
		(*A)++;
		printf("Obrada procesa no.%d\n",rbr + 1);
		//sleep(1);
	}
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

int main(int argc, char *argv[]) {
	if (argc != 3) {
		printf("Nije unesen tocan broj argumenata preko komandne ljuske!\n");
		exit(1);
	}

	sigset(SIGINT, obrisi);
	sigset(SIGQUIT, obrisi);
	N = atoi(argv[1]);
	M = atoi(argv[2]);
	int i;
	int broj[N];

	ShmId = shmget(IPC_PRIVATE, sizeof(int), 0600);
	if (ShmId == -1) {
		printf("Doslo je do greske prilikom alociranja novog zajednickog segmenta!\n");
		exit(1);
	}

	A = (int *)shmat(ShmId, NULL, 0);
	if ((*A) == -1) {
		printf("Doslo je do greske prilikom vezanja segmenta na odredenu adresu!\n");
		exit(1);
	}
	*A = 0;

	for (i = 0; i < N; i++) {
		broj[i] = i;
		if (fork() == 0) {
			procesi(i);
			exit(0);
		}
	}

	while (i--) wait(NULL);
	
	printf("A = %d\n",*A);
	obrisi(0);

	return 0;
}
	
