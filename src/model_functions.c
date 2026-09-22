/*
 * model_functions.c
 *
 *  Created on: 10 sept. 2025
 *      Author: constance
 */
#define _XOPEN_SOURCE_EXTENDED 1

#include <stdlib.h>
#include <string.h>
#include <ncursesw/curses.h>
#include <locale.h>
#include <wchar.h>
#include "structures.h"
#include "model_functions.h"

// Fonction pour convertir un code ASCII étendu en UTF-8
void asciiExtendedToUTF8(int asciiCode, char *utf8Buffer) {
	// Table de correspondance pour certains caractères ASCII étendus
	switch (asciiCode) {
	case 179: // │ (Box Drawings Light Vertical)
		strcpy(utf8Buffer, "\u2502");
		break;
	case 194: // ┬ (Box Drawings  Down and Horizontal)
		strcpy(utf8Buffer, "\u252C");
		break;
	case 191: // ┐ (Box Drawings Down and Right)
		strcpy(utf8Buffer, "\u2510");
		break;
	case 217: // ┘ (Box Drawings Up and Left)
		strcpy(utf8Buffer, "\u2518");
		break;
	case 218: // ┌ (Box Drawings Up and Right)
		strcpy(utf8Buffer, "\u250C");
		break;
	case 192: // └ (Box Drawings Down and Left)
		strcpy(utf8Buffer, "\u2514");
		break;
	case 193: // ┴ (Box Drawings Up and Horizontal)
		strcpy(utf8Buffer, "\u2534");
		break;
	case 195: // ├ (Box Drawings Vertical and Right)
		strcpy(utf8Buffer, "\u251C");
		break;
	case 196: // ─  (Box Drawings Horizontal)
		strcpy(utf8Buffer, "\u2501");
		break;
	case 197: // ┼ (Box Drawings Vertical and Horizontal)
		strcpy(utf8Buffer, "\u253C");
		break;
	case 180: // ┤ (Box Drawings Vertical and Left)
		strcpy(utf8Buffer, "\u2524");
		break;
	case 205: // ═ (Box Drawings Double Horizontal)
		strcpy(utf8Buffer, "\u2550");
		break;
	case 201: // ╔ (Box Drawings Double Down and Right)
		strcpy(utf8Buffer, "\u2554");
		break;
	case 187: // ╗ (Box Drawings Double Down and Left)
		strcpy(utf8Buffer, "\u2557");
		break;
	case 200: // ╚ (Box Drawings Double Up and Right)
		strcpy(utf8Buffer, "\u255A");
		break;
	case 188: // ╝ (Box Drawings Double Up and Left)
		strcpy(utf8Buffer, "\u255D");
		break;
	case 202: // ╩ (Box Drawings Double Up and Horizontal)
		strcpy(utf8Buffer, "\u2569");
		break;
	case 203: // ╦ (Box Drawings Double Down and Horizontal)
		strcpy(utf8Buffer, "\u2566");
		break;
	case 186: // ║ (Box Drawings Double Vertical)
		strcpy(utf8Buffer, "\u2551");
		break;
	case 204: // ╠ (Box Drawings Double Vertical and Right)
		strcpy(utf8Buffer, "\u2560");
		break;
	case 185: // ╣ (Box Drawings Double Vertical and Left)
		strcpy(utf8Buffer, "\u2563");
		break;
	case 206: // ╬ (Box Drawings Double Vertical and Horizontal)
		strcpy(utf8Buffer, "\u256C");
		break;
	default:
		// Si le code ASCII étendu n'est pas dans la table, retourner un espace
		strcpy(utf8Buffer, " ");
		break;
	}
}

/*
 * Charge le plan du parking depuis un fichier et retourne un tableau de lignes wide-char
 *
 * Lit le fichier ligne par ligne, nettoie les retours à la ligne, alloue chaque entrée,
 * et remplit un tableau de taille screen_height. Les lignes manquantes sont initialisées à NULL
 *
 * @param nom_fichier Chemin du fichier contenant le plan à charger
 *
 * @return Tableau dynamique de lignes wide-char, ou NULL en cas d’erreur
 */
wchar_t** setParkingPlan(int place_largeur, int place_longueur, int nombre_emplacement_longueur, int nombre_rangee, int ColorPair, char *nom_fichier) {
	extern int screen_height, screen_width;
	setlocale(LC_ALL, "fr_FR.UTF-8");  // Initialisation de la locale

	FILE *fichier = fopen(nom_fichier, "r");
	if (fichier == NULL) {
		perror("Erreur lors de l'ouverture du fichier ...");
		return NULL;
	}

	// Allouer le tableau de pointeurs
	wchar_t **plan = malloc(screen_height * sizeof(wchar_t*));
	if (plan == NULL) {
		perror("Erreur d'allocation mémoire pour le plan");
		fclose(fichier);
		return NULL;
	}

	wchar_t buffer[screen_width];
	int i = 0;
	while (i < screen_height && fgetws(buffer, screen_width, fichier) != NULL) {
		// Supprimer le \n
		int k = 0;
		while (k < screen_width - 1 && buffer[k] != L'\n') {
			k++;
		}
		buffer[k - 1] = L'\0';
		// Ignorer les lignes vides
		if (wcslen(buffer) == 0) {
			continue;
		}
		// Allouer la ligne
		plan[i] = malloc(screen_width * sizeof(wchar_t));
		if (plan[i] == NULL) {
			perror("Erreur d'allocation mémoire pour une ligne");
			// Libérer la mémoire déjà allouée
			for (int j = 0; j < i; j++) {
				free(plan[j]);
			}
			free(plan);
			fclose(fichier);
			return NULL;
		}
		// Copier la ligne
		wcscpy(plan[i], buffer);
		i++;
	}
	// Si le fichier a moins de lignes que screen_height, initialiser le reste à NULL
	for (int j = i; j < screen_height; j++) {
		plan[j] = NULL;
	}

	fclose(fichier);
	return plan;
}

