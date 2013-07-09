/*
 * Lamport's bakery algorithm
 * Diego Antognini & S�bastien Vaucher
 * 28 octobre 2012
 * bakery.h : Header du ficher d'impl�mentation principal
 */

#ifndef BAKERY_H
#define BAKERY_H

// Nombre de threads
int nbThreads;
// Compteur commun des diff�rents threads
int counter;
// Quand le compteur atteint countTo, on arr�te le programme
int countTo;
// Tableau de nbThreads cases o� chaque case repr�sente si le thread est en pleine attribution de ticket (true)
// ou s'il poss�de d�j� son ticket (false)
int* choices;
// Tableau de nbThreads cases o� chaque case repr�sente le num�ro de ticket du thread.
// Si la valeur vaut 0, le thread ne veut pas rentrer dans une section critique.
int* counters;

// Initialise les variables globales.
void initializeGlobals(int, int);
// Initialise le tableau des tickets et celui des choix.
void initializeCountersAndChoices();
// Lib�re la m�moire utilis�e par les tableaux counters et choices.
void freeMemory();

// Demande l'entr�e en section critique. Il ne peut y avoir qu'un seul thread en section critique.
// L'appel est bloquant : la m�thode retourne lorsque la section critique est libre pour l'appelant.
void lock(const int);
// Lib�re la section critique, ce qui permettra � un autre thread d'y entrer.
void unlock(const int);

// Code que doit ex�cuter chaque thread. Contient la section critique.
void* taskCode(void*);
// Cr�e les diff�rents threads qui ex�cuteront la m�thode taskCode()
void createAndUseThreads();

#endif