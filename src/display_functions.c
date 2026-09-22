/*
 * display_functions.c
 *
 *	Module d'affichage pour la simulation de parking
 *
 *  Created on: 9 sept. 2025
 *      Author: constance
 */

#define _XOPEN_SOURCE_EXTENDED 1
#define COLOR_GRAY 8

#include <fcntl.h>
#include <ncursesw/curses.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "display_functions.h"
#include "structures.h"

/*
 * Détecte une pression de touche sans bloquer l'exécution
 *
 * Cette fonction configure le terminal en mode non-canonique et non-bloquant
 * pour détecter si une touche a été pressée, puis restaure la configuration
 * d'origine du terminal.
 *
 * @return Le caractère pressé, ou 0 si aucune touche n'a été détectée
 *
 * @note Utilise les fonctions POSIX de manipulation du terminal (termios)
 * @note Le terminal est temporairement configuré en mode non-echo
 *
 * Ce choix à été fait car sur un mac le projet avait tendance à bloquer
 * l'excution sur le terminal
 */
char key_pressed() {
	struct termios oldterm, newterm;
	int oldfd;
	char c, result = 0;
	tcgetattr(STDIN_FILENO, &oldterm);
	newterm = oldterm;
	newterm.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newterm);
	oldfd = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, oldfd | O_NONBLOCK);
	c = getchar();
	tcsetattr(STDIN_FILENO, TCSANOW, &oldterm);
	fcntl(STDIN_FILENO, F_SETFL, oldfd);
	if (c != EOF) {
		ungetc(c, stdin);
		result = getchar();
	}
	return result;
}

/*
 * Compte le nombre d'occurrences d'un caractère dans une chaîne
 *
 * @param str Chaîne de caractères à analyser
 * @param ch Caractère à compter
 *
 * @return Nombre d'occurrences du caractère dans la chaîne
 */
int countChar(const char *str, char ch) {
	int count = 0;
	const char *ptr = str;
	while ((ptr = strchr(ptr, ch)) != NULL) {
		count++;
		ptr++; // Passer au caractère suivant pour éviter une boucle infinie
	}
	return count;
}

/*
 * Affiche l'interface principale avec bordure dans un buffer ncurses
 *
 * Version buffer de displayInterface, dessine le cadre dans une fenêtre
 * ncurses spécifique plutôt que sur l'écran standard.
 *
 * @param win Pointeur vers la fenêtre ncurses de destination
 * @param w Largeur de la fenêtre (paramètre non utilisé, dimensions calculées)
 * @param h Hauteur de la fenêtre (paramètre non utilisé, dimensions calculées)
 *
 * @note Utilise les variables globales screen_width, screen_height, screen_padding
 */
void displayInterfaceBuffer(WINDOW *win, int w, int h) {
	extern int screen_width, screen_height, screen_padding;

	// Définir l'arrière-plan avec la paire de couleurs 1 (texte blanc sur fond gris)
	wbkgd(win, COLOR_PAIR(1));
	wclear(win); // Effacer l'écran pour appliquer l'arrière-plan
	wattroff(win, COLOR_PAIR(1));
	// Activer la couleur pour la bordure
	wattron(win, COLOR_PAIR(6));

	int start_x = screen_padding, start_y = screen_padding / 2, width =
			screen_width - 2 * screen_padding + 3, height = screen_height
			- screen_padding + 3;
	// Dessiner la bordure supérieure
	mvwaddwstr(win, start_y, start_x, L"\u2554");
	for (int x = 1; x < width - 1; x++) {
		// mvprintw(ft->start_y, ft->start_x + x, "-");
		mvwaddwstr(win, start_y, start_x + x, L"\u2550");
	}
	mvwaddwstr(win, start_y, start_x + width - 1, L"\u2557");

	// Dessiner la bordure inférieure
	mvwaddwstr(win, start_y + height - 1, start_x, L"\u255A");
	for (int x = 1; x < width - 1; x++) {
		mvwaddwstr(win, start_y + height - 1, start_x + x, L"\u2550");
	}
	mvwaddwstr(win, start_y + height - 1, start_x + width - 1, L"\u255D");

	// Dessiner les bordures latérales
	for (int y = 1; y < height - 1; y++) {
		mvwaddwstr(win, start_y + y, start_x, L"\u2551");
		mvwaddwstr(win, start_y + y, start_x + width - 1, L"\u2551");
	}
	wattroff(win, COLOR_PAIR(1));
}