/*
 * Initialise une structure parking avec ses paramètres et charge son plan associé
 *
 * @return Un pointeur vers la structure parking nouvellement allouée
 */
parking* setParking(int place_largeur, int place_longueur,int nombre_emplacement_longueur, int nombre_rangee, int ColorPair, char *fichier_nom, int taux_horaire) {
	parking *p = malloc(sizeof(parking));
	p->place_largeur = place_largeur;
	p->place_longueur = place_longueur;
	p->nombre_emplacement_longueur = nombre_emplacement_longueur;
	p->nombre_rangee = nombre_rangee;
	p->fichier_nom = fichier_nom;
	p->taux_horaire = taux_horaire;
	p->prix_a_payer = 0;
	p->plan = setParkingPlan(place_largeur, place_longueur, nombre_emplacement_longueur, nombre_rangee, ColorPair, fichier_nom);
	return p;
}


/*
 * Création la carrosserie d’un véhicule (voitures et camions) selon son type
 * Remplit le tableau de chaînes `carrosserie` du véhicule pour l’affichage
 *
 * @param v Pointeur vers le véhicule
 */
void setCarrosserie(vehicule *v) {
	switch (v->type) {
	case 'v':
		strcpy(v->carrosserie[0], "┌─┐|╠═╣|║ ║|╚═╝");
		strcpy(v->carrosserie[1], "╔═╗|║ ║|╠═╣|└─┘");
		strcpy(v->carrosserie[2], "┌─╦══╗|└─╩══╝|");
		strcpy(v->carrosserie[3], "╔══╦─┐|╚══╩─┘|");
		break;
	case 'c':
		strcpy(v->carrosserie[0], "┌═══┐|╠═══╣|║   ║|╚═══╝");
		strcpy(v->carrosserie[1], "╔═══╗|║   ║|╠═══╣|└═══┘");
		strcpy(v->carrosserie[2], "┌─╦═══╗|║ ║   ║|└─╩═══╝");
		strcpy(v->carrosserie[3], "╔═══╦─┐|║   ║ ║|╚═══╩─┘");
		break;
	}
}

/*
 * Crée et initialise un véhicule avec ses attributs et sa carrosserie
 * @return Pointeur vers le véhicule nouvellement alloué et configuré
 */
vehicule* setVehicule(char direction, int posx, int posy, int vitesse, char type, char specificite, int code_couleur, time_t h_arrivee, int etat) {
	extern int immatriculation;
	vehicule *v = malloc(sizeof(vehicule));
	v->direction = direction;
	v->posx = posx;
	v->posy = posy;
	v->vitesse = vitesse;
	v->type = type;
	v->specificite = specificite;
	v->code_couleur = code_couleur;
	setCarrosserie(v);
	v->instantArrivee = h_arrivee;
	v->instantSortie = h_arrivee;
	v->prix_parking = 0;
	v->etat = etat;
	v->suivant = NULL;
	v->identifiant = immatriculation++;
	return v;
}

/*
 * Affiche les informations principales d’un véhicule dans le terminal
 *
 * Affiche le type (voiture ou camion) et la spécificité (électrique, handicap)
 * du véhicule, puis calcule la durée écoulée depuis l’arrivée ou jusqu’à la sortie
 * Les calculs incluent heures, minutes et secondes et le prix total du parking
 *
 * @param v Pointeur vers le véhicule dont les informations doivent être affichées
 *
 * @note Les sorties détaillées du débogage sont commentées dans le code
 * @note idée de crée un second terminal uniquement de data pour réaliser un suivi
 */
void afficheVehicule(vehicule *v) {
	switch (v->type) {
	case 'v':
		printf("Voiture ");
		break;
	case 'c':
		printf("Camion ");
		break;
	}
	switch (v->specificite) {
	case 'e':
		printf("Électrique ");
		break;
	case 'h':
		printf("Handicap ");
		break;
	}
	// Convertir en une structure tm (heure locale)
	struct tm *heure_locale_arrivee = localtime(&(v->instantArrivee));
	char buffer_arrivee[80];

	strftime(buffer_arrivee, sizeof(buffer_arrivee), "%H:%M:%S.", heure_locale_arrivee);
	double seconds;
	int hours, minutes, prix, secs, prixEuro, prixCent;

	if (v->instantArrivee != v->instantSortie) {
		struct tm *heure_locale_depart = localtime(&(v->instantSortie));
		char buffer_depart[80];

		strftime(buffer_depart, sizeof(buffer_depart), "%H:%M:%S.", heure_locale_depart);
		seconds = difftime(v->instantSortie, v->instantArrivee);
		hours = (int) (seconds / 3600);
		minutes = (int) ((seconds - hours * 3600) / 60);
		secs = (int) (seconds - hours * 3600 - minutes * 60);
		prix = v->prix_parking;
		prixEuro = prix/100;
		prixCent = prix%100;
		//DEBUG AFFICHAGE
		//printf("immat : %d arrivée à %s, sortie à %s, restée %dh-%dm-%ds, payée %d€%d\r\n", v->identifiant, buffer_arrivee, buffer_depart, hours, minutes, secs, prixEuro, prixCent);
	}
	else {
		seconds = difftime(time(NULL), v->instantArrivee);
		hours = (int) (seconds / 3600);
		minutes = (int) ((seconds - hours * 3600) / 60);
		secs = (int) (seconds - hours * 3600 - minutes * 60);
		//DEBUG AFFICHAGE
		//printf("immat : %d arrivée à %s, soit depuis %dh-%dm-%ds Actuellement positionné en (%d,%d), en direction de %c\r\n", v->identifiant, buffer_arrivee, hours, minutes, secs, v->posx, v->posy, v->direction);
	}

}

