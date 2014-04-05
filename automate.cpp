/*
 * Automate
 * Diego Antognini & S�bastien Vaucher
 * 17 D�cembre 2012
 * automate.cpp : Fichier d'impl�mentation principal
 */

#include "automate.h"
#include "structures.h"

#include <cmath>
#include <ctype.h>
#include <iomanip>
#include <iostream>
#include <pthread.h>
#include <vector>

using namespace std;

// Exclusion mutuelle globale
pthread_mutex_t mutex;

pthread_cond_t inserer;
pthread_cond_t distribuer;
pthread_cond_t vendeur;

void* MONNAIE(void* param)
{
	//Permettre l'arr�t du thread depuis un autre
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

	while(true)
	{
		pthread_mutex_lock(&mutex);
		//On attend que le thread VENDEUR donne le signal d'insertion d'une pi�ce
		pthread_cond_wait(&inserer, &mutex);

		double n = 0;
		system("CLS");
		cout << "Les pi�ces accept�es sont les suivantes : ";

		static vector<double> availablePieces = getAvailablePieces();

		for(vector<double>::iterator it = availablePieces.begin() ; it!=availablePieces.end() ; it++)
			cout << *it << ",";

		cout << endl;
		do
		{
			cout << "Ins�rez une pi�ce  : ";

			userInput(n);

			//On v�rifie que la pi�ce existe
			if(!isAvailablePiece(n))
			{
				cerr << "Pi�ce refus�e" << endl;
				n = 0;
			}
			else
				cout << "La pi�ce de " << n << " a �t� accept�e !" << endl;
		} while (n <= 0);

		//On met � jour la variable contenant la pi�ce
		((Piece*)param)->setValue(n,ID_MONNAIE);//WRITE ONLY
		system("PAUSE");

		//On annonce au thread VENDEUR que l'action est termin�e
		pthread_cond_signal(&vendeur);
		pthread_mutex_unlock(&mutex);
	}
	return NULL;
}

void* DISTRIBUTEUR(void* param)
{
	//Permettre l'arr�t du thread depuis un autre
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

	while(true)
	{
		pthread_mutex_lock(&mutex);
		//On attend que le thread VENDEUR donne le signal de la distribution de la bouteille
		pthread_cond_wait(&distribuer, &mutex);

		//On r�cup�re le param�tre
		Commande* ach = (Commande*)param;

		system("CLS");
		cout << "Monnaie restante : " << ach->getSolde() << endl;
		//Si une bouteille a �t� achet�e (c-�-d que le solde est suffisant et qu'il reste au moins 1 bouteille
		if(ach->getBouteille())
			cout << "Vous recevez 1 bouteille." << endl;
		else
			cout << "Vous ne recevez aucune bouteille." << endl;
		system("PAUSE");

		//On annonce au thread VENDEUR que l'action est termin�e
		pthread_cond_signal(&vendeur);
		pthread_mutex_unlock(&mutex);
	}
	return NULL;
}

void* VENDEUR(void* param)
{
	int nbBottles = NB_BOTTLES;
	//On r�cup�re le param�tre
	Gestion* gest = (Gestion*)param;

	while(true)
	{
		system("CLS");
		cout << "Solde : " << setprecision(2) << fixed << gest->getDistrib()->getSolde() << endl;
		cout << "Bouteilles restantes : " << nbBottles << endl;
		cout << "Prix bouteille : " << PRICE_BOTTLE << endl << endl;

		pthread_mutex_lock(&mutex);

		double newSolde = 0;

		switch(menuChoix())
		{
            case GET_BOTTLE:
                gest->getDistrib()->setRecuBouteille(false,ID_VENDEUR);

                //Si le solde est suffisant et qu'il reste assez de bouteilles, on effectue la commande tout en d�duisant le solde
                //On met � jour la ressource partag�e
                if(gest->getDistrib()->getSolde() >= PRICE_BOTTLE && nbBottles > 0)
				{
					newSolde = gest->getDistrib()->getSolde() - PRICE_BOTTLE;

                    gest->getDistrib()->setSolde(newSolde,ID_VENDEUR);
                    nbBottles--;
                    gest->getDistrib()->setRecuBouteille(true,ID_VENDEUR);
                }

                //On annonce au thread DISTRIBUTEUR qu'il peut ex�cuter la commande
                pthread_cond_signal(&distribuer);
                //On attend que le thread DISTRIBUTEUR ait fini sa t�che
                pthread_cond_wait(&vendeur,&mutex);
                break;

            case INSERT_COIN:
                //On annonce au thread MONNAIE qu'il peut lancer la proc�dure d'insertion d'argent
                pthread_cond_signal(&inserer);
                //On attend que le thread DISTRIBUTEUR ait fini sa t�che
                pthread_cond_wait(&vendeur,&mutex);

				newSolde = gest->getDistrib()->getSolde() + gest->getPiece()->getValue();

                //On met � jour le solde en r�cup�rant la pi�ce fra�chement ins�r�e
                gest->getDistrib()->setSolde(newSolde, ID_VENDEUR);
                break;

            case QUIT:
                system("CLS");
				cout << endl << "Monnaie rendue : " << gest->getDistrib()->getSolde() << endl << endl;
				system("PAUSE");
				//On quitte le thread avec le code de retour
				pthread_exit((void*)FIN_THREAD);
			break;
		}

		pthread_mutex_unlock(&mutex);
	}
	return NULL;
}