/*
 * Affiche un texte encadré sur l'écran standard
 *
 * Crée un cadre décoratif autour d'un texte avec couleurs personnalisées
 * pour le cadre et le contenu. Gère les textes multi-lignes.
 *
 * @param ft Pointeur vers la structure framedText contenant les paramètres d'affichage
 *
 * @note La hauteur est calculée automatiquement selon le nombre de lignes
 * @note Le texte peut contenir des retours à la ligne (\n)
 */
void displayFramedText(framedText *ft) {
	// ft->width = 2 + strlen(ft->text);

	ft->height = 3 + countChar(ft->text, '\n');

	// Activer la couleur pour la bordure
	attron(COLOR_PAIR(ft->pairColorFrame));
	// Dessiner la bordure supérieure
	mvaddwstr(ft->start_y, ft->start_x, L"\u2554");
	for (int x = 1; x < ft->width - 1; x++) {
		// mvprintw(ft->start_y, ft->start_x + x, "-");
		mvaddwstr(ft->start_y, ft->start_x + x, L"\u2550");
	}
	mvaddwstr(ft->start_y, ft->start_x + ft->width - 1, L"\u2557");

	// Dessiner la bordure inférieure
	mvaddwstr(ft->start_y + ft->height - 1, ft->start_x, L"\u255A");
	for (int x = 1; x < ft->width - 1; x++) {
		mvaddwstr(ft->start_y + ft->height - 1, ft->start_x + x, L"\u2550");
	}
	mvaddwstr(ft->start_y + ft->height - 1, ft->start_x + ft->width - 1,
			L"\u255D");

	// Dessiner les bordures latérales
	for (int y = 1; y < ft->height - 1; y++) {
		mvaddwstr(ft->start_y + y, ft->start_x, L"\u2551");
		mvaddwstr(ft->start_y + y, ft->start_x + ft->width - 1, L"\u2551");
	}
	// Désactiver la couleur
	attroff(COLOR_PAIR(ft->pairColorFrame));
	// Activer la couleur pour le texte
	attron(COLOR_PAIR(ft->pairColorText));

	const char delimiter[] = "\n";
	char text[1000];
	strcpy(text, ft->text);
	// Premier appel à strtok
	char *token = strtok(text, delimiter);

	// Parcourir les autres tokens
	int dy = 0;
	while (token != NULL) {
		// Afficher le texte à l'intérieur de la bordure
		mvprintw(ft->start_y + 1 + dy, ft->start_x + 1, "%s", token);
		for (int dx = strlen(token) + 1; dx < ft->width - 1; dx++) {
			mvprintw(ft->start_y + 1 + (dy), ft->start_x + dx, " ");
		}
		dy++;
		token = strtok(NULL, delimiter);
	}

	// Désactiver la couleur
	attroff(COLOR_PAIR(ft->pairColorText));

	// Rafraîchir l'écran
	refresh();
}

/*
 * Affiche un texte encadré dans un buffer pour l'écran de sortie
 *
 * Version buffer de displayFramedText, destinée à l'affichage des informations
 * de sortie du parking dans une fenêtre spécifique.
 *
 * @param win Pointeur vers la fenêtre ncurses de destination
 * @param ft Pointeur vers la structure framedText contenant les paramètres
 */
