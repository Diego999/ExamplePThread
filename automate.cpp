#include "automate.h"
#include "structures.h"

#include <cmath>
#include <ctype.h>
#include <iomanip>
#include <iostream>
#include <pthread.h>
#include <vector>
#include <algorithm>

using namespace std;

pthread_mutex_t _mutex;

pthread_cond_t insert;
pthread_cond_t distribute;
pthread_cond_t vendor;

void* MONNEY(void* param)
{
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

	while(true)
	{
        pthread_mutex_lock(&_mutex);
        pthread_cond_wait(&insert, &_mutex);

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

        static_cast<Coin*>(param)->setValue(n,ID_MONNEY);
        pauseDAP();

        pthread_cond_signal(&vendor);
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
        pthread_cond_wait(&distribute, &_mutex);

        Command* ach = static_cast<Command*>(param);

        clearScreenDAP();
        cout << "Left monney : " << ach->getWallet() << endl;
        if(ach->getTeddy())
            cout << "You get 1 awesome teddy ! :D" << endl;
        else if(getTeddies().size() <= 0)
            cout << "There isn't any more teddies :/ We have been stolen by a tux. Oh god, how it happened ?" << endl;
        else if(ach->getWallet() <= PRICE_TEDDY)
            cout << "You don't have enough money ! Get find a job moron ... Or just insert coins if you already have one !" << endl;
            cout << "You don't have enough money ! Get find a job moron ... Or just insert coins if you already have some !" << endl;
        pauseDAP();

        pthread_cond_signal(&vendor);
        pthread_mutex_unlock(&_mutex);
	}
	return NULL;
}

