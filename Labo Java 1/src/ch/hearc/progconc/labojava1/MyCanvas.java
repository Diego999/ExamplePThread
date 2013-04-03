package ch.hearc.progconc.labojava1;

import java.awt.Canvas;
import java.awt.Color;
import java.awt.Graphics;
import java.awt.Image;
import java.awt.Point;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.awt.event.ComponentListener;
import java.util.Date;
import java.util.Enumeration;
import java.util.Random;
import java.util.Vector;

/**
 * Cette classe fournit un canvas affichant des objets graphiques Dessinable � intervalles r�guliers
 */
@SuppressWarnings("serial")
class MyCanvas extends Canvas implements Runnable
{

	private Vector<ObjetGraphique> listeObjetsDessinables = new Vector<ObjetGraphique>();
	// Pour le double buffer
	private Image offScrImage;
	private Graphics offScrGC;
	private int largeur, hauteur;
	// temps en millisecondes entre chaque frame d'animation

	// Changement de couleurs
	private Random alea;
	
	// Zones prot�g�es
	private final ProtectedZone[] pz = {
			//new ProtectedZoneReetrantLock(new Point(500, 100)),
			new ProtectedZoneSemaphore(new Point(50, 200))
			};

	public MyCanvas()
	{

		// Classe anonyme interne pour g�rer l'�v�nement resize()
		ComponentListener resizeCallback = new ComponentAdapter()
		{
			public void componentResized(ComponentEvent e)
			{
				// D�finition de la zone d'�volution des cercles.
				largeur = getSize().width;
				hauteur = getSize().height;

				ObjetGraphique.setZoneEvolution(largeur, hauteur);

				// Le double buffer = une image m�moire de la taille de la
				// zone d'�volution
				// des cercles
				offScrImage = createImage(largeur, hauteur);

				// R�cup�ration d'un contexte graphique pour dessiner dans le
				// double buffer.
				offScrGC = offScrImage.getGraphics();
				offScrGC.setColor(Color.red);

				//System.out.println("Canvas Drawing1 retaill�e : width = " + largeur + "height = " + hauteur);
			}
		}; // Fin de la classe anonyme

		// On indique qu'en cas de retaillage du canvas, c'est l'objet
		// resizeCallback
		// qui va traiter l'�v�nement
		addComponentListener(resizeCallback);
		alea = new Random((new Date()).getTime());
		new Thread(this).start();
	}

	/**
	 * Ajoute � la liste des objets affich� � chaque frame d'animation un objet de plus
	 */
	public void ajouteObjetDessinable(ObjetGraphique objet)
	{
		listeObjetsDessinables.addElement(objet);
	}
	
	public ProtectedZone[] getProtectedZones()
	{
		return pz;
	}

	/**
	 * On surcharge update() car on fait du double buffer. On ne veut pas effacer � la fois la fen�tre graphique de l'applet, ce que fait par d�faut update(),
	 * et effacer en plus le double buffer dans paint()
	 */
	public void update(Graphics g)
	{
		paint(g);
	}

	/** Modifie la couleur du trac� des objets fil de fer */
	public void changeCouleur()
	{
		offScrGC.setColor(new Color(Math.abs(alea.nextInt()) % 256, Math.abs(alea.nextInt()) % 256, Math.abs(alea.nextInt()) % 256));
	}

	public void clear()
	{
		for (ObjetGraphique og : listeObjetsDessinables)
		{
			og.interruptThread();
		}
		listeObjetsDessinables.removeAllElements();
		for (ProtectedZone p : pz)
		{
			p.init();
		}
	}

	/**
	 * La fonction qui effectue l'affichage des objets graphiques que doit g�rer cette classe
	 */
	public void paint(Graphics gc)
	{
		// appel� automatiquement par l'AWT, ou sur demande par repaint()
		Enumeration<ObjetGraphique> liste = listeObjetsDessinables.elements();
		ObjetGraphique objetDessinable;

		// On efface le double buffer
		offScrGC.clearRect(0, 0, largeur, hauteur);

		// On affiche les objets anim�s
		while (liste.hasMoreElements())
		{
			objetDessinable = (ObjetGraphique) liste.nextElement();

			// On dessine un cercle dans le double buffer
			objetDessinable.dessineToi(offScrGC);
		}
		
		for (ProtectedZone p : pz)
		{
			p.draw(offScrGC);
		}

		// On recopie le double buffer dans le canvas
		gc.drawImage(offScrImage, 0, 0, this);
	}

	public void run()
	{

		while (true)
		{
			// On demande � redessiner les cercles
			repaint();

			// On "dort" tempsEntreFrame millisecondes
			try
			{
				Thread.sleep(100);
			}
			catch (InterruptedException e)
			{
				System.out.println("Erreur dans le sleep(tempsEntreFrames);");
				e.printStackTrace();
			}
		}
	}

}