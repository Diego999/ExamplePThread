package ch.hearc.progconc.labojava2;

/**
 * <h1>Impl�mentation</h1>
 * Pour la gestion des 2 zones, nous avons cr�� une classe parent abstraite "ProtectedZone", repr�sentant une zone. Dans cette classe, on retrouve son emplacement, ses dimensions,
 * sa couleur, le rectangle repr�sentant l'ensemble de la zone, son nom puis un attribut par type d'objet. Concernant les m�thodes non-abstract, 
 * on peut y voir une m�thode "isInZone" recevant un objet graphique et retournant un bool�ean indiquant si un objet se trouve � l'int�rieur. 
 * <br />Il y a encore une autre m�thode non-abstract, la m�thode "exit" qui permet � un objet graphique de sortir de la zone et de laisser un autre objet du m�me type entrer.
 * <br /><br />
 * Pour les m�thodes abstracts il y a :
 * <br />- iWantToEnter, permettant de faire entrer un objet dans la zone
 * <br />- releaseXXX, permettant de lib�rer un objet de type XXX de la zone.
 * 
 * <br /><br />Nous avons ensuite 2 classes enfants : ProtectedZoneReetrantLock, repr�sentant la zone utilisant le ReetrantLock, puis ProtectedZoneSemaphore, repr�sentant la zone utilisant le Semaphore.
 * <br />
 * Ces 2 classes impl�mentent les m�thodes abstraites en utilisant les m�thodes associ�s au bon outil de programmation concurrente.
 * <br />A l'ajout d'un objet, on le g�n�re en dehors des zones, pour �viter toutes incoh�rences (qu'il y ait 2 objets du m�me type dans la m�me zone, m�me si l'un d'entre eux ne bougera pas),
 * <br /> ReetrantLock utilisera lockInterruptibly(), Semaphore un acquire(), et l'attribut correspondant au type d'objet se voit attribu� par l'objet entrant.
 * <br />A la sortie d'un objet, celui-ci est mis � null puis on utilise, dans ReetrantLock unlock() et dans Semaphore release().
 * <br /><br />
 * Lorsque l'on clique sur le bouton "effacer", il se passe les choses suivantes :
 * <br />1) Nous interrompons tous les threads des objets graphiques
 * <br />2) Nous supprimons tous les objets du vector contenant les objets
 * <br />3) Nous r�initalisons les zones prot�g�es, avec la m�thode init(), qui est aussi utilis�e dans le constructeur
 * 
 * 
 * <h1>V�rification du fonctionnement</h1>
 * Pour tester l'ensemble de notre programme, nous avons dessin� nos 2 zones de contr�les (Semaphore + ReentrantLock), 
 * puis avons g�n�r� un grand nombre d'objets.<br /> Pour chacune de ces zones, il devait y avoir, au maximum, un objet graphique de chaque type
 * � l'int�rieur. Les autres objets n'�tant pas dedans attendent � l'ext�rieur.<br />
 * Pour le deuxi�me test, nous cliquons sur le bouton "Effacer" et refaisons la m�me manipulation. 
 * <br />Si l'ensemble marche � nouveau sans erreur, c'est que le test est concluant.
 * 
 * Les 2 cas fonctionnent correctement.<br /><br />
 * 
 * @author Diego Antognini
 * @author S�bastien Vaucher
 */
class Principale
{
	public static void main(String args[])
	{

		AjoutComposant maFrame = new AjoutComposant("Ajouter Graphique");
		maFrame.setSize(1024,768);
		maFrame.setVisible(true);
	}
}