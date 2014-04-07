#ifndef AUTOMATE_H
#define AUTOMATE_H

#include <vector>

const char INSERT_COIN = 'I';
const char GET_BOTTLE = 'G';
const char QUIT = 'Q';

const int NB_BOTTLES = 5;
const double PRICE_BOTTLE = 2.5;
const int FIN_THREAD = -1;

void* MONNAIE(void* param);
void* DISTRIBUTEUR(void* param);
void* VENDEUR(void* param);

void initializeMutexCond();
void destroyMutexCond();
void startAutomate();
char menuChoix();
bool isAvailablePiece(double n);
std::vector<double> getAvailablePieces();

static bool doubleEquals(double a, double b);

template<typename T>
static void userInput(T& v);

void clearScreenDAP();
void pauseDAP();

#endif