void displayFramedTextBufferSortie(WINDOW *win, framedText *ft) {

	ft->height = 3 + countChar(ft->text, '\n');

	// Activer la couleur pour la bordure
	wattron(win, COLOR_PAIR(ft->pairColorFrame));
	// Dessiner la bordure supérieure
	mvwaddwstr(win, ft->start_y, ft->start_x, L"\u2554");
	for (int x = 1; x < ft->width - 1; x++) {
		// mvprintw(ft->start_y, ft->start_x + x, "-");
		mvwaddwstr(win, ft->start_y, ft->start_x + x, L"\u2550");
	}
	mvwaddwstr(win, ft->start_y, ft->start_x + ft->width - 1, L"\u2557");

	// Dessiner la bordure inférieure
	mvwaddwstr(win, ft->start_y + ft->height - 1, ft->start_x, L"\u255A");
	for (int x = 1; x < ft->width - 1; x++) {
		mvwaddwstr(win, ft->start_y + ft->height - 1, ft->start_x + x,
				L"\u2550");
	}
	mvwaddwstr(win, ft->start_y + ft->height - 1, ft->start_x + ft->width - 1,
			L"\u255D");

	// Dessiner les bordures latérales
	for (int y = 1; y < ft->height - 1; y++) {
		mvwaddwstr(win, ft->start_y + y, ft->start_x, L"\u2551");
		mvwaddwstr(win, ft->start_y + y, ft->start_x + ft->width - 1,
				L"\u2551");
	}
	// Désactiver la couleur
	wattroff(win, COLOR_PAIR(ft->pairColorFrame));
	// Activer la couleur pour le texte
	wattron(win, COLOR_PAIR(ft->pairColorText));

	const char delimiter[] = "\n";
	char text[1000];
	strcpy(text, ft->text);
	// Premier appel à strtok
	char *token = strtok(text, delimiter);

	// Parcourir les autres tokens
	int dy = 0;
	while (token != NULL) {
		// Afficher le texte à l'intérieur de la bordure
		mvwprintw(win, ft->start_y + 1 + dy, ft->start_x + 1, "%s", token);
		for (int dx = strlen(token) + 1; dx < ft->width - 1; dx++) {
			mvwprintw(win, ft->start_y + 1 + (dy), ft->start_x + dx, " ");
		}
		dy++;
		token = strtok(NULL, delimiter);
	}

	// Désactiver la couleur
	wattroff(win, COLOR_PAIR(ft->pairColorText));

	// Rafraîchir l'écran
	// refresh();
}

/*
 * Affiche un texte encadré avec le prix total à payer dans un buffer
 *
 * Fonction spécialisée pour l'affichage du montant à régler lors de la sortie
 * d'un véhicule. Formate automatiquement le prix en euros et centimes.
 *
 * @param win Pointeur vers la fenêtre ncurses de destination
 * @param ft Pointeur vers la structure framedText (modifiée avec le prix)
 * @param prixTotal Prix en centimes d'euro (ex: 1550 pour 15,50€)
 *
 * @note Le prix est affiché au format "XX€YY" où XX sont les euros et YY les centimes
 */
void displayFramedTextBufferPrix(WINDOW *win, framedText *ft, int prixTotal) {
	int prixEuro, prixCent;
	prixEuro = prixTotal / 100;
	prixCent = prixTotal % 100;
	char chaine[20];
	snprintf(chaine, sizeof(chaine), "%d", prixEuro);

	// ft->width = 2 + strlen(ft->text);
	strcpy(ft->text, "TOTAL à PAYER :\n");
	strcat(ft->text, chaine);
	strcat(ft->text, "€");

	snprintf(chaine, sizeof(chaine), "%02d", prixCent);
	strcat(ft->text, chaine);

	ft->height = 3 + countChar(ft->text, '\n');

	// Activer la couleur pour la bordure
	wattron(win, COLOR_PAIR(ft->pairColorFrame));
	// Dessiner la bordure supérieure
	mvwaddwstr(win, ft->start_y, ft->start_x, L"\u2554");
	for (int x = 1; x < ft->width - 1; x++) {
		// mvprintw(ft->start_y, ft->start_x + x, "-");
		mvwaddwstr(win, ft->start_y, ft->start_x + x, L"\u2550");
	}
	mvwaddwstr(win, ft->start_y, ft->start_x + ft->width - 1, L"\u2557");

	// Dessiner la bordure inférieure
	mvwaddwstr(win, ft->start_y + ft->height - 1, ft->start_x, L"\u255A");
	for (int x = 1; x < ft->width - 1; x++) {
		mvwaddwstr(win, ft->start_y + ft->height - 1, ft->start_x + x,
				L"\u2550");
	}
	mvwaddwstr(win, ft->start_y + ft->height - 1, ft->start_x + ft->width - 1,
			L"\u255D");

	// Dessiner les bordures latérales
	for (int y = 1; y < ft->height - 1; y++) {
		mvwaddwstr(win, ft->start_y + y, ft->start_x, L"\u2551");
		mvwaddwstr(win, ft->start_y + y, ft->start_x + ft->width - 1,
				L"\u2551");
	}
	// Désactiver la couleur
	wattroff(win, COLOR_PAIR(ft->pairColorFrame));
	// Activer la couleur pour le texte
	wattron(win, COLOR_PAIR(ft->pairColorText));

	const char delimiter[] = "\n";
	char text[1000];
	strcpy(text, ft->text);
	// Premier appel à strtok
	char *token = strtok(text, delimiter);

	// Parcourir les autres tokens
	int dy = 0;
	while (token != NULL) {
		// Afficher le texte à l'intérieur de la bordure
		mvwprintw(win, ft->start_y + 1 + dy, ft->start_x + 1, "%s", token);
		for (int dx = strlen(token) + 1; dx < ft->width - 1; dx++) {
			mvwprintw(win, ft->start_y + 1 + (dy), ft->start_x + dx, " ");
		}
		dy++;
		token = strtok(NULL, delimiter);
	}

	// Désactiver la couleur
	wattroff(win, COLOR_PAIR(ft->pairColorText));

	// Rafraîchir l'écran
	// refresh();
}

