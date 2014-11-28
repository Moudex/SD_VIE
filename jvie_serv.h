#ifndef JVIE_SERV_H
#define JVIE_SERV_H

#include <stdio.h>
#include <stdlib.h>

typedef int SOCKET;

typedef enum
{
    A_TRAITER = 0,
    EN_TRAITEMENT = 1,
    TRAITEE = 2
}c_stat;

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
	int x;
	int y;
	int width;
	int height;
}Block;

int jvs_zoneValide(PlateauStatut* statuts, int x, int y, int width, int height);
int jvs_zoneLibre(PlateauStatut* statuts, int x, int y, int width, int height);
void jvs_init(PlateauStatut* statuts, int  width, int height);
PlateauStatut* jvs_newPlat(int width, int height);
int jvs_assigne(PlateauStatut* statuts, int x, int y, int width, int height, SOCKET client);
void jvs_reprend(PlateauStatut* statuts, SOCKET client);
int jvs_termine(PlateauStatut* statuts, int x, int y, int width, int height);
int jvs_finGen(PlateauStatut* statuts);
void jvs_delete(PlateauStatut* statuts);
int jvs_nextGen(PlateauStatut* statuts);
Block jvs_getBlock(PlateauStatut* statuts, int width, int height);

#endif
