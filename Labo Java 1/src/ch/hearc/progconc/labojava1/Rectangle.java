package ch.hearc.progconc.labojava1;

import java.awt.Graphics;

public class Rectangle extends ObjetGraphique
{

	// Constructeurs
	public Rectangle(int x, int y, int width, int height)
	{
		super(x, y, width, height);

	}

	public void dessineToi(Graphics gc)
	{

		gc.drawRect(x - (largeur / 2), y - (hauteur / 2), largeur, hauteur);
	}
}
