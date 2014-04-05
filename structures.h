/*
 * Automate
 * Diego Antognini & S�bastien Vaucher
 * 17 D�cembre 2012
 * structures.h : Header des structures param�tres
 */

#ifndef STRUCTURES_H
#define STRUCTURES_H

//Identifiant des threads pour prot�ger les ressources
#define ID_VENDEUR 1
#define ID_MONNAIE 2
#define ID_DISTRIBUTEUR 3

//Param�tre utilis� dans le thread DISTRIBUTEUR et dans le param�tre de VENDEUR
class Commande
{
public:
	Commande(double s, bool b) : solde(s), bouteille(b) {}
	double getSolde() const { return solde; }
	double getBouteille() const { return bouteille; }

	void setSolde(double s, int id);
	void setRecuBouteille(bool b, int id);//D�termine si le syst�me peut donner une bouteille ou non
private:
	double solde;
	bool bouteille;
};

//Param�tre utilis� dans le thread MONNAIE
class Piece
{
public:
	Piece(double v):valeur(v) {}
	double getValue() const { return valeur; }
	void setValue(double v,int id);
private:
	double valeur;
};

//Param�tre utilis� dans le thread VENDEUR
class Gestion
{
public:
	Gestion(Piece* p,Commande* d):piece(p),distrib(d){}
	Piece* getPiece() const {return piece;}
	Commande* getDistrib() const {return distrib;}

private:
	//Ressource partag�e entre le thread VENDEUR (read only) et MONNAIE (write only)
	Piece* piece;
	//Ressource partag�e entre le thread VENDEUR (write only) et DISTRIBUTEUR (read only)
	Commande* distrib;
};

#endif