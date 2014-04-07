#ifndef STRUCTURES_H
#define STRUCTURES_H

#define ID_VENDOR 1
#define ID_MONNEY 2
#define ID_DISTRIBUTOR 3

class Command
{
public:
    Command(double s, bool b) : wallet(s), teddy(b) {}
    double getWallet() const { return wallet; }
    double getTeddy() const { return teddy; }

    void setWallet(double s, int id);
    void setTeddy(bool b, int id);
private:
    double wallet;
    bool teddy;
};

class Coin
{
public:
    Coin(double v):value(v) {}
    double getValue() const { return value; }
	void setValue(double v,int id);
private:
    double value;
};

class Manage
{
public:
    Manage(Coin* c,Command* d):coin(c),distrib(d){}
    Coin* getCoin() const {return coin;}
    Command* getDistrib() const {return distrib;}

private:
    Coin* coin;
    Command* distrib;
};

#endif