/*
 * Parcourt et affiche la liste des véhicules selon leur catégorie
 * Selon le type de liste (entrants, garés, sortants, sortis), parcourt la
 * liste chaînée des véhicules et appelle `afficheVehicule` pour chacun
 *
 * @param lv Pointeur vers la structure listeVehicules_s à afficher
 *
 * @note Les appels à l’affichage sont actuellement commentés, pour le second terminal
 */
void afficheListeVehicule(listeVehicules_s *lv) {
	//printf("\r\nListe des véhicules ");
	switch (lv->type) {
	case 1: // véhicules entrants
		//printf("entrants\r\n");
		break;
	case 2: //véhicules garés
		//printf("garés\r\n");
		break;
	case 3: // véhicules sortants
		//printf("sortants\r\n");
		break;
		case 4: // véhicules sortis
		 //printf("sortis\r\n");
		 break;
	}
	vehicule *v = lv->listeVehicules;
	//if (lv->type != 4) {

		if (v == NULL) {
			//printf("Liste vide\r");
		} else {
			while (v != NULL) {
				//afficheVehicule(v);
				v = v->suivant;
			}
		}
//	}
}

/*
 * Crée et initialise une liste de véhicules vide
 * Alloue dynamiquement une structure `listeVehicules_s`, initialise le pointeur
 * vers la liste chaînée à NULL, le compteur de véhicules à 0, et définit le type
 * de liste (entrants, garés, sortants, etc.)
 *
 * @param typeListe Type de la liste à créer
 * @return Pointeur vers la liste de véhicules nouvellement allouée
 */
listeVehicules_s* setListeVehiculeVide(int typeListe) {
	listeVehicules_s *l = malloc(sizeof(listeVehicules_s));
	l->listeVehicules = malloc(sizeof(vehicule*));
	l->listeVehicules = NULL;
	l->nbVehicules = 0;
	l->type = typeListe;
	// *l = NULL;
	return l;
}

/*
 * Ajoute un véhicule en tête d’une liste de véhicules et met à jour ses attributs selon le type de liste
 * Met à jour la couleur, la vitesse ou l’heure de sortie du véhicule selon que
 * la liste représente des véhicules entrants, garés, sortants ou sortis
 * Ensuite, insère le véhicule en tête de la liste chaînée et incrémente le compteur
 *
 * @param v  Pointeur vers le véhicule à ajouter
 * @param lv Pointeur vers la liste de véhicules cible
 */
void addVehiculeHeadListe(vehicule *v, listeVehicules_s *lv) {
	switch (lv->type) {
	case 1: // véhicules entrants
		if (v->specificite == 's'){
			v->code_couleur = 2; // vert
		}else if (v->specificite == 'e'){
			v->code_couleur = 11; // magenta
		}else if (v->specificite == 'h'){
			v->code_couleur = 5; // cyan
		}

		break;
	case 2: //véhicules garés
		v->code_couleur = 6; // gris
		v->vitesse = 0;
		break;
	case 3: // véhicules sortants
		v->code_couleur = 1; // rouge
		v->vitesse = 4;
		break;
	case 4:
		v->instantSortie = time(NULL);
		break;
	}
	if (lv->nbVehicules == 0) {
		v->suivant = NULL;
	} else {
		v->suivant = lv->listeVehicules;
	}
	lv->listeVehicules = v;
	lv->nbVehicules += 1;
}

/*
 * Parcourt la liste chaînée et supprime le véhicule correspondant à l'identifiant (immatriculation)
 * Met à jour le pointeur de tête et le compteur de véhicules
 *
 * @param immatriculation Identifiant du véhicule à retirer
 * @param lv Pointeur vers la liste de véhicules
 *
 * @return Pointeur vers le véhicule retiré, ou NULL si non trouvé
 */
vehicule* removeVehiculeByImmatriculation(int immatriculation, listeVehicules_s *lv) {

	vehicule *v_courant = lv->listeVehicules;
	// printf("\r\nEntering removeVehiculeFromList : immat : %d\r\n", immatriculation);
	if (v_courant != NULL && v_courant->identifiant == immatriculation) {
		lv->listeVehicules = v_courant->suivant;
		lv->nbVehicules -= 1;
		// printf("véhicule %d retiré en tête de liste\r\n", immatriculation);
		return v_courant;
	}
	vehicule *v_prec = v_courant;
	v_courant = v_courant->suivant;

	while (v_courant != NULL && v_courant->identifiant != immatriculation) {
		v_prec = v_courant;
		v_courant = v_courant->suivant;
	}
	if (v_courant != NULL && v_courant->identifiant == immatriculation) {
		v_prec->suivant = v_courant->suivant;
		lv->nbVehicules -= 1;
		// printf("véhicule %d retiré\r\n", immatriculation);
		v_courant->suivant = NULL;
		return v_courant;
	}
	return NULL;
}

