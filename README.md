README.md

Auteurs : Constance Fleury
Projet : ESIEA 3A S1 - Année 2025/2026
Date de création : 12 décembre 2025

Projet Autonome en Programmation C - 3A
SIMULATEUR DE PARKING LE JEU

Un simulateur de parking intelligent développé en C, permettant de gérer le stationnement automatisé de véhicules dans un parking payant avec affichage terminal dynamique utilisant ncurses.

Table des matières
    Description
    Fonctionnalités
    Prérequis
    Utilisation
    Modes de jeu
    Aspects techniques
    INDEX : Architecture

Ce projet simule la gestion d'un parking payant avec entrée/sortie, réalisation de paiement et gestion intelligente des places. Les véhicules se déplacent de manière autonome pour trouver des places libres et gérer leur stationnement en fonction de leur type.

Le simulateur intègre :

    Véhicules de différents modèles et couleurs
    Indicateurs de places (vert = libre, rouge = occupé, violet = électrique, bleu = handicapé)
    Une borne d'Entrée et de Sortie
    Un affichage sur la borne Sortie du montant en € à regler à la fin du temps de stationnement
    Une signalitique au sol du sens de déplacement autorisé dans la parking


Fonctionnalités : 
Visuel :
    Affichage console dynamique avec ncurses : Interface temps réel fluide et optimisée
    Gestion intelligente des véhicules : Déplacement autonome avec évitement de collisions
    Système de paiement : Calcul automatique des frais selon le temps de stationnement
    Modes de difficulté multiples : Fluide, Chargé, et taux de remplissage initial du parking
    Personnalisation : Design de véhicules et décor modifiables

    <img width="1401" height="777" alt="Capture d’écran 2026-09-22 à 17 58 01" src="https://github.com/user-attachments/assets/91d50fb1-b03a-47da-bd6b-871f0789858a" />

Technique :
    Liste chaînée : Gestion dynamique des véhicules en mémoire
    Support UTF-8 complet : Affichage de caractères spéciaux et émojis via ncursesw


Prérequis
Système d'exploitation :
    GNU/Linux
    macOS (avec Homebrew)

