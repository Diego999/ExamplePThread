#include "automate.h"
#include "structures.h"

#include <cmath>
#include <ctype.h>
#include <iomanip>
#include <iostream>
#include <pthread.h>
#include <vector>

using namespace std;

pthread_mutex_t _mutex;

pthread_cond_t inserer;
pthread_cond_t distribuer;
pthread_cond_t vendeur;

void* MONNEY(void* param)
{
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

	while(true)
	{
        pthread_mutex_lock(&_mutex);
        pthread_cond_wait(&inserer, &_mutex);

		double n = 0;
        clearScreenDAP();
        cout << "The accepted coins are as follow : ";

		static vector<double> availablePieces = getAvailablePieces();
		for(vector<double>::iterator it = availablePieces.begin() ; it!=availablePieces.end() ; it++)
            cout << *it << ", ";
		cout << endl;
		do
		{
            cout << "Insert a coin : ";

            userInput(n);
			if(!isAvailablePiece(n))
			{
                cerr << "Coin rejected" << endl;
				n = 0;
			}
			else
                cout << "The coin of " << n << " has been accepted !" << endl;
		} while (n <= 0);

        static_cast<Piece*>(param)->setValue(n,ID_MONNAIE);
        pauseDAP();

		pthread_cond_signal(&vendeur);
        pthread_mutex_unlock(&_mutex);
	}
	return NULL;
}

void* DISTRIBUTOR(void* param)
{
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

	while(true)
	{
        pthread_mutex_lock(&_mutex);
        pthread_cond_wait(&distribuer, &_mutex);

        Commande* ach = static_cast<Commande*>(param);

        clearScreenDAP();
        cout << "Left monney : " << ach->getSolde() << endl;
		if(ach->getBouteille())
            cout << "You get 1 awesome teddy ! :D You'll get it when you quit the machine." << endl;
		else
            cout << "Yout don't get any teddy :/ " << endl;
        pauseDAP();

		pthread_cond_signal(&vendeur);
        pthread_mutex_unlock(&_mutex);
	}
	return NULL;
}

void* VENDOR(void* param)
{
    int nbBottles = NB_TEDDY;
    Gestion* gest = static_cast<Gestion*>(param);

	while(true)
	{
        clearScreenDAP();
        cout << "Wallet : " << setprecision(2) << fixed << gest->getDistrib()->getSolde() << endl;
        cout << "Left teddies : " << nbBottles << endl;
        cout << "Teddy price : " << PRICE_BOTTLE << endl << endl;

        pthread_mutex_lock(&_mutex);

		double newSolde = 0;
        switch(menuChoice())
		{
            case GET_TEDDY:
                gest->getDistrib()->setRecuBouteille(false,ID_VENDEUR);

                if(gest->getDistrib()->getSolde() >= PRICE_BOTTLE && nbBottles > 0)
				{
					newSolde = gest->getDistrib()->getSolde() - PRICE_BOTTLE;

                    gest->getDistrib()->setSolde(newSolde,ID_VENDEUR);
                    nbBottles--;
                    gest->getDistrib()->setRecuBouteille(true,ID_VENDEUR);
                }

                pthread_cond_signal(&distribuer);
                pthread_cond_wait(&vendeur,&_mutex);
                break;

            case INSERT_COIN:
                pthread_cond_signal(&inserer);
                pthread_cond_wait(&vendeur,&_mutex);

				newSolde = gest->getDistrib()->getSolde() + gest->getPiece()->getValue();
                gest->getDistrib()->setSolde(newSolde, ID_VENDEUR);
                break;

            case QUIT:
                clearScreenDAP();
                giveChange(gest->getDistrib()->getSolde());
                pthread_exit((void*)END_THREAD);
			break;
		}
        pthread_mutex_unlock(&_mutex);
	}
	return NULL;
}

void giveChange(double value)
{
    static vector<double> availablePieces = getAvailablePieces();
    vector<double>::iterator it = availablePieces.end()-1;
    cout << "Here your change my friend : ";
    bool flag = true;
    int times = 0;
    while(value > 0.0 || flag)
    {
        if(*it <= value)
        {
            value -= *it;
            ++times;
            flag = true;
        }
        else
        {
            if(times > 0)
                cout << times << "x" << *it << " ";
            --it;
            times = 0;
            flag = false;
        }
    }
    cout << endl;
}

void initializeMutexCond()
{
    pthread_mutex_init(&_mutex, NULL);
	pthread_cond_init (&inserer, NULL);
	pthread_cond_init (&distribuer, NULL);
	pthread_cond_init (&vendeur, NULL);
}

void destroyMutexCond()
{
    pthread_mutex_destroy(&_mutex);
	pthread_cond_destroy(&inserer);
	pthread_cond_destroy(&distribuer);
	pthread_cond_destroy(&vendeur);
}