/*
 * Retire un véhicule d’une liste selon sa position dans la liste chaînée
 * Met à jour le pointeur de tête et le compteur de véhicules
 * Retourne levéhicule retiré, ou NULL si la position est invalide ou que la liste est vide
 *
 * @param pos Position du véhicule à retirer (1 = tête de liste : pos)
 * @param lv  Pointeur vers la liste de véhicules
 *
 * @return Pointeur vers le véhicule retiré, ou NULL si la position est hors limites
 */
vehicule* removeVehiculeByPositionInList(int pos, listeVehicules_s *lv) {
	if (lv->nbVehicules > 0) {
		vehicule *v_courant = lv->listeVehicules;
		if (pos == 1) {
			lv->listeVehicules = lv->listeVehicules->suivant;
			lv->nbVehicules -= 1;
			v_courant->suivant = NULL;
			return v_courant;
		} else if (pos <= lv->nbVehicules) {
			vehicule *v_prec = v_courant;
			v_courant = v_courant->suivant;
			for (int i = 2; i < pos; i++) {
				v_prec = v_courant;
				v_courant = v_courant->suivant;
			}
			v_prec->suivant = v_courant->suivant;
			lv->nbVehicules -= 1;
			// printf("véhicule %d retiré\r\n", immatriculation);
			v_courant->suivant = NULL;
			return v_courant;
		}
		return NULL;
	} else {
		return NULL;
	}
}

/*
 * Met à jour la position d’un véhicule selon sa direction et sa vitesse
 *
 * Vérifie les limites de l’écran et inverse la direction si le véhicule atteint un bord
 * La position est ajustée horizontalement ou verticalement en fonction de la direction courante
 *
 * @param v Pointeur vers le véhicule à déplacer
 */
void updateCarPosition(vehicule *v) {

	extern int screen_width, screen_height, screen_padding;
	switch (v->direction) {
	case 'N':
		if (v->posy >= screen_padding + 2) {
			v->posy -= v->vitesse;
		} else {
			v->direction = 'S';
			updateCarPosition(v);
		}
		break;
	case 'S':
		if (v->posy <= screen_height - screen_padding - 2 - 6) {
			v->posy += v->vitesse;
		} else {
			v->direction = 'N';
			updateCarPosition(v);
		}
		break;
	case 'W':
		if (v->posx >= screen_padding + 4) {
			v->posx -= 2 * v->vitesse;
		} else {
			v->direction = 'E';
			updateCarPosition(v);
		}
		break;
	case 'E':
		if (v->posx <= screen_width - screen_padding - 4 - 6) {
			v->posx += 2 * v->vitesse;
		} else {
			v->direction = 'W';
			// setCarrosserie(v);
			updateCarPosition(v);
		}
		break;
	}
}

/*
 * Met à jour la position d’un véhicule dans la simulation de parking en fonction de son statut
 *
 * Déplace le véhicule selon sa direction et l’état de la grille du parking (emplacements :occupés ou libres)
 * Gère trois statuts principaux :
 *   Véhicule entrant : recherche et occupation d’une place libre
 *   Véhicule garé : maintien ou ajustement léger de position
 *   Véhicule sortant : progression vers la sortie et calcul du prix
 *
 * La fonction modifie l’occupation des emplacements dans la structure `parking` et met à jour la direction et la position du véhicule
 *
 * @param v Pointeur vers le véhicule à déplacer
 * @param p  Pointeur vers la structure parking
 * @param statutVehicule Statut actuel du véhicule (1 = entrant, 2 = garé, 3 = sortant)
 *
 * @return Entier indiquant le nouvel état (new statut) ou action effectuée pour le véhicule
 */
