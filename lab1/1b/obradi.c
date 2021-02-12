#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <signal.h>

#define N 6

int OZNAKA_CEKANJA[N];
int PRIORITET[N];
int TEKUCI_PRIORITET = 0;
char ZNAKOVI[] = {'-','-','-','-','-','-'};

int SIGNALI[] = {SIGUSR1, SIGUSR2, SIGTERM, SIGQUIT, SIGINT};

void zabrani_prekidanje() {
	int i;
	for (i = 0; i < N - 1; i++) {
		sighold(SIGNALI[i]);
	}
}

void dozvoli_prekidanje() {
	int i;
	for (i = 0; i < N - 1; i++) {
		sigrelse(SIGNALI[i]);
	}
}

void obrada_signala(int x) { 
	int i;
	int time = 5;
	for (i = 0; i < N; i++) {
		ZNAKOVI[i]='-';
	}
	ZNAKOVI[x] = 'P';
	ispisi();
	for (i = 1; i <= time; i++) {
		sleep(1);
		ZNAKOVI[x] = i + '0';
		ispisi();
	}
	ZNAKOVI[x] = 'K';
	ispisi();
	ZNAKOVI[x] = '-';	
}

void ispisi() {
	int i;
	for (i = 0; i < N; i++) {
		printf(" %c  ",ZNAKOVI[i]);
	}
	printf("\n");
}

void prekidna_rutina(int sig) {
	int n = -1;
	int x;
	int j;
	zabrani_prekidanje();
	switch (sig) {
		case SIGUSR1:
			n = 1;
			printf(" -   X   -   -   -   -\n");
			break;
		case SIGUSR2:
			n = 2;
			printf(" -   -   X   -   -   -\n");
			break;
		case SIGTERM:
			n = 3;
			printf(" -   -   -   X   -   -\n");
			break;
		case SIGQUIT:
			n = 4;
			printf(" -   -   -   -   X   -\n");
			break;
		case SIGINT:
			n = 5;
			printf(" -   -   -   -   -   X\n");
			break;
	}
	OZNAKA_CEKANJA[n] = 1;
	if (TEKUCI_PRIORITET < n) {
		do {
			x = 0;
			for (j = TEKUCI_PRIORITET + 1; j < N; j++) {
				if (OZNAKA_CEKANJA[j] > 0) {
					x = j;
				}
			}
		
			if (x > 0) {
				OZNAKA_CEKANJA[x] = 0;
				PRIORITET[x] = TEKUCI_PRIORITET;
				TEKUCI_PRIORITET = x;
				dozvoli_prekidanje();
				obrada_signala(x);
				zabrani_prekidanje();
				TEKUCI_PRIORITET = PRIORITET[x];
			}
		} while (x > 0);
		dozvoli_prekidanje();
	}
}

int main() {

	int i;
	sigset (SIGUSR1, prekidna_rutina);
	sigset (SIGUSR2, prekidna_rutina);
	sigset (SIGTERM, prekidna_rutina);	
	sigset (SIGQUIT, prekidna_rutina);	
	sigset (SIGINT, prekidna_rutina);

	printf("Proces obrade prekida, PID = %ld\n", getpid());
	printf("GP  s1  s2  s3  s4  s5\n");
	printf("----------------------\n");
	for (i = 1; i <= 9; i++) {
		sleep(1);
		ZNAKOVI[0] = i + '0';
		ispisi();
		
	}
	printf("Zavrsio osnovni program\n");

	return 0;
}