/*
 * Gestionnaire de signal pour le redimensionnement de fenêtre
 *
 * Cette fonction est appelée automatiquement lorsque le terminal est redimensionné.
 * Elle active un drapeau pour signaler au programme principal de redessiner l'interface.
 *
 * @param sig Numéro du signal reçu (généralement SIGWINCH)
 *
 * @note Modifie la variable globale resize_flag
 */
void handle_resize(int sig) {
	extern int resize_flag;
	(void) sig; // éviter un warning
	resize_flag = 1;
}

/*
 * Calcule la longueur d'une chaîne UTF-8 en nombre de caractères
 *
 * Compte le nombre de caractères Unicode dans une chaîne, en tenant compte
 * de l'encodage UTF-8 multi-octets.
 *
 * @param str Chaîne UTF-8 à analyser
 *
 * @return Nombre de caractères Unicode
 *
 * @note Gère les caractères UTF-8 de 1 à 4 octets
 * @note Les séquences UTF-8 invalides sont ignorées
 */
int utf8_strlen(const char *str) {
	int count = 0;
	while (*str) {
		// Détermine le nombre d'octets du caractère UTF-8 actuel
		if ((*str & 0x80) == 0) {
			// Caractère ASCII (1 octet)
			count++;
			str += 1;
		} else if ((*str & 0xE0) == 0xC0) {
			// Caractère UTF-8 sur 2 octets
			count++;
			str += 2;
		} else if ((*str & 0xF0) == 0xE0) {
			// Caractère UTF-8 sur 3 octets
			count++;
			str += 3;
		} else if ((*str & 0xF8) == 0xF0) {
			// Caractère UTF-8 sur 4 octets
			count++;
			str += 4;
		} else {
			// Cas invalide (ne devrait pas arriver pour une chaîne UTF-8 valide)
			str++;
		}
	}
	return count;
}

/*
 * Affiche un véhicule dans un buffer ncurses avec sa carrosserie
 *
 * Dessine la représentation graphique d'un véhicule en utilisant sa carrosserie
 * personnalisée selon sa direction et son type.
 *
 * @param win Pointeur vers la fenêtre ncurses de destination
 * @param car Pointeur vers la structure vehicule à afficher
 *
 * @note La carrosserie est stockée sous forme de chaînes délimitées par '|'
 * @note Les camions ('c') nécessitent un décalage supplémentaire
 */