int updateCarPositionSimulation(vehicule *v, parking *p, int statutVehicule) {
	extern int place_largeur, place_hauteur, nb_rangee,
			nombre_emplacement_longueur;
	int numLigne = getVehiculeNumLigne(v->posy, v->type, v->direction);
	int numColonne = getVehiculeNumColonne(v->posx, v->type, v->direction);
	int nbLignes = 4*(1+p->nombre_rangee);
	int nbColonnes = p->nombre_emplacement_longueur;
	switch (statutVehicule) { // 1 : entrant, 2 stationne, 3 sortant
	case 1: // véhicule entrant et cherchant à se garer
		if (numColonne == 0 && numLigne == nbLignes - 2) { // véhicule entrant se trouvant devant la sortie
			if (p->empl[numLigne][numColonne + 1].occupation == 0) {
				v->direction = 'E';
				v->posx += place_largeur + 1;
				p->empl[numLigne][numColonne].occupation = 0;
				p->empl[numLigne][numColonne + 1].occupation = 1;
			}
			return 1;
		}

		if (v->specificite != 's' && numColonne == nbColonnes-1  && numLigne == nbLignes - 3) { // véhicule électrique ou handicapé entrant
			if (p->empl[numLigne][numColonne - 1].occupation == 0) {
				v->direction = 'O';
				v->posx -= (place_largeur + 1);
				p->empl[numLigne][numColonne].occupation = 0;
				p->empl[numLigne][numColonne -1].occupation = 1;
			}
			return 1;
		}
		if (p->empl[numLigne][numColonne].dirN == 1 // Place de parking libre au N
		&& p->empl[numLigne - 1][numColonne].type == 'p'
				&& p->empl[numLigne - 1][numColonne].occupation == 0
				&& v->specificite == p->empl[numLigne - 1][numColonne].specificite) {
			v->posy -= place_hauteur;
			v->direction = 'N';
			p->empl[numLigne - 1][numColonne].occupation = 1;
			p->empl[numLigne][numColonne].occupation = 0;
			v->etat = 0;
			return 2;
		}
		if (numLigne < nbLignes-1 &&  p->empl[numLigne][numColonne].dirS == 1 // Place de parking libre au S
		&& p->empl[numLigne + 1][numColonne].type == 'p'
				&& p->empl[numLigne + 1][numColonne].occupation == 0
				&& v->specificite == p->empl[numLigne + 1][numColonne].specificite) {
			v->posy += place_hauteur;
			v->direction = 'S';
			p->empl[numLigne + 1][numColonne].occupation = 1;
			p->empl[numLigne][numColonne].occupation = 0;
			v->etat = 0;
			return 2;
		}
		if (v->direction == 'O' && p->empl[numLigne + 2][numColonne].type == 'p'
				&& p->empl[numLigne + 1][numColonne].occupation == 0
				&& p->empl[numLigne + 2][numColonne].occupation == 0
				&& v->specificite == p->empl[numLigne + 2][numColonne].specificite) {
			v->direction = 'S';
			v->posy += place_hauteur;
			p->empl[numLigne][numColonne].occupation = 0;
			p->empl[numLigne + 1][numColonne].occupation = 1;
			return 1;
		}
		if (v->direction == 'O' && numColonne > 2
				&& p->empl[numLigne][numColonne - 1].occupation == 0) {
			v->posx -= (place_largeur + 1);
			p->empl[numLigne][numColonne].occupation = 0;
			p->empl[numLigne][numColonne - 1].occupation = 1;
			return 1;
		}
		if (v->direction == 'E' && numColonne < nombre_emplacement_longueur - 2
				&& p->empl[numLigne][numColonne + 1].occupation == 0) {
			v->posx += (place_largeur + 1);
			p->empl[numLigne][numColonne].occupation = 0;
			p->empl[numLigne][numColonne + 1].occupation = 1;
			return 1;
		}

		int cpt = 0;
		do {
			switch (rand() % 4) {
			case 0:
				if (numColonne > 0 && p->empl[numLigne][numColonne].dirO == 1
						&& p->empl[numLigne][numColonne - 1].occupation == 0) {
					v->posx -= (place_largeur + 1);
					v->direction = 'O';
					p->empl[numLigne][numColonne - 1].occupation = 1;
					p->empl[numLigne][numColonne].occupation = 0;
					return 1;
				}
				break;
			case 1:
				if (numColonne < nombre_emplacement_longueur - 1
						&& p->empl[numLigne][numColonne].dirE == 1
						&& p->empl[numLigne][numColonne + 1].occupation == 0) {
					v->posx += place_largeur + 1;
					v->direction = 'E';
					p->empl[numLigne][numColonne + 1].occupation = 1;
					p->empl[numLigne][numColonne].occupation = 0;
					return 1;
				}
				break;
			case 2:
				if (numLigne > 0 && p->empl[numLigne][numColonne].dirN == 1
						&& p->empl[numLigne - 1][numColonne].occupation == 0) {
					v->posy -= place_hauteur;
					v->direction = 'N';
					p->empl[numLigne - 1][numColonne].occupation = 1;
					p->empl[numLigne][numColonne].occupation = 0;
					return 1;
				}
				break;
			case 3:
				if (numLigne < (4 * (1 + nb_rangee)) - 1
						&& p->empl[numLigne][numColonne].dirS == 1
						&& p->empl[numLigne + 1][numColonne].occupation == 0) {
					v->posy += place_hauteur;
					v->direction = 'S';
					p->empl[numLigne + 1][numColonne].occupation = 1;
					p->empl[numLigne][numColonne].occupation = 0;
					return 1;
				}

				break;
			}
			cpt++;
		} while (cpt < 10);
		return 1;
		break;
	case 2: // vehicule garé
		if (v->etat == 0) {
			return 2;
		} else {
			if (p->empl[numLigne][numColonne].dirN == 1
					&& p->empl[numLigne - 1][numColonne].occupation == 0) {
				v->direction = 'N';
				v->posy -= place_hauteur;
				p->empl[numLigne][numColonne].occupation = 0;
				p->empl[numLigne - 1][numColonne].occupation = 1;
				return 3;
			}
			if (p->empl[numLigne][numColonne].dirS == 1
					&& p->empl[numLigne + 1][numColonne].occupation == 0) {
				v->direction = 'S';
				v->posy += place_hauteur;
				p->empl[numLigne][numColonne].occupation = 0;
				p->empl[numLigne + 1][numColonne].occupation = 1;
				return 3;
			}
		}
		return 2;
		break;
	case 3:

		if (numColonne == 0 && numLigne == nbLignes - 1) {
			v->instantSortie = time(NULL);
			v->prix_parking = prixParking(v, p->taux_horaire);
			p->prix_a_payer = v->prix_parking;
			return 4;
		}
		if (numColonne == 0 && p->empl[numLigne + 1][0].occupation == 0) {
			v->posy += place_hauteur;
			v->direction = 'S';
			p->empl[numLigne][0].occupation = 0;
			p->empl[numLigne + 1][0].occupation = 1;
		} else if (v->direction == 'S') {
			if (p->empl[numLigne][numColonne].dirO == 1
					&& p->empl[numLigne][numColonne - 1].occupation == 0) {
				v->posx -= (place_largeur + 1);
				v->direction = 'O';
				p->empl[numLigne][numColonne].occupation = 0;
				p->empl[numLigne][numColonne - 1].occupation = 1;
			} else if ( numLigne < nbLignes -1 && p->empl[numLigne + 1][numColonne].occupation == 0) {
				v->posy += place_hauteur;
				p->empl[numLigne][numColonne].occupation = 0;
				p->empl[numLigne + 1][numColonne].occupation = 1;
			} else if (p->empl[numLigne][numColonne].dirE == 1
					&& p->empl[numLigne][numColonne + 1].occupation == 0) {
				v->posx += (place_largeur + 1);
				v->direction = 'E';
				p->empl[numLigne][numColonne].occupation = 0;
				p->empl[numLigne][numColonne + 1].occupation = 1;
			}
		} else if (v->direction == 'N') {
			if (p->empl[numLigne][numColonne].dirO == 1
					&& p->empl[numLigne][numColonne - 1].occupation == 0) {
				v->posx -= (place_largeur + 1);
				v->direction = 'O';
				p->empl[numLigne][numColonne].occupation = 0;
				p->empl[numLigne][numColonne - 1].occupation = 1;
			} else if (p->empl[numLigne - 1][numColonne].occupation == 0) {
				v->posy -= place_hauteur;
				p->empl[numLigne][numColonne].occupation = 0;
				p->empl[numLigne - 1][numColonne].occupation = 1;
			}

		} else if (v->direction == 'O'
				&& p->empl[numLigne][numColonne - 1].occupation == 0) {
			v->posx -= (place_largeur + 1);
			p->empl[numLigne][numColonne].occupation = 0;
			p->empl[numLigne][numColonne - 1].occupation = 1;
		} else if (v->direction == 'E') {
			if (p->empl[numLigne - 1][numColonne].occupation == 0) {
				v->posy -= place_hauteur;
				v->direction = 'O';
				p->empl[numLigne][numColonne].occupation = 0;
				p->empl[numLigne - 1][numColonne].occupation = 1;
			}
		}
		return 3;
		break;
	}
	return 1;
}

