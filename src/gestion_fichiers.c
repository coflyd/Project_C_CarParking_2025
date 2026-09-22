/*
 * gestion_fichiers.c
 *
 *  Created on: 16 sept. 2025
 *      Author: constance
 *
 * Module de gestion des fichiers pour le chargement de carrosseries
 * de véhicules et la sauvegarde de matrices de caractères larges
 *
 */

#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

/**
 * Charge la carrosserie d'un véhicule depuis un fichier texte
 *
 * Cette fonction lit un fichier contenant exactement 4 lignes décrivant
 * la carrosserie d'un véhicule et retourne un tableau de chaînes de caractères.
 *
 * @param fileName Nom du fichier à lire (chemin complet ou relatif)
 *
 * @return Pointeur vers un tableau de 4 chaînes de caractères allouées dynamiquement,
 *         ou NULL en cas d'erreur (fichier inexistant ou format invalide)
 *
 * @note L'appelant est responsable de libérer la mémoire allouée
 * @warning Le fichier doit contenir au moins 4 lignes
 */
char **chargementCarrosserieVehicule(char *fileName) {
	FILE *fichIn;
	char **carrosserie = NULL;
	fichIn = fopen(fileName, "r");
	if (fichIn == NULL) {
		puts("Ouverture du fichier impossible ...");
		return NULL;
	} else {
		int nbCaracteres = 100;
		carrosserie = malloc(4 * sizeof(char *));
		for (int i = 0; i < 4; i++) {
			carrosserie[i] = malloc(nbCaracteres * sizeof(char));
			if (fgets(carrosserie[i], nbCaracteres, fichIn) == NULL) {
				printf(
					"Erreur : Le fichier ne contient pas assez de lignes.\n");
				fclose(fichIn);
				return NULL;
			}
			size_t len = strlen(carrosserie[i]);
			if (len > 0 && carrosserie[i][len - 1] == '\n') {
				carrosserie[i][len - 1] = '\0';
			}
		}
		fclose(fichIn);
	}
	for (int i = 0; i < 4; i++) {
		printf("%d : %s\n", i, carrosserie[i]);
	}
	return carrosserie;
}

/**
 * Sauvegarde une matrice de caractères larges dans un fichier UTF-8
 *
 * Cette fonction écrit une matrice bidimensionnelle de wchar_t dans un fichier
 * texte encodé en UTF-8, en préservant la structure ligne par ligne.
 *
 * @param matrice Pointeur vers la matrice de wchar_t à sauvegarder
 * @param nbLignes Nombre de lignes de la matrice
 * @param nbColonnes Nombre de colonnes de la matrice
 * @param nomFichier Nom du fichier de destination
 *
 * @return Aucune valeur de retour (void)
 *
 * @note Les caractères nuls (L'\0') sont ignorés lors de l'écriture
 * @note La locale est automatiquement configurée pour supporter UTF-8
 */
void sauvegarderMatriceWChar(wchar_t **matrice, int nbLignes, int nbColonnes, const char *nomFichier) {
	// Configurer la locale pour supporter UTF-8
	setlocale(LC_ALL, "");

	// Ouvrir le fichier en mode écriture
	FILE *fichier = fopen(nomFichier, "w, ccs=UTF-8");
	if (fichier == NULL) {
		perror("Erreur lors de l'ouverture du fichier");
		return;
	}

	/*Parcourir la matrice et écrire chaque caractère dans le fichier*/
	for (int i = 0; i < nbLignes; i++) {
		for (int j = 0; j < nbColonnes; j++) {
			if (matrice[i][j] != L'\0') {
				fputwc(matrice[i][j],
					   fichier); 					/* Écrire le caractère large dans le fichier*/
			}
		}
		fputwc(L'\n', fichier); // Ajouter un saut de ligne après chaque ligne
								// de la matrice
	}

	// Fermer le fichier
	fclose(fichier);
}