void displayCarBuffer(WINDOW *win, vehicule *car) {
	// Activer la couleurcarrosserie[i] != '\0'
	wattron(win, COLOR_PAIR(car->code_couleur));
	char *carrosserie;
	switch (car->direction) {
	case 'N':
		carrosserie = car->carrosserie[0];
		break;
	case 'S':
		carrosserie = car->carrosserie[1];
		break;
	case 'O':
		carrosserie = car->carrosserie[2];
		break;
	case 'E':
		carrosserie = car->carrosserie[3];
		break;
	}

	const char *delimiter = "|";
	char buffer[128];
	int dx = 0, dy = 0;
	switch (car->direction) {
	case 'S':
		dy = 0;
		break;
	case 'N':
		dy = 1;
		break;
	case 'O':
	case 'E':
		dy = 2;
		break;
	}
	if (car->type == 'c') {
		dx = -1;
		if (car->direction == 'O' || car->direction == 'E') {
			dy -= 1;
		}
	}
	strcpy(buffer, carrosserie);
	// Premier appel à strtok
	char *token = strtok(buffer, delimiter);
	// Parcourir les autres tokens

	while (token != NULL) {
		mvwprintw(win, car->posy + dy, car->posx + dx, "%s", token);
		dy++;
		token = strtok(NULL, delimiter);
	}
	// Désactiver la couleur
	wattroff(win, COLOR_PAIR(car->code_couleur));
}

/*
* Affiche l'ensemble des emplacements d'un parking dans une fenêtre ncurses.
*
* Parcourt la matrice des emplacements et dessine chaque case en fonction de :
* - son type (place de parking, voie de roulement, élément divers)
* - son état (occupé ou libre)
* - ses directions autorisées (N, S, E, O)
* - ses éventuelles spécificités (borne électrique, place handicapée, etc.)
*
* La fonction utilise les informations de dimensionnement du parking pour
* calculer dynamiquement la position d'affichage et les couleurs attribuées.
*
* @param win Fenêtre ncurses dans laquelle afficher la grille
* @param p Structure `parking` contenant l'ensemble des emplacements
* @param colorPlaceLibre Couleur pour une place de parking libre
* @param colorPlaceOccupee Couleur pour une place de parking occupée
* @param colorRoulementLibre Couleur pour une voie de roulement libre
* @param colorRoulementOccupee Couleur pour une voie de roulement occupée
* @param colorAutre Couleur par défaut pour les autres types
*
* @note Les flèches directionnelles sont affichées en Unicode
* @note Les places spécifiques utilisent une couleur additionnelle
*/
void displayEmplacementsBuffer(WINDOW *win, parking *p, int colorPlaceLibre, int colorPlaceOccupee, int colorRoulementLibre, int colorRoulementOccupee, int colorAutre) {
	extern int screen_padding;
	int nbLignes = 4 * (1 + p->nombre_rangee);
	int nbColonnes = p->nombre_emplacement_longueur;
	int currentColor, specificiteColor;
	int decalage_parking_x, decalage_parking_y;
	wchar_t directionsPossibles[5];

	for (int li = 0; li < nbLignes; li++) {
		for (int col = 0; col < nbColonnes; col++) {
			wcscpy(directionsPossibles, L"\0");
			if (p->empl[li][col].dirO == 1) {
				wcscat(directionsPossibles, L"←");
			}
			if (p->empl[li][col].dirN == 1) {
				wcscat(directionsPossibles, L"↑");
			}
			if (p->empl[li][col].dirS == 1) {
				wcscat(directionsPossibles, L"↓");
			}
			if (p->empl[li][col].dirE == 1) {
				wcscat(directionsPossibles, L"→");
			}
			wcscat(directionsPossibles, L"\0");
			decalage_parking_x = 0;
			decalage_parking_y = 0;
			specificiteColor = 0;
			switch (p->empl[li][col].type) {
			case 'p':
				decalage_parking_x = 2;
				if (p->empl[li][col].dirStationnement == 'N') {
					decalage_parking_y = 2;
				} else {
					decalage_parking_y = -1;
				}
				if (p->empl[li][col].occupation == 0) { // place de parking non occupée
					currentColor = colorPlaceLibre;
				} else {
					currentColor = colorPlaceOccupee;
				}
				switch (p->empl[li][col].specificite) {
				case 'e':
					specificiteColor = 11;
					break;
				case 'h':
					specificiteColor = 5;
					break;
				default:
					specificiteColor = 0;
					break;
				}
				break;
			case 'r':
				if (p->empl[li][col].occupation == 0) { // voie routière non occupée
					currentColor = colorRoulementLibre;
				} else {
					currentColor = colorRoulementOccupee;
				}
				break;
			default:
				currentColor = colorAutre;
				break;
			}
			wattron(win, COLOR_PAIR(currentColor));

			mvwaddwstr(win,
					screen_padding / 2 + p->place_longueur / 2
							+ li * p->place_longueur - decalage_parking_y,
					1+decalage_parking_x + (p->place_largeur / 2) + screen_padding
							+ col * (p->place_largeur + 1), L"▄");
			wattroff(win, COLOR_PAIR(currentColor));
			wattron(win, COLOR_PAIR(4));
			mvwaddwstr(win,
					screen_padding / 2 + p->place_longueur / 2
							+ li * p->place_longueur - 1,
					1 + (p->place_largeur / 2) + screen_padding
							+ col * (p->place_largeur + 1),
					directionsPossibles);
			wattroff(win, COLOR_PAIR(4));

			if (specificiteColor != 0) {
				wattron(win, COLOR_PAIR(specificiteColor));
				mvwaddwstr(win,
						screen_padding / 2 + p->place_longueur / 2
								+ li * p->place_longueur - 3,
						-1 + (p->place_largeur / 2) + screen_padding
								+ col * (p->place_largeur + 1), L"─────");
				wattroff(win, COLOR_PAIR(specificiteColor));
			}

		}
	}
}

