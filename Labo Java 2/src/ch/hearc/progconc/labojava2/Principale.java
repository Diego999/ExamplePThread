package ch.hearc.progconc.labojava2;

/**
 * <h1>Impl�mentation</h1>
 * Nous avons repr�sent� la barrier avec la classe Barrier qui contient autant de CyclicBarrier que de type d'objets rentrant. <br />
 * Pour cela, nous les stockons dans une Map.
 * 
 * Lorsqu'un objet touche la barri�re, repr�sent� par une ligne, celui-ci se appelle la m�thode iWantToPass, qui va le mettre en attente.<br />
 * Une fois que 3 objets du m�me types sont pr�sents, la barri�re les lib�re,les laisse repartir et attent � nouveau 3 objets du m�me type.
 * <br />
 * Pour �viter toute incoh�rence, nous g�n�rons les objets en dehors de la barri�re.
 * <br />
 * Lorsque l'on clique sur le bouton "effacer", il se passe les choses suivantes :
 * <br />1) Nous interrompons tous les threads des objets graphiques
 * <br />2) Nous supprimons tous les objets du vector contenant les objets
 * <br />3) Nous r�initalisons la barri�re, avec la m�thode resetBarrier, appelant la m�thode clear de la classe CyclicBarrier.
 * 
 * 
 * <h1>V�rification du fonctionnement</h1>
 * Pour tester l'ensemble de notre programme, nous avons dessin� notre CyclicBarrier, puis avons g�n�r� un certain nombre d'objets.
 * <br /> Pour la barri�re, il doit avoir au maximum 3 objets graphiques du m�me type ! Une fois ce nombre atteint, les
 * objets repartent correctement s'en s'arr�ter � nouveau et les autres objets du m�me type arrivants, s'arr�tent en attendant d'�tre 3(sans compter ceux qui viennet de partir).<br />
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