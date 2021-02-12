#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>

int PID = 0;
int SIGNALI[] = {SIGUSR1, SIGUSR2, SIGTERM, SIGQUIT, SIGINT};

void prekidna_rutina(int sig) {
	kill(PID, SIGINT);
	exit(0);
}

int main(int argc, char *argv[]) {
	int i;
	PID = atoi(argv[1]);
	sigset(SIGINT, prekidna_rutina);
	srand((unsigned)time(NULL));
	while(1) {
		i = rand()%4;
		sleep(rand()%3 + 3);
		kill(PID, SIGNALI[i]);
		printf("Poslan je signal s%d\n",i+1);
		
	}
	return 0;
}
