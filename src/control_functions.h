/*
 * control_functions.h
 *
 *  Created on: 6 oct. 2025
 *      Author: constance
 */

#ifndef CONTROL_CONTROL_FUNCTIONS_H_
#define CONTROL_CONTROL_FUNCTIONS_H_

#include <time.h>
#include "structures.h"

clock_t updateEnteringVehiculeList(listeVehicules_s *lv_entering, clock_t begin_clock, int delay_sec, parking *p);
clock_t updateExitingVehiculeList(listeVehicules_s *lv, clock_t begin_clock, int delay_sec);
int initialisationParking(parking *p, listeVehicules_s *lv) ;

#endif /* CONTROL_CONTROL_FUNCTIONS_H_ */