void* VENDOR(void* param)
{
    int nbTeddies = NB_TEDDY;
    Manage* gest = static_cast<Manage*>(param);

	while(true)
	{
        clearScreenDAP();
        cout << "Wallet : " << setprecision(2) << fixed << gest->getDistrib()->getWallet() << endl;
        cout << "Left teddies : " << nbTeddies << endl;
        cout << "Teddy price : " << PRICE_TEDDY << endl << endl;

        pthread_mutex_lock(&_mutex);

        double newWallet = 0;
        switch(menuChoice())
		{
            case GET_TEDDY:
                gest->getDistrib()->setTeddy(false,ID_VENDOR);

                if(gest->getDistrib()->getWallet() >= PRICE_TEDDY && nbTeddies > 0)
                {
                    vector<vector<string> >& teddiesBank = getTeddies();
                    vector<string> teddies = teddiesBank.back();
                    teddiesBank.erase(teddiesBank.end()-1);
                    for(size_t j = 0; j <= teddies.size(); ++j)
                    {
                        for(int k = j; k > 0; --k)
                            cout << teddies[teddies.size()-k] << endl;
                        clearScreenDAP();
                    }
                    for(size_t j = 1; j <= teddies.size(); ++j)
                    {
                        for(size_t k = 0; k < teddies.size()-j; ++k)
                            cout << teddies[k] << endl;
                        clearScreenDAP();
                    }

                    newWallet = gest->getDistrib()->getWallet() - PRICE_TEDDY;
                    gest->getDistrib()->setWallet(newWallet,ID_VENDOR);
                    --nbTeddies;
                    gest->getDistrib()->setTeddy(true,ID_VENDOR);
                }

                pthread_cond_signal(&distribute);
                pthread_cond_wait(&vendor,&_mutex);
                break;

            case INSERT_COIN:
                pthread_cond_signal(&insert);
                pthread_cond_wait(&vendor,&_mutex);

                newWallet = gest->getDistrib()->getWallet() + gest->getCoin()->getValue();
                gest->getDistrib()->setWallet(newWallet, ID_VENDOR);
                break;

            case QUIT:
                clearScreenDAP();
                giveChange(gest->getDistrib()->getWallet());
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
    pthread_cond_init (&insert, NULL);
    pthread_cond_init (&distribute, NULL);
    pthread_cond_init (&vendor, NULL);
}

void destroyMutexCond()
{
    pthread_mutex_destroy(&_mutex);
    pthread_cond_destroy(&insert);
    pthread_cond_destroy(&distribute);
    pthread_cond_destroy(&vendor);
}

void startAutomate()
{
	initializeMutexCond();

	pthread_t threads[3];

    Coin piece = Coin(0.0);
    Command dis = Command(0.0,false);
    Manage ven = Manage(&piece,&dis);

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
    cout << "To get a teddy, press key '" << GET_TEDDY << "'." <<endl;
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

vector<vector<string> >& getTeddies()
{
    static vector<vector<string> > availableTeddies;
    static bool init = false;
    if(!init)
    {
        init = true;
        std::srand ( unsigned ( std::time(0) ) );
        {
            vector<string> v1;
            v1.push_back("  c___c");
            v1.push_back("  /. .\\");
            v1.push_back("  \\_T_/");
            v1.push_back(" /`   `\\");
            v1.push_back("(/  .  \\)");
            v1.push_back(" /';-;'\\");
            v1.push_back("()/   \\()");
            availableTeddies.push_back(v1);
        }

        {
            vector<string> v1;
            v1.push_back(",.   ,.");
            v1.push_back("\\.\\ /,/");
            v1.push_back(" Y Y f");
            v1.push_back(" |. .|");
            v1.push_back("(\"_, l");
            v1.push_back(" ,- , \\");
            v1.push_back("(_)(_) Y,.");
            v1.push_back(" _j _j |,'");
            v1.push_back("(_,(__,'");
            availableTeddies.push_back(v1);
        }

        {
            vector<string> v1;
            v1.push_back(" .--,       .--,");
            v1.push_back("( (  \\.---./  ) )");
            v1.push_back("' .__/o   o\\__.'");
            v1.push_back("    {=  ^  =}");
            v1.push_back("     >  -  <");
            v1.push_back("    /       \\");
            v1.push_back("   //       \\");
            v1.push_back("  //|   .   |\\");
            v1.push_back("  \"'\\       /'\"_.-~^`'-.");
            v1.push_back("     \\  _  /--'         `");
            v1.push_back("   ___)( )(___");
            v1.push_back("  (((__) (__)))");
            availableTeddies.push_back(v1);
        }

        {
            vector<string> v1;
            v1.push_back("               __");
            v1.push_back("       _,..,_ (, )");
            v1.push_back("    .,'      `,./");
            v1.push_back("  .' :`.----.': `,");
            v1.push_back(" :   : ^    ^ :   ;");
            v1.push_back(":   :  6    6  :   ;");
            v1.push_back(":   :          :   ;");
            v1.push_back(":   :    __    :   ;");
            v1.push_back(" :   `:'.--.`:'   ;");
            v1.push_back("  `.  : o  o :  .'");
            v1.push_back("   :   `----'   :");
            v1.push_back("     : .  :'`:  . :");
            v1.push_back("   `.:.'    `.:.'");
            availableTeddies.push_back(v1);
        }

        {
            vector<string> v1;
            v1.push_back("        ▄              ▄");
            v1.push_back("       ▌▒█           ▄▀▒▌");
            v1.push_back("       ▌▒▒▀▄       ▄▀▒▒▒▐");
            v1.push_back("      ▐▄▀▒▒▀▀▀▀▄▄▄▀▒▒▒▒▒▐");
            v1.push_back("    ▄▄▀▒▒▒▒▒▒▒▒▒▒▒█▒▒▄█▒▐");
            v1.push_back("  ▄▀▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▀██▀▒▌");
            v1.push_back(" ▐▒▒▒▄▄▄▒▒▒▒▒▒▒▒▒▒▒▒▒▀▄▒▒▌");
            v1.push_back(" ▌▒▒▐▄█▀▒▒▒▒▄▀█▄▒▒▒▒▒▒▒█▒▐");
            v1.push_back("▐▒▒▒▒▒▒▒▒▒▒▒▌██▀▒▒▒▒▒▒▒▒▀▄▌");
            v1.push_back("▌▒▀▄██▄▒▒▒▒▒▒▒▒▒▒▒░░░░▒▒▒▒▌");
            v1.push_back("▌▀▐▄█▄█▌▄▒▀▒▒▒▒▒▒░░░░░░▒▒▒▐");
            v1.push_back("▐▒▀▐▀▐▀▒▒▄▄▒▄▒▒▒▒▒░░░░░░▒▒▒▌");
            v1.push_back("▐▒▒▒▀▀▄▄▒▒▒▄▒▒▒▒▒▒░░░░░░▒▒▐");
            v1.push_back("▌▒▒▒▒▒▒▀▀▀▒▒▒▒▒▒▒▒░░░░▒▒▒▒▌");
            v1.push_back("▐▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▐");
            v1.push_back(" ▀▄▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▄▒▒▒▒▌");
            v1.push_back("   ▀▄▒▒▒▒▒▒▒▒▒▒▄▄▄▀▒▒▒▒▄▀");
            v1.push_back("  ▐▀▒▀▄▄▄▄▄▄▀▀▀▒▒▒▒▒▄▄▀");
            v1.push_back(" ▐▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▀▀");
            availableTeddies.push_back(v1);
        }
    }
    std::random_shuffle(availableTeddies.begin(), availableTeddies.end());
    return availableTeddies;
}

bool isAvailablePiece(double piece)
{
	static vector<double> availablePieces = getAvailablePieces();
	for(vector<double>::iterator it = availablePieces.begin() ; it != availablePieces.end() ; it++)
        if(fabs(piece-*it) < 1e-3)
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
    //for(int n = 0; n < 10; n++)
    //    printf("\n\n");
    sleep(1);
}

void pauseDAP()
{
    std::cout << std::endl << "PRESS ENTER TO CONTINUE" << std::endl;
    cin.ignore().get();
}
