#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h> 

#define CekajOSEM(i) SemOp(i, -1)
#define PostaviOSEM(i) SemOp(i, 1)

int SemId;
int BrojMjesta;
int BrojOsoba;

void SemGet(int n) {
	SemId = semget(IPC_PRIVATE, n, 0600);
	if (SemId == -1) {
		printf("Ne mogu dobiti semafore!\n");
		exit(1);
	}
}

int SemOp(int semNum, int semOp) {
	struct sembuf semBuf;
	semBuf.sem_num = semNum;
	semBuf.sem_op = semOp;
	semBuf.sem_flg = 0;
	return semop(SemId, &semBuf, 1);
}

void SemRemove(void) {
	printf("\nZavrsavam program i brisem sve semafore iz memorije!\n");
	(void) semctl(SemId, 0, IPC_RMID, 0);
}

void ObradiSignal_dijete(int sig) {
	exit(0);
}

void ObradiSignal_roditelj(int sig) {
	while (BrojOsoba + 1) {
		wait(NULL);
		BrojOsoba--;
	}
	SemRemove();
	exit(0);
}

void Sigset_roditelj(void) {
	sigset(SIGINT, ObradiSignal_roditelj);
	sigset(SIGQUIT, ObradiSignal_roditelj);
}

void Sigset_dijete(void) {
	sigset(SIGINT, ObradiSignal_dijete);
	sigset(SIGQUIT, ObradiSignal_dijete);
}

void Error(int semNum, int semOp) {
	if (semOp == -1) {
		printf("Doslo je do greske kod funkcije CekajOSEM(%d)!\n",semNum);
	} else {
		printf("Doslo je do greske kod funckije PostaviOSEM(%d)!\n",semNum);
	}
	exit(1);
}
		
void Vrtuljak(void) {
	int i;
	printf("Vrtuljak je stvoren!\n\n");
	while (1) {
		for (i = 0; i < BrojMjesta; i++) {
			if (PostaviOSEM(0) == -1) {	//PostaviOSEM(ulaz)
				 Error(0, 1);
			}
		}
		for (i = 0; i < BrojMjesta; i++) {
			if (CekajOSEM(1) == -1) {	//CekajOSEM(usao)
				Error(1, -1);
			}
		}
		printf("\nPokreni vrtuljak!\n");
		sleep(2);
		printf("Zaustavi vrtuljak!\n\n");
		for (i = 0; i < BrojMjesta; i++) {
			if (PostaviOSEM(2) == -1) {	//PostaviOSEM(kraj)
				Error(2, 1);
			}
		}
		for (i = 0; i < BrojMjesta; i++) {
			if (CekajOSEM(3) == -1) {	//CekajOsem(izlaz)
				Error(3, -1);
			}
		}
	}	
}

void Posjetitelj(int passenger) {
	int i;
	if (CekajOSEM(0) == -1) {	//CekajOSEM(ulaz)
		Error(0, -1);
	}
	printf("Posjetitelj no.%2d je sjeo na vrtuljak!\n",passenger);
	if (PostaviOSEM(1) == -1) {	//PostaviOSEM(usao)
		Error(1, 1);
	}
	if (CekajOSEM(2) == -1) {	//CekajOSEM(kraj)
		Error(2, -1);
	}
	printf("Posjetitelj no.%2d je sisao sa vrtuljka!\n",passenger);
	if (PostaviOSEM(3) == -1){	//PostaviOSEM(izlaz)
		Error(3, -1);
	}
}
	
int main(void) {
	int i;
	printf("Unesite zeljeni broj mjesta na vrtuljku: ");
	scanf("%d",&BrojMjesta);
	do {
		printf("Unesite zeljeni broj osoba (minimalno %d): ",BrojMjesta);
		scanf("%d",&BrojOsoba);
		if (BrojOsoba < BrojMjesta) {
			printf("Vrtuljak ne moze krenuti dok sva mjesta nisu popunjena!\n");
		}
	} while (BrojOsoba < BrojMjesta);
	SemGet(4);
	Sigset_roditelj();
	if (fork() == 0) {
		Sigset_dijete();
		Vrtuljak();
	}
	for (i = 0; i < BrojOsoba; i++) {
		if (fork() == 0) {
			Sigset_dijete();
			while(1) {
				Posjetitelj(i+1);
			}
		}
	}
	while (BrojOsoba + 1) {
		wait(NULL);
		BrojOsoba--;
	}
	SemRemove();
	return 0; 
}
