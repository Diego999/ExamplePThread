#ifndef STRUCTURES_H
#define STRUCTURES_H

#define ID_VENDEUR 1
#define ID_MONNAIE 2
#define ID_DISTRIBUTEUR 3

class Commande
{
public:
	Commande(double s, bool b) : solde(s), bouteille(b) {}
	double getSolde() const { return solde; }
	double getBouteille() const { return bouteille; }

	void setSolde(double s, int id);
    void setRecuBouteille(bool b, int id);
private:
	double solde;
	bool bouteille;
};

class Piece
{
public:
	Piece(double v):valeur(v) {}
	double getValue() const { return valeur; }
	void setValue(double v,int id);
private:
	double valeur;
};

class Gestion
{
public:
	Gestion(Piece* p,Commande* d):piece(p),distrib(d){}
	Piece* getPiece() const {return piece;}
	Commande* getDistrib() const {return distrib;}

private:
	Piece* piece;
	Commande* distrib;
};

#endif
