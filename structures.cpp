#include "structures.h"

void Command::setWallet(double w,int id)
{
    if(id == ID_VENDOR)
        wallet = w;
}

void Command::setTeddy(bool t,int id)
{
    if(id == ID_VENDOR)
        teddy = t;
}

void Coin::setValue(double v,int id)
{
    if(id == ID_MONNEY)
        value = v;
}