void startAutomate()
{
	initializeMutexCond();

	pthread_t threads[3];

	Piece piece = Piece(0.0);
	Commande dis = Commande(0.0,false);
	Gestion ven = Gestion(&piece,&dis);

    pthread_create(&threads[0], NULL, VENDOR, &ven);
    pthread_create(&threads[1], NULL, DISTRIBUTOR, &dis);
    pthread_create(&threads[2], NULL, MONNEY, &piece);

	void* status;
	pthread_join(threads[0], &status);

    if(*static_cast<int*>(status) == END_THREAD)
	{
		pthread_cancel(threads[1]);
		pthread_cancel(threads[2]);
	}
	destroyMutexCond();
}

char menuChoice()
{
    cout << "To insert a coin, press key '" << INSERT_COIN << "'." <<endl;
    cout << "To get a bottle, press key '" << GET_TEDDY << "'." <<endl;
    cout << "To quit, press key '" << QUIT << "'." <<endl << endl;

    char c = 0;
	do
	{
        cout << "Choice : ";
		userInput(c);
        if(!isupper(c))
			c = toupper(c);
    } while(c != INSERT_COIN && c != GET_TEDDY && c != QUIT);
	return c;
}

bool doubleEquals(double a,double b)
{
    double e = 1e-3;
    return fabs(a - b) < e;
}

vector<double> getAvailablePieces()
{
	static vector<double> availablePieces;
	if(availablePieces.size() == 0)
	{
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

vector<string> getTeddies()
{
    static vector<string> availableTeddies;
    if(availableTeddies.size() == 0)
    {
        availableTeddies.push_back("  c___c\n  /. .\\\n  \\_T_/\n /`   `\\\n(/  .  \\)\n /';-;'\\\n()/   \\()\n");
        availableTeddies.push_back(",.   ,.\n\\.\\ /,/\n Y Y f\n |. .|\n(\"_, l\n ,- , \\\n(_)(_) Y,.\n _j _j |,'\n(_,(__,'\n");
        availableTeddies.push_back(".--,       .--,\n( (  \\.---./  ) )\n'.__/o   o\\__.'\n   {=  ^  =}\n    >  -  <\n   /       \\\n  //       \\\n //|   .   |\\\n \"'\\       /'\"_.-~^`'-.\n    \\  _  /--'         `\n   ___)( )(___\n (((__) (__)))\n");
        availableTeddies.push_back("               __\n       _,..,_ (, )\n    .,'      `,./\n  .' :`.----.': `,\n :   : ^    ^ :   ;\n:   :  6    6  :   ;\n:   :          :   ;\n:   :    __    :   ;\n :   `:'.--.`:'   ;\n  `.  : o  o :  .'\n   :   `----'   :\n     : .  :'`:  . :\n   `.:.'    `.:.'\n");
        availableTeddies.push_back("        ▄              ▄\n       ▌▒█           ▄▀▒▌\n       ▌▒▒▀▄       ▄▀▒▒▒▐\n      ▐▄▀▒▒▀▀▀▀▄▄▄▀▒▒▒▒▒▐\n    ▄▄▀▒▒▒▒▒▒▒▒▒▒▒█▒▒▄█▒▐\n  ▄▀▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▀██▀▒▌\n ▐▒▒▒▄▄▄▒▒▒▒▒▒▒▒▒▒▒▒▒▀▄▒▒▌\n ▌▒▒▐▄█▀▒▒▒▒▄▀█▄▒▒▒▒▒▒▒█▒▐\n▐▒▒▒▒▒▒▒▒▒▒▒▌██▀▒▒▒▒▒▒▒▒▀▄▌\n▌▒▀▄██▄▒▒▒▒▒▒▒▒▒▒▒░░░░▒▒▒▒▌\n▌▀▐▄█▄█▌▄▒▀▒▒▒▒▒▒░░░░░░▒▒▒▐\n▐▒▀▐▀▐▀▒▒▄▄▒▄▒▒▒▒▒░░░░░░▒▒▒▌\n▐▒▒▒▀▀▄▄▒▒▒▄▒▒▒▒▒▒░░░░░░▒▒▐\n▌▒▒▒▒▒▒▀▀▀▒▒▒▒▒▒▒▒░░░░▒▒▒▒▌\n▐▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▐\n ▀▄▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▄▒▒▒▒▌\n   ▀▄▒▒▒▒▒▒▒▒▒▒▄▄▄▀▒▒▒▒▄▀\n  ▐▀▒▀▄▄▄▄▄▄▀▀▀▒▒▒▒▒▄▄▀\n ▐▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▀▀\n");
    }
    return availableTeddies;
}

bool isAvailablePiece(double piece)
{
	static vector<double> availablePieces = getAvailablePieces();
	for(vector<double>::iterator it = availablePieces.begin() ; it != availablePieces.end() ; it++)
		if(doubleEquals(piece, *it))
			return true;
	return false;
}

template<typename T>
void userInput(T& v)
{
    cin.ignore(cin.rdbuf()->in_avail());
    cin >> v;
}

void clearScreenDAP()
{
    system("clear");
}

void pauseDAP()
{
    std::cout << std::endl << "PRESS ENTER TO CONTINUE" << std::endl;
    cin.ignore().get();
}