/*
 * Met à jour la position de tous les véhicules d’une liste chaînée
 *
 * Parcourt la liste des véhicules et applique `updateCarPosition` à chacun
 * en ajustant leur position et direction selon les limites de l’écran
 *
 * @param lv Pointeur vers la liste de véhicules à mettre à jour
 */
void updateListeVehiculesPosition(listeVehicules_s *lv) {
	vehicule *v_courant = lv->listeVehicules;
	while (v_courant != NULL) {
		updateCarPosition(v_courant);
		v_courant = v_courant->suivant;
	}
}

/*
 * Met à jour les positions de tous les véhicules dans plusieurs listes de simulation.
 *
 * Pour chaque liste de véhicules, applique `updateCarPositionSimulation` à chaque véhicule
 *  Si le statut d’un véhicule change (entrée, stationnement, sortie),
 * il est retiré de la liste actuelle et ajouté à la liste correspondante
 *
 * @param lv Tableau de pointeurs vers les listes de véhicules
 * @param nbListes Nombre total de listes à mettre à jour
 * @param p Pointeur vers la structure parking pour vérifier l’occupation
 */
void updateListesVehiculesPositionSimulation(listeVehicules_s **lv,
		int nbListes, parking *p) {
	extern int nb_rangee;
	for (int i = 0; i < nbListes; i++) {
		// printf("\r\nDébut MAJ position liste %d\r\n", i);
		vehicule *v_courant = lv[i]->listeVehicules;
		int typeMouvement, immat;
		while (v_courant != NULL) {
			// printf("Maj véhicule %d\r\n", v_courant->identifiant);
			typeMouvement = updateCarPositionSimulation(v_courant, p, lv[i]->type);
			immat = v_courant->identifiant;
			v_courant = v_courant->suivant;
			if (typeMouvement != lv[i]->type) {
				vehicule *v = removeVehiculeByImmatriculation(immat, lv[i]);
				if (typeMouvement == 4) {
					p->empl[(4 * (1 + nb_rangee)) - 1][0].occupation = 0;
					//free(v);
					// } else {
				}
				addVehiculeHeadListe(v, lv[i + 1]);
			}

		}
		printf("Fin MAJ position liste %d\r\n", i);
	}
}

int getVehiculePosX(int numCol, char type, char direction) {
	extern int screen_padding, place_largeur;
	return screen_padding + (place_largeur / 2) + numCol * (place_largeur + 1);
}
int getVehiculePosY(int numLigne, char type, char direction) {
	extern int screen_padding, place_hauteur;
	return (screen_padding / 2) + numLigne * place_hauteur;
}

