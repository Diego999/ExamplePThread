#ifndef AUTOMATE_H
#define AUTOMATE_H

#include <vector>
#include <string>
const char INSERT_COIN = 'I';
const char GET_TEDDY = 'G';
const char QUIT = 'Q';

const int NB_TEDDY = 5;
const double PRICE_TEDDY = 2.5;
const int END_THREAD = -1;

void* MONNEY(void* param);
void* DISTRIBUTOR(void* param);
void* VENDOR(void* param);

void initializeMutexCond();
void destroyMutexCond();
void startAutomate();
char menuChoice();
bool isAvailablePiece(double n);
std::vector<double> getAvailablePieces();
std::vector<std::vector<std::string> > &getTeddies();
void giveChange(double);

template<typename T>
static void userInput(T& v);

void clearScreenDAP();
void pauseDAP();

#endif