/*
* Affiche le plan textuel général du parking dans une fenêtre ncurses.
* En parcourant chaque ligne du plan contenu dans la structure `parking` et
* l'affiche dans la fenêtre ncurses en appliquant la couleur.
* Ce rendu sert notamment pour les éléments statiques du décor (murs, bordures,
* contours…)
*
* @param win Fenêtre ncurses dans laquelle le plan doit être affiché
* @param p Structure `parking` contenant le tableau de lignes du plan
* @param colorPair Identifiant de la paire de couleurs utilisée pour l'affichage
*
* @note Utilise screen_height pour déterminer le nombre de lignes à afficher
* @note L’affichage copie directement chaque chaîne depuis p->plan[i]
*/
void displayParkingBuffer(WINDOW *win, parking *p, int colorPair) {
	extern int screen_height;
	wattron(win, COLOR_PAIR(colorPair));
	for (int i = 0; i < screen_height; i++) {
		mvwaddwstr(win, i, 0, p->plan[i]);
	}
	wattroff(win, COLOR_PAIR(colorPair));
}

/*
* Affichage de l'ensemble des véhicules d'une liste chaînée dans la fenêtre ncurses
*
* Parcourt la liste chaînée des véhicules et appelle pour chacun la fonction
* `displayCarBuffer` chargée d'afficher graphiquement un véhicule individuel.
* Cette fonction centralise ainsi l'affichage de tous les véhicules présents
* dans le parking.
*
* @param win Fenêtre ncurses dans laquelle les véhicules doivent être affichés
* @param lv Structure contenant la liste chaînée des véhicules
*
* @note La liste est parcourue séquentiellement via le champ `suivant`
*/
void displayListeVehiculesBuffer(WINDOW *win, listeVehicules_s *lv) {
	vehicule *v_courant = lv->listeVehicules;
	while (v_courant != NULL) {
		displayCarBuffer(win, v_courant);
		v_courant = v_courant->suivant;
	}
}

/*
* Affiche l'écran d'accueil et le menu principal du simulateur de parking.
* Présentation du projet, et des options de modes disponibles pour l'utilisateur
*
* @note Aucun paramètre n'est requis : la fonction écrit directement dans la
* fenêtre standard ncurses (stdscr)
*/
void initialisationMenu(){

    printw("\n Projet Autonome en Programmation C - 3A \n\n");

    printw("==============================================\n");
    printw("- - - - - - - SIMULATEUR DE PARKING - - - - - \n");
    printw("==============================================\n\n\n");

    printw("----------------------------------------------\n");
    printw("INDEX :\n");
    printw("----------------------------------------------\n");
    printw(" 1)  Mode CHARGÉ entrer 1\n");
    printw(" 2)  Mode FLUIDE entrer 2\n");
    printw("----------------------------------------------\n");

}
