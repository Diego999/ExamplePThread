/*
 * Automate
 * Diego Antognini & S�bastien Vaucher
 * 17 D�cembre 2012
 * structures.cpp : Fichiers d'impl�mentation des structures param�tres
 */

#include "structures.h"

void Commande::setSolde(double s,int id)
{
	if(id == ID_VENDEUR)
		solde = s;
}

void Commande::setRecuBouteille(bool b,int id)
{
	if(id == ID_VENDEUR)
		bouteille = b;
}

void Piece::setValue(double v,int id)
{
	if(id == ID_MONNAIE)
		valeur = v;
}