int getVehiculeNumLigne(int posY, char type, char direction) {
	extern int screen_padding, place_hauteur;
	return (posY - (screen_padding / 2)) / place_hauteur;

}
int getVehiculeNumColonne(int posX, char type, char direction) {
	extern int screen_padding, place_largeur;
	return (posX - screen_padding - place_largeur / 2) / (place_largeur + 1);
}

/*
 * Initialise les emplacements d’un parking en configurant leur type, occupation et directions
 *
 * Crée dynamiquement la grille des emplacements et définit :
 *   - Les places de parking (normales, électriques, handicap)
 *   - Les voies de circulation horizontales et verticales
 *   - Les directions autorisées pour chaque emplacement
 *   - L’entrée et la sortie du parking
 *
 * @param p Pointeur vers la structure parking à initialiser
 */
void initiateEmplacements(parking *p) {
	int nbLignes = 4 * (1 + p->nombre_rangee);
	int nbColonnes = p->nombre_emplacement_longueur;
	p->empl = malloc(nbLignes * sizeof(emplacement_s*));
	for (int li = 0; li < nbLignes; li++) {
		p->empl[li] = malloc(nbColonnes * sizeof(emplacement_s));
		for (int col = 0; col < nbColonnes; col++) {
			p->empl[li][col].type = 'r';
			p->empl[li][col].occupation = 0;
			p->empl[li][col].dirN = 1;
			p->empl[li][col].dirE = 1;
			p->empl[li][col].dirO = 1;
			p->empl[li][col].dirS = 1;
			p->empl[li][col].dirStationnement = 'N';
			p->empl[li][col].specificite = 's';

		}
	}
	// rangée de places en haut et en bas du parking
	for (int col = 0; col < nbColonnes; col++) {
		p->empl[0][col].type = 'p';
		p->empl[0][col].dirN = 0;
		p->empl[0][col].dirE = 0;
		p->empl[0][col].dirO = 0;
		p->empl[0][col].dirS = 1;
	}
	for (int col = 1; col <= (nbColonnes - 1) / 3; col++) {
		p->empl[nbLignes - 1][col].type = 'p';
		p->empl[nbLignes - 1][col].dirN = 1;
		p->empl[nbLignes - 1][col].dirE = 0;
		p->empl[nbLignes - 1][col].dirO = 0;
		p->empl[nbLignes - 1][col].dirS = 0;
		p->empl[nbLignes - 1][col].dirStationnement = 'S';
		p->empl[nbLignes - 1][col].specificite = 'h';
	}

	for (int col =1+ (nbColonnes - 1) / 3; col < nbColonnes - 1; col++) {
		p->empl[nbLignes - 1][col].type = 'p';
		p->empl[nbLignes - 1][col].dirN = 1;
		p->empl[nbLignes - 1][col].dirE = 0;
		p->empl[nbLignes - 1][col].dirO = 0;
		p->empl[nbLignes - 1][col].dirS = 0;
		p->empl[nbLignes - 1][col].dirStationnement = 'S';
		p->empl[nbLignes - 1][col].specificite = 'e';
	}

	// rangées de voies de roulement en haut du parking
	for (int col = 0; col < nbColonnes; col++) {
		p->empl[1][col].dirE = 0;
		p->empl[2][col].dirO = 0;
	}
	// rangées de voies de roulement en bas du parking
	for (int col = 1; col < nbColonnes - 1; col++) {
		p->empl[nbLignes - 3][col].dirE = 0;
		p->empl[nbLignes - 2][col].dirO = 0;

	}

	for (int r = 0; r < p->nombre_rangee; r++) {
		for (int col = 2; col < nbColonnes - 2; col++) {
			p->empl[3 + 4 * r][col].type = 'p';
			p->empl[3 + 4 * r][col].dirN = 1;
			p->empl[3 + 4 * r][col].dirE = 0;
			p->empl[3 + 4 * r][col].dirO = 0;
			p->empl[3 + 4 * r][col].dirS = 0;
			p->empl[3 + 4 * r][col].dirStationnement = 'S';

			p->empl[4 + 4 * r][col].type = 'p';
			p->empl[4 + 4 * r][col].dirN = 0;
			p->empl[4 + 4 * r][col].dirE = 0;
			p->empl[4 + 4 * r][col].dirO = 0;
			p->empl[4 + 4 * r][col].dirS = 1;

			p->empl[5 + 4 * r][col].dirE = 0;
			p->empl[6 + 4 * r][col].dirO = 0;

		}
	}
	// rangées de voies de roulement à gauche et à droite du parking
	for (int r = 2; r < nbLignes - 1; r++) {
		p->empl[r][0].dirO = 0;
		p->empl[r][0].dirE = 0;
		p->empl[r][0].dirN = 0;
		p->empl[r][1].dirS = 0;
		p->empl[r][1].dirO = 0;
		p->empl[r][1].dirE = 0;
		p->empl[r][nbColonnes - 1].dirE = 0;
		p->empl[r][nbColonnes - 1].dirO = 0;
		p->empl[r][nbColonnes - 1].dirS = 0;
		p->empl[r][nbColonnes - 2].dirE = 0;
		p->empl[r][nbColonnes - 2].dirO = 0;
		p->empl[r][nbColonnes - 2].dirN = 0;
	}
	p->empl[2][nbColonnes - 1].dirO = 1;
	for (int r = 0; r < p->nombre_rangee; r++) {
		p->empl[5 + 4 * r][1].dirO = 1;
		p->empl[6 + 4 * r][0].dirE = 1;
		p->empl[6 + 4 * r][1].dirE = 1;
		p->empl[5 + 4 * r][nbColonnes - 1].dirO = 1;
		p->empl[5 + 4 * r][nbColonnes - 2].dirO = 1;
		p->empl[6 + 4 * r][nbColonnes - 2].dirE = 1;
	}
	// entree et sortie
	p->empl[nbLignes - 1][0].dirE = 0;
	p->empl[nbLignes - 1][0].dirO = 0;
	p->empl[nbLignes - 1][0].dirN = 0;
	p->empl[nbLignes - 1][0].dirS = 1;
	p->empl[nbLignes - 1][nbColonnes - 1].dirE = 0;
	p->empl[nbLignes - 1][nbColonnes - 1].dirO = 0;
	p->empl[nbLignes - 1][nbColonnes - 1].dirN = 1;
	p->empl[nbLignes - 1][nbColonnes - 1].dirS = 0;
	p->empl[nbLignes - 2][1].dirS = 1;

}

