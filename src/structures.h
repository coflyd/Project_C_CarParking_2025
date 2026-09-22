/*
 * structures.h
 *
 *  Created on: 9 sept. 2025
 *      Author: constance
 */

#ifndef STRUCTURES_H_
#define STRUCTURES_H_
#include <ncursesw/curses.h>
#include <string.h>
#include <time.h>

typedef struct framedText {
	int start_x;
	int start_y;
	int height;
	int width;
	char text[50];
	int pairColorText;
	int pairColorFrame;
} framedText;

typedef struct vehicule {
	char direction; 			/*N => Nord, S => Sud, E => EST, O => OUEST*/
	int posx; 					/*Position courante coin haut gauche x de la voiture*/
	int posy;					/*Position courante coin haut gauche y de la voiture*/
	int vitesse;				/*Vitesse du véhicule*/
	char type;					/*’v’=>voiture, ’c’=>camion*/
	int identifiant;			/* identifiant unique : immatriculation*/

	char specificite;			/* 's'=> standard, 'e'=>électrique, 'h'=>handicap */
	char carrosserie[4][100];	/*Carrosserie de la voiture, servira pour l’affichage du véhicule à tout moment*/
	int code_couleur; 			/*Code couleur de la voiture à utiliser lors de l’affichage*/
	int etat;					/*État du véhicule : ’1’ => actif et ’0’ => inactif*/

	time_t instantArrivee;
	time_t instantSortie;
	int prix_parking; 			/*Prix a régler en centime d'euros*/

	struct vehicule *suivant;	/*Pointeur vers une prochaine voiture,
								nécessaire pour la liste chaînée*/

} vehicule;

typedef struct listeVehicules_s {
	vehicule *listeVehicules;
	int nbVehicules;
	int type; 					/*1 : entrant, 2 stationne, 3 sortant , (4 sortis)*/
} listeVehicules_s;

typedef struct emplacement_s {
	char type;					/*p : parking, r : zone de roulement, x : autre*/
	char specificite; 			/*'s' => standard, 'e'=> électrique, 'h'=>handicap*/
	int occupation; 			/*0 vide, 1 : occupé*/
	int dirN, dirS, dirO, dirE;
	char dirStationnement;
} emplacement_s;

typedef struct parking {
	int nombre_emplacement_longueur;
	int nombre_rangee;
	int place_largeur;
	int place_longueur;
	char *fichier_nom;
	wchar_t **plan;
	emplacement_s **empl;
	int taux_horaire;		/*Prix en fonction du temps de stationnement dans le parking*/
	int prix_a_payer;		/*Prix final a régler en centime d'euros*/


} parking;

#endif /* STRUCTURES_H_ */
