/*
 * model_functions.h
 *
 *  Created on: 10 sept. 2025
 *      Author: constance
 */

#ifndef MODEL_FUNCTIONS_H_
#define MODEL_FUNCTIONS_H_
#include "structures.h"

void asciiExtendedToUTF8(int asciiCode, char *utf8Buffer);

wchar_t** setParkingPlan(int place_largeur, int place_longueur, int nombre_emplacement_longueur, int nombre_rangee, int ColorPair, char *nom_fichier);
parking* setParking(int place_largeur, int place_longueur, int nombre_emplacement_longueur, int nombre_rangee, int ColorPair, char *plan, int taux_horaire);
void setCarrosserie(vehicule *v);
vehicule* setVehicule(char direction, int posx, int posy, int vitesse, char type, char specificite, int code_couleur, time_t h_arrivee, int etat);
void afficheVehicule(vehicule *v);
void afficheListeVehicule(listeVehicules_s *lv);
listeVehicules_s* setListeVehiculeVide(int typeListe);
void addVehiculeHeadListe(vehicule *v, listeVehicules_s *lv);
void updateCarPosition(vehicule *v);
void updateListeVehiculesPosition(listeVehicules_s *lv);
void updateListesVehiculesPositionSimulation(listeVehicules_s **lv, int nbListes, parking *p);
int updateCarPositionSimulation(vehicule *v, parking *p, int statut);
vehicule* removeVehiculeByImmatriculation(int immatriculation, listeVehicules_s *lv);
vehicule* removeVehiculeByPositionInList(int pos, listeVehicules_s *lv);
void initiateEmplacements(parking *p);
int getVehiculePosX(int numLigne, char vehiculeType, char Direction);
int getVehiculePosY(int numCol, char vehiculeType, char Direction);
int getVehiculeNumLigne(int posY, char vehiculeType, char Direction);
int getVehiculeNumColonne(int posX, char vehiculeType, char Direction);
int addVehiculeStationnant(vehicule *v, parking *p, listeVehicules_s *lv);
void open_log_terminal();
void write_log(const char *format, ...);
int prixParking(vehicule *v, int tauxHoraire);
#endif /* MODEL_FUNCTIONS_H_ */
