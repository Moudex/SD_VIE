#ifndef JVIE_SERV_H
#define JVIE_SERV_H

#include <stdio.h>
#include <stdlib.h>

typedef int SOCKET;

typedef enum
{
    A_TRAITER_V = 0,
    A_TRAITER_H = 1,
    A_TRAITER = 2,

    EN_TRAITEMENT_V = 3,
    EN_TRAITEMENT_H = 4,
    EN_TRAITEMENT = 5,

    TRAITEE_V = 6,
    TRAITEE_H = 7,
    TRAITEE = 8
}c_stat;

typedef enum
{
    VIRUS = 0,
    HEAL = 1,
    NORMAL = 2
}type;

typedef struct
{
    c_stat statut;
    SOCKET client;
}cellstat;

typedef struct
{
    int width;
    int height;
    int generation;
    cellstat** grille;
}PlateauStatut;

typedef struct
{
	type t;
	int x;
	int y;
	int width;
	int height;
}Block;

/* La zone est elle sur le plateau */
int jvs_zoneValide(PlateauStatut* statuts, int x, int y, int width, int height);

/* La zone est elle a traiter */
int jvs_zoneLibre(PlateauStatut* statuts, int x, int y, int width, int height);

/* Initialise un plateau */
void jvs_init(PlateauStatut* statuts);

/* Construit un plateau */
PlateauStatut* jvs_newPlat(int width, int height);

/* Detruit un plateau */
void jvs_delete(PlateauStatut* statuts);

/* Assigne un client à une zone */
int jvs_assigne(PlateauStatut* statuts, int x, int y, int width, int height, SOCKET client);

/* Assigne un client a la reslution d'un heal ou virus */
int jvs_assigne_vh(PlateauStatut* statuts, int x, int y, SOCKET client);

/* Remet une zone assigne au client a traiter */
void jvs_reprend(PlateauStatut* statuts, SOCKET client);

/* Termine le traitement d'un client */
int jvs_termine(PlateauStatut* statuts, int x, int y, int width, int height);

/* La generation est elle terminee */
int jvs_finGen(PlateauStatut* statuts);

/* Passe a la generation suivante */
int jvs_nextGen(PlateauStatut* statuts);

/* Retourne le premier block libre de taille width height */
Block jvs_getBlock(PlateauStatut* statuts, int width, int height);

/* Remplis le sac de virus ou de heal */
int jvs_set_vh(PlateauStatut* statuts, type t, int x, int y);

int jvs_termine2(PlateauStatut* statuts, SOCKET client);
#endif