void updateStatutVehicules(listeVehicules_s *lvEntrants,
		listeVehicules_s *lvStationnants, listeVehicules_s *lvSortants) {

}

/*
 * Place un véhicule aléatoirement dans un emplacement libre correspondant à sa spécificité.
 *
 * Cherche un emplacement libre compatible avec le type du véhicule et le marque comme occupé.
 * Initialise la position et la direction du véhicule avant de l’ajouter à la liste des véhicules garés.
 *
 * @param v  Pointeur vers le véhicule à stationner
 * @param p  Pointeur vers la structure parking
 * @param lv Pointeur vers la liste des véhicules garés
 * @return 1 si un emplacement a été trouvé et le véhicule placé, 0 sinon
 */
int addVehiculeStationnant(vehicule *v, parking *p, listeVehicules_s *lv) {
	int nbLignes = 4 * (1 + p->nombre_rangee);
	int nbColonnes = p->nombre_emplacement_longueur;
	int nbPlacesParking = (2 * p->nombre_emplacement_longueur) - 2
			+ 2 * p->nombre_rangee * (p->nombre_emplacement_longueur - 4);
	int randLigne, randColonne, compteur;
	if (lv[1].nbVehicules == nbPlacesParking) {
		return 0;
	}
	compteur = 0;
	do {
		randLigne = rand() % nbLignes;
		randColonne = rand() % nbColonnes;
		compteur +=1;
	} while (compteur < 100 && !(p->empl[randLigne][randColonne].type == 'p'
			&& p->empl[randLigne][randColonne].occupation == 0
			&& v->specificite == p->empl[randLigne][randColonne].specificite));
	if (compteur < 100){
	// printf("\n\rEmplacement libre trouvé en %dX%d\n",randLigne, randColonne);
	v->direction = p->empl[randLigne][randColonne].dirStationnement;
	v->posx = getVehiculePosX(randColonne, v->type, v->direction);
	v->posy = getVehiculePosY(randLigne, v->type, v->direction);
	v->etat = 0;
	p->empl[randLigne][randColonne].occupation = 1;
	addVehiculeHeadListe(v, lv);
	return 1;
	}
	return 0;
}

/* @note : idée supplementaire
 * Ouvre un terminal externe pour afficher les logs en temps réel.
 *
 * Utilise `xterm` et redirige la sortie du flux `log_file` vers ce terminal.
 * En cas d’échec, affiche un message d’erreur et termine le programme.
 */
void open_log_terminal() {
	// Ouvrir un terminal externe (xterm) pour afficher les logs
	extern FILE *log_file;
	log_file = popen("xterm -hold -e 'cat -'", "w");
	if (log_file == NULL) {
		perror("Erreur lors de l'ouverture du terminal de logs");
		exit(EXIT_FAILURE);
	}
}
void write_log(const char *format, ...) {
	extern FILE *log_file;
	if (log_file == NULL) {
		return;
	}
	va_list args;
	va_start(args, format);

	// Écrire le log formaté dans le terminal externe
	vfprintf(log_file, format, args);
	fflush(log_file);  // Forcer l'écriture

	va_end(args);
}

/*
 * Calcule le prix du stationnement d’un véhicule en fonction du temps écoulé.
 *
 * Les 30 premières minutes sont gratuites. Au-delà, chaque quart d’heure commencé
 * est facturé 25 centimes. Le calcul utilise l’heure d’arrivée et de sortie.
 *
 * @param v Pointeur vers le véhicule
 * @param tauxHoraire Tarif horaire en centimes (non utilisé dans cette version, fixe à 25 centimes/15 min)
 *
 * @return Prix total en centimes d’euros
 */
int prixParking(vehicule *v, int tauxHoraire) {
    double tempsTotalSec = v->instantSortie - v->instantArrivee;
    double tempsTotalMin = tempsTotalSec / 60.0;  // Conversion en minutes

    if (tempsTotalMin <= 30.0) {
        return 0;
    }

    double tempsPayant = tempsTotalMin - 30.0;
    int quartsHeure = (int)(tempsPayant / 15.0);

    if ((int)tempsPayant % 15 != 0) {
        quartsHeure++;
    }
    int prix = quartsHeure * 25;

    return prix; // prix en centimes d’euros
}

