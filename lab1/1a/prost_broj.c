#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <math.h>
#include <sys/time.h>

int pauza = 0;
unsigned long broj = 2;
unsigned long zadnji = 2;

int prost(unsigned long n) {
	unsigned long i;
	double max = sqrt(n);
	if (n % 2 == 0) return 0;
	for (i = 3; i <= max; i+=2)
		 if (n % i == 0) return 0;
	return 1;
}

void periodicki_ispis(int sig) {
	printf("zadnji prosti broj = %ld\n",zadnji);
}

void postavi_pauzu(int sig) {
	pauza = 1 - pauza;
}

void prekini(int sig) {
	printf("zadnji prosti broj = %ld",zadnji);
	exit(0);
}

int main() {
	struct itimerval t;
	sigset (SIGALRM, periodicki_ispis);
	sigset (SIGTERM, prekini);
	sigset (SIGINT , postavi_pauzu);
	// definiranje periodickog slanja signala
	// prvi puta nakon:
	t.it_value.tv_sec = 5;
	t.it_value.tv_usec = 0;
	// nakon prvog puta, periodicki sa periodom:
	t.it_interval.tv_sec = 5;
	t.it_interval.tv_usec = 0;
	setitimer (ITIMER_REAL, &t, NULL);

	while (1) {
		if (prost(broj)) zadnji = broj;
		broj++;
		while (pauza == 1) pause();
	}
	return 0;
}
