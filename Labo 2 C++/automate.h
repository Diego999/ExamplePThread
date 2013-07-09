/*
 * Automate
 * Diego Antognini & S�bastien Vaucher
 * 17 D�cembre 2012
 * automate.h : Header du ficher d'impl�mentation principal
 */

#ifndef AUTOMATE_H
#define AUTOMATE_H

#include <vector>

//Touches utilis�es pour activer les diff�rentes actions
const char INSERT_COIN = 'I';
const char GET_BOTTLE = 'O';
const char QUIT = 'Q';

//Constantes (Nombre de bouteilles dans l'automate et prix d'une bouteille)
const int NB_BOTTLES = 5;
const double PRICE_BOTTLE = 2.5;

//Code de retour lorsque l'on quitte le thread VENDEUR
const int FIN_THREAD = -1;

//Les t�ches en concurrence

// Insertion de la monnaie
void* MONNAIE(void* param);
// Ex�cution de la commande
void* DISTRIBUTEUR(void* param);
// Gestion de l'automate
void* VENDEUR(void* param);

// Initialise les conditions et le mutex
void initializeMutexCond();
// D�truit les conditions et le mutex
void destroyMutexCond();
// Cr�e les diff�rents threads qui ex�cuteront MONNAIE, VENDEUR et DISTRIBUTEUR
void startAutomate();
// Menu affichant les diff�rentes actions possibles pour l'utilisateur
char menuChoix();
// V�rifie que la pi�ce ins�r�e est acceptable
bool isAvailablePiece(double n);
//Retourne le tableau des pi�ces valides
std::vector<double> getAvailablePieces();

// Permet de comparer 2 double
static bool doubleEquals(double a, double b);

// Entr�e d'un nombre par l'utilisateur
template<typename T>
static void userInput(T& v);

#endif