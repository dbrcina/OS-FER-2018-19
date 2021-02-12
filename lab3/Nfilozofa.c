#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

typedef struct {
	int number;
	int numberOfHands;
	int numberOfMeals;
} philosopher;

int NumberOfSticks;
int NumberOfPhilosophers;
int NumberOfMeals;
pthread_mutex_t Key;
pthread_cond_t Conditions;

void take_sticks(philosopher *philosopher) {
	pthread_mutex_lock(&Key);
	while (NumberOfSticks < philosopher->numberOfHands) {
		pthread_cond_wait(&Conditions, &Key);
	}
	NumberOfSticks -= philosopher->numberOfHands;
	printf("Filozof no.%d sa %d ruke uzima %d stapica i jede po %d. put!\n",philosopher->number + 1, philosopher->numberOfHands, 
										philosopher->numberOfHands, philosopher->numberOfMeals + 1);
	printf("Na stolu se trenutno nalaze %d stapica!\n\n",NumberOfSticks);
	pthread_mutex_unlock(&Key);
}

void return_sticks(philosopher *philosopher) {
	pthread_mutex_lock(&Key);
	NumberOfSticks += philosopher->numberOfHands;
	printf("Filozof no.%d sa %d ruke na stol vraca %d stapica i pocinje misliti!\n",philosopher->number + 1, philosopher->numberOfHands, philosopher->numberOfHands);
	printf("Na stolu se trenutno nalaze %d stapica!\n\n",NumberOfSticks);
	pthread_cond_broadcast(&Conditions);
	pthread_mutex_unlock(&Key);
}

void *pthread_philosopher(void *ptr) {
	philosopher *philosopher = ptr;
	while (philosopher->numberOfMeals < NumberOfMeals) {
		take_sticks(philosopher);
		sleep(2);
		return_sticks(philosopher);
		sleep(2);
		philosopher->numberOfMeals++;
	}
	free(philosopher);
}

int main(void) {
	int numberOfHands,i = 0;
	philosopher *filozof;
	pthread_t *thread_ids;
	FILE *fptr;
	fptr = fopen("unos.txt","r");
	if (fptr == NULL) {
		printf("Doslo je do greske kod otvaranja datoteke!\n");
		exit(1);
	}
	if (fscanf(fptr, "%d %d", &NumberOfSticks, &NumberOfPhilosophers) != 2) {
		printf("Doslo je do greske kod fscanf!\n");
		exit(1);
	}
	printf("Za stolom sjedi %d filozova i na stolu se trenutno nalazi %d stapica!\n\n",NumberOfPhilosophers, NumberOfSticks);
	printf("Unesi broj obroka: ");
	scanf("%d", &NumberOfMeals);
	printf("\n");
	pthread_mutex_init(&Key, NULL);
	pthread_cond_init(&Conditions, NULL);
	thread_ids = (pthread_t*)malloc(sizeof(pthread_t) * NumberOfPhilosophers);
	while (fscanf(fptr, "%d", &numberOfHands) != EOF) {
		filozof = (philosopher*)malloc(sizeof(philosopher));
		filozof->number = i;
		filozof->numberOfHands = numberOfHands;
		filozof->numberOfMeals = 0;
		if (pthread_create(thread_ids + i, NULL, pthread_philosopher, filozof) != 0) {
			printf("Ne mogu stvoriti novu dretvu!\n");
			exit(1);
		}
		i++;
	}
	for(i = 0; i < NumberOfPhilosophers; i++) {
		if (pthread_join(thread_ids[i], NULL) != 0) {
			printf("Doslo je do greske kod pthread_join!\n");
			exit(1);
		}
	}
	free(thread_ids);
	fclose(fptr);
	printf("Svi filozofi su jeli po %d puta!\n",NumberOfMeals);
	printf("Oslobadam memoriju i zatvaram resurse!\n");
	return 0;
}		
