/*
 * Lamport's bakery algorithm
 * Diego Antognini & S�bastien Vaucher
 * 28 octobre 2012
 * bakery.c : Fichier d'impl�mentation principal
 */

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include "bakery.h"

#define false 0
#define true 1

int* choices = 0;
int* counters = 0;
int nbThreads = 0;
int countTo = 0;
int counter = 0;

void initializeGlobals(int _nbThreads, int _countTo)
{
	nbThreads = _nbThreads;
	countTo = _countTo;
}

void initializeCountersAndChoices()
{
	int i = 0;
	counters = (int*)malloc(sizeof(int) * nbThreads);
	choices = (int*)malloc(sizeof(int) * nbThreads);

	// On contr�le que l'allocation de m�moire a r�ussi
	if(counters == NULL || choices == NULL)
		exit(EXIT_FAILURE);

	for(i = 0; i < nbThreads; i++)
	{
		counters[i] = 0;
		choices[i] = false;
	}
}

void freeMemory()
{
	free(counters);
	counters = NULL;

	free(choices);
	choices = NULL;
}

void lock(const int id)
{
	int max = counters[0];
	int i = 0;

	// Le thread n�id entre en mode attribution de ticket
	choices[id] = true;

	// On recherche le plus grand num�ro de ticket parmi les threads
	for(i = 0; i < nbThreads; i++)
		if(counters[i] > max)
			max = counters[i];

	// On attribut un ticket plus grand que le dernier, le thread n�id devient le dernier
	counters[id] = max + 1;

	// Fin du mode attribution de ticket
	choices[id] = false;

	// Si plusieurs threads ex�cutent ces calculs en m�me temps, il est possible que certains d'entre-eux
	// poss�dent le m�me n� de ticket. C'est pourquoi il faut v�rifier que tous les autres tickets ont
	// des num�ros inf�rieurs � celui actuel.

	for(i = 0; i < nbThreads; i++)
	{
		// Si le thread est en mode attribution du ticket, on attend d'avoir le n� du ticket
		while(choices[i]);

		/*Condition 1 : Avoir un num�ro de ticket pour rentrer dans la section critique
			ET
				Condition 2 : On regarde si le thread de la boucle a une priorit� sup�rieur au thread actuel
				OU
				Condition 3 : Ils ont le m�me num�ro de ticket
					ET
					Condition 4 : Dans ce cas la priorit� va au thread ayant l'indice le plus petit
		*/
		while(counters[i]/*(1)*/ && ((counters[i] < counters[id])/*(2)*/ || ((counters[i] == counters[id])/*(3)*/ && (i < id)/*(4)*/)));
	}
}

void unlock(const int id)
{
	// On met � 0 le num�ro de ticket, donc le thread ne veut pas rentrer en section critique
	counters[id] = 0;
}

void* taskCode(void* argument)
{
	// On r�cup�re l'identifiant du thread
	int tid = *((int *) argument);

	while(counter < countTo)
	{
		// On demande l'entr�e en section critique
		lock(tid);
		// D�but section critique
		if(counter < countTo)
		{
			// On incr�mente le compteur et on l'affiche
			counter++;
			printf("Thread %d\tTicket %d\tCounter : %d\n", tid, counters[tid], counter);
		}
		// Fin section critique

		//On lib�re la section critique
		unlock(tid);
	}

	return NULL;
}

void createAndUseThreads()
{
	int i = 0;
	// On cr�� le tableau des threads et le tableau des arguments
	pthread_t* threads = (pthread_t*)malloc(sizeof(pthread_t) * nbThreads);
	int* threads_args = (int*)malloc(sizeof(int) * nbThreads);

	// Cr�ation des nbThreads threads
	for (i = 0; i < nbThreads; i++) {
		threads_args[i] = i;
		pthread_create(threads + i, NULL, taskCode, threads_args + i);
	}

	// On attend que tous les threads aient fini leurs t�ches
	for (i = 0; i < nbThreads; i++)
		pthread_join(threads[i], NULL);

	// Lib�ration de la m�moire occup�e par les tableaux dynamiques
	free(threads);
	threads = NULL;
	free(threads_args);
	threads_args = NULL;
}