void initializeMutexCond()
{
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init (&inserer, NULL);
	pthread_cond_init (&distribuer, NULL);
	pthread_cond_init (&vendeur, NULL);
}

void destroyMutexCond()
{
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&inserer);
	pthread_cond_destroy(&distribuer);
	pthread_cond_destroy(&vendeur);
}

void startAutomate()
{
	initializeMutexCond();

	pthread_t threads[3];

	// Cr�ation des ressources partag�es

	//MONNAIE
	Piece piece = Piece(0.0);

	//DISTRIBUTEUR
	Commande dis = Commande(0.0,false);

	//VENDEUR
	//R�f�rence � la ressource de MONNAIE
	//R�f�rence � la ressource de DISTRIBUTEUR
	Gestion ven = Gestion(&piece,&dis);

	// Cr�ation des threads
	pthread_create(&threads[0], NULL, VENDEUR, &ven);
	pthread_create(&threads[1], NULL, DISTRIBUTEUR, &dis);
	pthread_create(&threads[2], NULL, MONNAIE, &piece);

	//On attend seulement que le thread VENDEUR finisse. C'est vendeur qui g�re les 2 autres threads
	//On r�cup�re le r�sultat de retour du thread VENDEUR
	void* status;
	pthread_join(threads[0], &status);

	//Si VENDEUR est termin� (dans le cas o� l'utilisateur a choisi l'action "Quitter", on supprime les 2 autres threads
	if((int)status == FIN_THREAD)
	{
		pthread_cancel(threads[1]);
		pthread_cancel(threads[2]);
	}

	destroyMutexCond();
}

char menuChoix()
{
	char c = 0;

	cout << "Pour ins�rer une pi�ce de monnaie, pressez la touche '" << INSERT_COIN << "'." <<endl;
	cout << "Pour obtenir une bouteille, pressez la touche '" << GET_BOTTLE << "'." <<endl;
	cout << "Pour quitter, pressez la touche '" << QUIT << "'." <<endl << endl;

	do
	{
		cout << "Choix : ";

		userInput(c);

		if(!isupper(c))//Si la touche est en minuscule, on la passe en majuscule
			c = toupper(c);
	} while(c != INSERT_COIN && c != GET_BOTTLE && c != QUIT);

	return c;
}

bool doubleEquals(double a,double b)
{
	double e = 1e-3;// Pas besoin d'une meilleure pr�cision, la pi�ce minimale est de 0.05
	return fabs(a - b) < e;
}

vector<double> getAvailablePieces()
{
	static vector<double> availablePieces;

	if(availablePieces.size() == 0)
	{
		//Ajout des diff�rentes pi�ces existantes
		availablePieces.push_back(0.05);
		availablePieces.push_back(0.10);
		availablePieces.push_back(0.20);
		availablePieces.push_back(0.50);
		availablePieces.push_back(1.00);
		availablePieces.push_back(2.00);
		availablePieces.push_back(5.00);
	}
	return availablePieces;
}

bool isAvailablePiece(double piece)
{
	static vector<double> availablePieces = getAvailablePieces();

	// Regarde si la pi�ce pass�e en param�tre est accept�e
	for(vector<double>::iterator it = availablePieces.begin() ; it != availablePieces.end() ; it++)
	{
		if(doubleEquals(piece, *it))
			return true;
	}
	return false;
}

template<typename T>
void userInput(T& v)
{
	//Dans le cas o� le nombre entr� n'est pas un chiffre,on nettoye les bits d'erreurs
	cin.ignore(cin.rdbuf()->in_avail());
	cin >> v;
	if(!cin || cin.rdbuf()->in_avail() != 1)
	{
		cin.clear();
		v=0;
	}
}