/*
 * Lamport's bakery algorithm
 * Diego Antognini & S�bastien Vaucher
 * 28 octobre 2012
 * main.c : Point d'entr�e du programme
 *
 *	==IMPL�MENTATION==
 *
 *	1) M�moire partag�e :
 *		Le programme contient une zone de m�moire partag�e, utilis�e par tous les threads.
 *		nbThreads : Nombre de threads voulus
 *		counter : Compteur commun des threads
 *		countTo : Valeur maximale du compteur � laquelle le programme va se terminer
 *		choices[] : Tableau de bool�ens contenant le choix des threads (n�cessaire � l'algorithme du boulanger)
 *		counters[] : Tableau d'entiers contenant le num�ro du ticket de chaque thread (n�cessaire � l'algorithme du boulanger)
 *
 *	2) M�thodes :
 *		- Trois m�thodes pour la cr�ation et l'initialisation des tableaux et variables n�cessaires � l'algorithme
 *		- Une m�thode pour la lib�ration de la m�moire utilis�e par les tableaux dynamiques
 *		- Deux m�thodes pour la gestion de la section critique (lock() et unlock())
 *		- Une m�thode qui indique les actions � effectuer par les threads (taskCode)
 *		- Une m�thode qui permet la cr�ation des threads et leur utilisation
 *
 *	==V�RIFICATION DU FONCTIONNEMENT==
 *	En classe, nous avons vu que des des incoh�rences dans l'ordre d'ex�cution se produisent si l'on 
 *	demande � plusieurs threads d'incr�menter un compteur commun (Exemple : 7-8-12-9-13-10-11-...). 
 *	L'id�e est de refaire le m�me test, en variant la limite du compteur et le nombre de threads. Si 
 *	l'algorithme est correct, nous obtenons un comptage correct. Cependant, plus le nombre de threads 
 *	est grand, plus longtemps il faudra attendre, mais dans tous les cas il n'y a pas d'incoh�rence !
 *
 *	==SOURCE==
 *	http://fr.wikipedia.org/wiki/Algorithme_de_la_boulangerie
 */

#include "bakery.h"
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>

// Lit un nombre de l'entr�e standard
int getNumber(char *, int, int);

int main(void)
{
	int nbThreads = getNumber("Nombre de threads ?", 1, 100);
	int countTo = getNumber("Jusqu'� quelle valeur le programme doit-il compter ?", 1, INT_MAX);

	initializeGlobals(nbThreads, countTo);
	initializeCountersAndChoices();
	createAndUseThreads();
	freeMemory();

	printf("Appuyez sur une touche pour sortir ...");
	_getch();
	return EXIT_SUCCESS;
}

// Lit un nombre entre lowerLimit et upperLimit de l'entr�e standard
int getNumber(char *prompt, int lowerLimit, int upperLimit)
{
	int number = lowerLimit -1, res = 0;
	puts(prompt);	
	do
	{
		printf("Choix [%d, %d] : ", lowerLimit, upperLimit);
		_flushall();
		res = scanf_s("%9d", &number);
	} while (res != 1 || number < lowerLimit || number > upperLimit);

	return number;
}
