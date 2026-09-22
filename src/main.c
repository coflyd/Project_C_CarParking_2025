#define _XOPEN_SOURCE_EXTENDED 1
#define COLOR_GRAY 8

#define FIFO_PATH "/tmp/simulation_fifo"

#include <locale.h>
#include <ncursesw/curses.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <wchar.h>

#include <sys/stat.h>
#include <fcntl.h>

#include "display_functions.h"
#include "gestion_fichiers.h"
#include "model_functions.h"
#include "control_functions.h"
#include "structures.h"

// Variable globale pour indiquer un redimensionnement
volatile sig_atomic_t resize_flag = 0;
int screen_width = 200, screen_height = 70, screen_padding = 4;
int immatriculation = 0;
const int place_largeur = 7;
const int place_hauteur = 4;
const int nombre_emplacement_longueur = 24;
const int nb_rangee = 2;
const int nbListesVehicules = 4;
int taux_horaire = 250;

// Variable globale pour le flux de logs
FILE *log_file = NULL;

void create_fifo() {
	mkfifo(FIFO_PATH, 0666);
}

int main() {

	srand(time(0));

	char parkingPlanFileName[] = "/Users/constancefleury/Desktop/C_project_car2/src/data/parking.txt";

	parking *parking_1 = setParking(place_largeur, place_hauteur,
			nombre_emplacement_longueur, nb_rangee, 3, parkingPlanFileName,
			taux_horaire);
	initiateEmplacements(parking_1);
	listeVehicules_s *listesVehicule[4];
	listesVehicule[0] = setListeVehiculeVide(1); // entrant
	listesVehicule[1] = setListeVehiculeVide(2); //stationne
	listesVehicule[2] = setListeVehiculeVide(3); // sortant
	listesVehicule[3] = setListeVehiculeVide(4); // sortis
	screen_height = 4 * parking_1->place_longueur
			* (1 + parking_1->nombre_rangee) + 2 * screen_padding;
	screen_width = parking_1->nombre_emplacement_longueur
			* (parking_1->place_largeur + 1) + 2 * screen_padding;
	int nbPlacesParking = (2 * nombre_emplacement_longueur) - 2
			+ 2 * nb_rangee * (nombre_emplacement_longueur - 4);

	// char **car = chargementcarrosserieVehicule("src/data/voiture_1.txt");
	setlocale(LC_ALL, "");

	// Initialisation de ncurses
	initscr();
	cbreak(); // Désactiver la mise en buffer des entrées
	signal(SIGWINCH, handle_resize);
	start_color();

	curs_set(FALSE);
	keypad(stdscr, TRUE); // Activer les touches spéciales
	//initialiser le menu

	initialisationMenu();
	//initialisationParking(parking_1, listesVehicule[1]);
	//choix des modes
	int entering_delay = initialisationParking(parking_1, listesVehicule[1]);

	noecho();

	// Définir les paires de couleurs
	init_pair(0, COLOR_WHITE, COLOR_BLACK);	 // Ligne blanc
	init_pair(1, COLOR_RED, COLOR_BLACK);	 // Ligne rouge
	init_pair(2, COLOR_GREEN, COLOR_BLACK);	 // Ligne verte
	init_pair(3, COLOR_BLUE, COLOR_BLACK);	 // Ligne bleue
	init_pair(4, COLOR_YELLOW, COLOR_BLACK); // Ligne jaune
	init_pair(5, COLOR_CYAN, COLOR_BLACK);	 // Ligne cyan
	init_pair(6, COLOR_GRAY, COLOR_BLACK);	 // Ligne gris
	init_pair(11, COLOR_MAGENTA, COLOR_BLACK);//Lignes rose

	init_pair(7, COLOR_RED, COLOR_GRAY);   // Ligne rouge
	init_pair(8, COLOR_GREEN, COLOR_GRAY); // Ligne verte
	init_pair(9, COLOR_BLUE, COLOR_GRAY);  // Ligne bleue fond gris
	init_pair(10, COLOR_BLUE, COLOR_GRAY); // Ligne bleue fond gris


	// Créer une fenêtre hors écran (buffer)
	WINDOW *buffer = newwin(screen_height, screen_width, 0, 0);

	framedText ft1 =
			{ 14, screen_height - screen_padding, 3, 8, "SORTIE", 5, 5 };
	framedText ft2 =
		    { 22, screen_height - screen_padding, 5, 18, "TOTAL  A PAYER :\n3.75 euros", 4, 5 };
	framedText ft3 =
			{ screen_width - 20, screen_height - screen_padding, 3, 8, "ENTREE", 5, 5 };

	displayParkingBuffer(buffer, parking_1, 3);
	displayEmplacementsBuffer(buffer, parking_1, 2, 1, 3, 4, 6);
	displayFramedTextBufferSortie(buffer, &ft1);
	displayFramedTextBufferSortie(buffer, &ft3);
	displayFramedTextBufferPrix(buffer, &ft2, parking_1->prix_a_payer);

	for (int i = 0; i < nbListesVehicules - 1; i++) {
		displayListeVehiculesBuffer(buffer, listesVehicule[i]);
	}

	// Copier le buffer sur l'écran principal
	copywin(buffer, stdscr, 0, 0, 0, 0, screen_height - 1, screen_width - 1,
	FALSE);
	refresh();

	for (int i = 0; i < nbListesVehicules; i++) {
		afficheListeVehicule(listesVehicule[i]);
	}


	// Variable pour gérer le temps
	clock_t last_clock = clock();
	clock_t begin_clock_entering = clock(), begin_clock_exiting = clock();

	while (1) {
		int ch = key_pressed();
		// Attendre une entrée utilisateur
		if (ch == 'q') {
			break; // Quitter si l'utilisateur appuie sur 'q'
		}
		// Mettre à jour la position de la voiture en fonction du temps
		clock_t current_clock = clock();

		if ((current_clock - last_clock) >=
		CLOCKS_PER_SEC / 1) { // Mettre à jour 1 fois par seconde
			last_clock = current_clock;
			clear();
			// Effacer le buffer
			werase(buffer);

			updateListesVehiculesPositionSimulation(listesVehicule,nbListesVehicules - 1, parking_1);

			if (listesVehicule[1]->nbVehicules + listesVehicule[0]->nbVehicules  < nbPlacesParking) {
				begin_clock_entering = updateEnteringVehiculeList(listesVehicule[0], begin_clock_entering, entering_delay, parking_1);
			}
			begin_clock_exiting = updateExitingVehiculeList(listesVehicule[1],begin_clock_exiting, 10);

			// displayInterfaceBuffer(buffer, screen_width, screen_height);
			displayParkingBuffer(buffer, parking_1, 3);
			displayEmplacementsBuffer(buffer, parking_1, 2, 1, 3, 4, 6);
			displayFramedTextBufferSortie(buffer, &ft1);
			displayFramedTextBufferSortie(buffer, &ft3);
			displayFramedTextBufferPrix(buffer, &ft2, parking_1->prix_a_payer);

			for (int i = 0; i < nbListesVehicules - 1; i++) {
				displayListeVehiculesBuffer(buffer, listesVehicule[i]);
			}
			copywin(buffer, stdscr, 0, 0, 0, 0, screen_height - 1,
					screen_width - 1, FALSE);

			refresh(); // Rafraîchir l'écran

			//idée ouverture d'un second terminal pour voir les datas
			/*for (int i = 0; i < nbListesVehicules; i++) {
				if (i != 1) {
					afficheListeVehicule(listesVehicule[i]);
				}
			}*/
		}
		if (resize_flag) {
			resize_term(0, 0); // Redimensionner ncurses pour s'adapter au terminal
			// nouvelle taille du terminal
			clear();// Effacer l'écran

			// displayInterfaceBuffer(buffer, screen_width, screen_height);
			displayParkingBuffer(buffer, parking_1, 3);
			displayEmplacementsBuffer(buffer, parking_1, 2, 1, 3, 4, 6);
			displayFramedTextBufferSortie(buffer, &ft1);
			displayFramedTextBufferSortie(buffer, &ft3);
			displayFramedTextBufferPrix(buffer, &ft2, parking_1->prix_a_payer);

			for (int i = 0; i < nbListesVehicules - 1; i++) {
				displayListeVehiculesBuffer(buffer, listesVehicule[i]);
			}
			copywin(buffer, stdscr, 0, 0, 0, 0, screen_height - 1,
					screen_width - 1, FALSE);
			refresh();		 // Rafraîchir l'écran

			for (int i = 0; i < nbListesVehicules; i++) {
				afficheListeVehicule(listesVehicule[i]);
			}
			resize_flag = 0; // Réinitialiser le drapeau de redimensionnement
		}
	}

	// Nettoyer et quitter
	delwin(buffer);
	endwin();
	return 0;
}
