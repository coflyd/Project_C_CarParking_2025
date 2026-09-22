/*
 * display_functions.h
 *
 *  Created on: 9 sept. 2025
 *      Author: constance
 */

#ifndef DISPLAY_FUNCTIONS_H_
#define DISPLAY_FUNCTIONS_H_
#include "structures.h"
char key_pressed();
int countChar(const char *str, char ch);
void displayInterfaceBuffer(WINDOW *win, int w, int h);
void displayEmplacementsBuffer(WINDOW *win,  parking *p, int colorPlaceLibre, int colorPlaceOccupee, int colorRoulementLibre, int colorRoulementOccupee, int colorAutre);
void displayFramedText(framedText *ft);
void displayFramedTextBufferSortie(WINDOW *win, framedText *ft);
void displayFramedTextBufferPrix(WINDOW *win, framedText *ft, int prixTotal);
void displayCarBuffer(WINDOW *win, vehicule *car);
void displayParkingBuffer(WINDOW *win, parking *p, int colorPair);
void displayListeVehiculesBuffer(WINDOW *win, listeVehicules_s *lv);
void handle_resize(int sig);
void initialisationMenu();

#endif /* DISPLAY_FUNCTIONS_H_ */