Librairies essentielles : 
    CRITICAL : ncursesw est OBLIGATOIRE pour ce projet
    ncursesw : Librairie ncurses avec support UTF-8 (CAPITAL pour l'affichage)

Librairies système standard :
    stdlib.h
    stdio.h
    string.h
    time.h
    unistd.h
    termios.h
    wchar.h
    sys/stat.h
    fcntl.h
    locale.h


Utilisation du jeu
#Lancer le simulateur : ./Debug/C_project_car2

#IMPORTANT : Ajustez le chemin du fichier dans main.c selon votre installation :
    cchar parkingPlanFileName[] = "/Users/constancefleury/Desktop/C_project_car2/src/data/parking.txt";

Menu de démarrage :

<img width="531" height="261" alt="Capture d’écran 2026-09-22 à 17 58 37" src="https://github.com/user-attachments/assets/4975649a-5960-4518-8072-349a4c4026d4" />

Choisir le mode de jeu avec les touches numériques :
    1 : Mode CHARGÉ (trafic dense)
    2 : Mode FLUIDE (circulation fluide)
Choisir le pourcentage de remplissage initial du parking (0-100%)

Pendant la simulation :
    Q ou ESC : Quitter le jeu
    Interaction en temps réel : Le jeu utilise key_pressed() pour la détection non-bloquante

Configuration
Les fichiers de configuration se trouvent dans le dossier src/data/ :
    parking.txt : Plan du parking avec décor personnalisé
    voiture_1.txt, voiture_2.txt : Modèles de véhicules (carrosserie ASCII)

Modes de jeu

    Mode Chargé (Mode 1)
        Génération de véhicules toutes les 1 seconde
        Trafic dense avec nombreux véhicules
        Risques de bouchons
        Places limitées rapidement
        Challenge accru en gestion de flux

    Mode Fluide (Mode 2)
        Génération de véhicules toutes les 5 secondes
        Circulation fluide sans embouteillages
        Places disponibles en abondance
        Sortie de véhicules toutes les 10 secondes
        Idéal pour tester les fonctionnalités

    Taux de remplissage initial
    Au démarrage, vous pouvez choisir le pourcentage de places déjà occupées (0-100%) :
        0% : Parking vide au départ
        50% : Parking à moitié plein
        100% : Parking complet (mode observation)

Aspects techniques
    Algorithmes clés
        Génération de véhicules
        Distribution probabiliste réaliste :
            70% voitures ('v'), 30% camions ('c')
            90% standard ('s'), 5% électrique ('e'), 5% handicapé ('h')

    Recherche de place
        Vérification des places compatibles dans les directions autorisées
        Priorité aux places proches
        Déplacement aléatoire en cas de blocage (max 10 tentatives)
        Respect des sens de circulation

    Calcul du prix
        Tarification progressive :
            30 premières minutes : Gratuit
            Au-delà : 0.25€ par tranche de 15 minutes commencée
    Gestion des collisions
    Matrice d'occupation en temps réel :
        Chaque emplacement a un flag occupation (0/1)
        Vérification avant tout déplacement
        Mise à jour atomique position → occupation

On espère que notre projet vous plaira !         

INDEX : 
Architecture du projet

    project_car2/
    ├── Binaries/                  # Exécutables compilés
    ├── Includes/                  # Fichiers d'en-tête (.h)
    ├── Debug/                     # Configuration de débogage Eclipse
    │   ├── src/
    │   ├── src-4/
    │   ├── debug.txt
    │   ├── makefile
    │   └── sources.mk
    ├── src/                       # Code source principal
    │   ├── data/                  # Fichiers de données du jeu
    │   │   ├── parking.txt        # Plan du parking
    │   │   ├── voiture_1.txt      # Modèle véhicule : voiture
    │   │   └── voiture_2.txt      # Modèle véhicule : camion
    │   ├── control_functions.c    # Logique de contrôle et IA
    │   ├── control_functions.h
    │   ├── display_functions.c    # Gestion affichage console
    │   ├── display_functions.h
    │   ├── gestion_fichiers.c     # Lecture/écriture fichiers
    │   ├── gestion_fichiers.h
    │   ├── main.c                 # Point d'entrée du programme
    │   ├── model_functions.c      # Modèles de données et structures
    │   ├── model_functions.h
    │   └── structures.h           # Définitions des structures
    ├── makefile                   # Compilation automatisée
    └── README.md                  # Documentation

Description des modules :

    main.c - Point d'entrée du programme
        Initialisation ncurses et configuration des couleurs
        Boucle principale du jeu avec gestion du temps
        Gestion des buffers d'affichage
        Coordination des différentes listes de véhicules

    control_functions.c/.h - Contrôle des flux de véhicules
        updateEnteringVehiculeList() : Génération périodique de nouveaux véhicules entrants
        updateExitingVehiculeList() : Sélection aléatoire de véhicules pour sortie
        initialisationParking() : Initialisation avec taux de remplissage personnalisable
        Distribution des types de véhicules (70% voitures, 30% camions)
        Distribution des spécificités (90% standard, 5% électrique, 5% handicapé)

    display_functions.c/.h - Gestion de l'affichage ncurses
        displayInterfaceBuffer() : Affichage du cadre principal
        displayParkingBuffer() : Rendu du plan textuel du parking
        displayEmplacementsBuffer() : Affichage de la grille avec places et directions
        displayCarBuffer() : Rendu graphique des véhicules
        displayListeVehiculesBuffer() : Affichage de tous les véhicules d'une liste
        displayFramedText() : Création de textes encadrés (bornes d'entrée/sortie)
        displayFramedTextBufferPrix() : Affichage du prix à payer
        key_pressed() : Détection non-bloquante des touches (compatible macOS)
        handle_resize() : Gestion du redimensionnement de terminal

    gestion_fichiers.c/.h - Traitement des fichiers
        chargementCarrosserieVehicule() : Chargement des modèles de véhicules depuis fichiers
        sauvegarderMatriceWChar() : Sauvegarde de matrices UTF-8
        Gestion de l'encodage UTF-8 pour caractères spéciaux

    model_functions.c/.h - Structures de données et logique métier
        Gestion du parking :
            setParking() : Initialisation de la structure parking
            setParkingPlan() : Chargement du plan depuis fichier
            initiateEmplacements() : Configuration de la grille d'emplacements

        Gestion des véhicules :
            setVehicule() : Création d'un véhicule avec tous ses attributs
            setCarrosserie() : Définition de la représentation visuelle
            updateCarPosition() : Déplacement simple avec gestion des limites
            updateCarPositionSimulation() : Déplacement intelligent avec logique parking
            addVehiculeStationnant() : Placement aléatoire dans un emplacement libre

        Listes chaînées :
            setListeVehiculeVide() : Création d'une liste vide
            addVehiculeHeadListe() : Insertion en tête avec mise à jour des attributs
            removeVehiculeByImmatriculation() : Suppression par identifiant
            removeVehiculeByPositionInList() : Suppression par position
            updateListesVehiculesPositionSimulation() : MAJ de toutes les listes
            
        Utilitaires :
            getVehiculePosX/Y() : Calcul position écran depuis coordonnées grille
            getVehiculeNumLigne/Colonne() : Calcul coordonnées grille depuis position écran
            prixParking() : Calcul du prix (30 min gratuites, 0.25€/15min ensuite)
            afficheVehicule() : Affichage debug des infos véhicule
