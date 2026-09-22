/*
 * control_functions.c
 *
 *  Created on: 6 oct. 2025
 *      Author: constance
 *
 * 	Module de contrôle pour la gestion des flux de véhicules dans le parking (entrées, sorties, initialisation).
 */

#include <stdlib.h>
#include <stdio.h>
#include "structures.h"
#include "model_functions.h"

/*
 * Met à jour la liste des véhicules entrants dans le parking
 *
 * Cette fonction génère périodiquement de nouveaux véhicules souhaitant entrer
 * dans le parking, en fonction d'un délai spécifié. Un véhicule est créé
 * uniquement si l'emplacement d'entrée est libre.
 *
 * @param lv_entering Pointeur vers la liste des véhicules entrants
 * @param begin_clock Horodatage du dernier ajout de véhicule
 * @param delay_sec Délai en secondes entre deux tentatives d'ajout
 * @param p Pointeur vers la structure du parking
 *
 * @return Nouvel horodatage si un véhicule a été ajouté, sinon l'horodatage précédent
 *
 * @note Les types de véhicules sont générés aléatoirement :
 *       - 70% de voitures ('v'), 30% de camions ('c')
 *       - Spécificités : 90% standard ('s'), 5% électrique ('e'), 5% handicapé ('h')
 */
clock_t updateEnteringVehiculeList(listeVehicules_s *lv_entering,clock_t begin_clock, int delay_sec, parking *p)
{
    extern int nombre_emplacement_longueur, nb_rangee;
    char type, specificite;
    int randomSpecificite;


    if (lv_entering->type == 1 &&
        clock() - begin_clock >= delay_sec * CLOCKS_PER_SEC){
        if (p->empl[(4 * (1 + nb_rangee)) - 2][nombre_emplacement_longueur - 1].occupation == 0){
            type = (rand() % 10 > 7) ? 'c' : 'v';
            randomSpecificite = rand() % 100;
            if (randomSpecificite >= 95)      specificite = 'h';
            else if (randomSpecificite >= 90) specificite = 'e';
            else                               specificite = 's';

            vehicule *v = setVehicule('N',getVehiculePosX(nombre_emplacement_longueur - 1, type, 'N'),
            		getVehiculePosY((4 * (1 + nb_rangee)) - 1, type, 'N'), 4, type, specificite, 3, time(NULL), 1);
            addVehiculeHeadListe(v, lv_entering);
            return clock();
        }
    }

    return begin_clock;
}


/**
 * Met à jour la liste des véhicules sortants du parking
 *
 * Cette fonction sélectionne aléatoirement un véhicule stationné pour
 * déclencher son départ, selon un délai périodique
 *
 * @param lv Pointeur vers la liste des véhicules stationnés
 * @param begin_clock Horodatage de la dernière mise à jour
 * @param delay_sec Délai en secondes entre deux départs de véhicules
 *
 * @return Nouvel horodatage si un véhicule a été marqué pour sortie,
 *         sinon l'horodatage précédent
 *
 * @note Le véhicule est sélectionné aléatoirement parmi tous les véhicules stationnés
 * @note L'état du véhicule est changé à 1 (en cours de sortie)
 */
clock_t updateExitingVehiculeList(listeVehicules_s *lv, clock_t begin_clock, int delay_sec){
	if (lv->type == 2&& lv->nbVehicules > 0 && clock() - begin_clock >=delay_sec * CLOCKS_PER_SEC) { // liste de véhicules stationnés
		vehicule *v_courant = lv->listeVehicules;
		for (int i = 0; i < rand() % lv->nbVehicules; i++) {
			v_courant = v_courant->suivant;
		}
		v_courant->etat = 1;
		return clock();
	}
	return begin_clock;
}

/**
 * Initialise le parking avec un taux de remplissage défini par l'utilisateur
 *
 * Cette fonction remplit le parking avec des véhicules stationnés selon
 * le mode et un pourcentage de remplissage demandé à l'utilisateur (0-100%)
 * Les véhicules sont positionnés aléatoirement avec des caractéristiques
 * et des heures d'arrivée variées
 *
 * @param p Pointeur vers la structure du parking à initialiser
 * @param lv Pointeur vers la liste des véhicules stationnés
 *
 * @return la valeure du mode (fluide ou chargé)
 *
 * @note Les véhicules sont générés avec des heures d'arrivée simulées
 *       entre 8h et 20h avant l'heure actuelle
 * @note La distribution des types et spécificités suit la même règle
 *       que pour updateEnteringVehiculeList()
 */
int initialisationParking(parking *p, listeVehicules_s *lv) {
    int nbPlacesParking =
        (2 * p->nombre_emplacement_longueur) - 2 + 2 * p->nombre_rangee * (p->nombre_emplacement_longueur - 4);
    int nombre, mode;
    char type, specificite, randomSpecificite;
    time_t maintenant = time(NULL);
    do {
        printw("Entrez le mode de JEU : ");
        scanw("%d", &mode);
        if (!(mode == 1 || mode == 2)) {
            printw("Erreur : le mode doit être 1(chargé) ou 2 (fluide).\n");
        }
    } while (!(mode == 1 || mode == 2));
    printw("mode choisi : %d\n", mode);
    if (mode == 1) {
        printw("mode choisi CHARGÉ\n");
    } else {
        printw("mode choisi FLUIDE\n");
        mode = 5;
    }

    do {
        printw("Entrez le pourcentage de remplissage initial du parking entre "
               "0 et 100 : ");
        scanw("%d", &nombre);
        if (nombre < 0 || nombre > 100) {
            printw("Erreur : le nombre doit être compris entre 0 et 100.\n");
        }
    } while (nombre < 0 || nombre > 100);
    printw("nombre choisi : %d\n", nombre);
    for (int i = 0; i < nombre * nbPlacesParking / 100; i++) {
        type = 'v';
        if (rand() % 10 > 7) {
            type = 'c';
        }
        specificite = 's';
        randomSpecificite = rand() % 100;
        if (randomSpecificite >= 90) {
            specificite = 'e';
        }
        if (randomSpecificite >= 95) {
            specificite = 'h';
        }
        vehicule *vp = setVehicule('S', getVehiculePosX(3, type, 'S'), getVehiculePosY(4, type, 'S'), 4, type, specificite, 0, maintenant - 3600 - rand() % (3600 * 12), 1); // 8h -> 20h
        addVehiculeStationnant(vp, p, lv);
    }
    return mode;
}

