#include "jvie_serv.h"


int jvs_zoneValide(PlateauStatut* statuts, int x, int y, int width, int height)
{
    return (0<x && 0<y && statuts->width>x+width && statuts->height>y+height);
}

int jvs_zoneLibre(PlateauStatut* statuts, int x, int y, int width, int height)
{
    if (!jvs_zoneValide(statuts, x, y, width, height))
	return 0;
    int i,j;
    for (i=x; i<x+width; i++)
	for (j=y; j<y+height; j++)
	    if (statuts->grille[i][j].statut != A_TRAITER)
		return 0;
    return 1;
}

void jvs_init(PlateauStatut* statuts, int width, int height)
{
    statuts->width = width;
    statuts->height = height;
    statuts->generation = 0;
    statuts->grille = (cellstat**)malloc(sizeof(cellstat*)*width);
    cellstat* tmp = (cellstat*)malloc(sizeof(cellstat)*width*height);
    int i,j;
    for (i=0; i<width; i++)
	statuts->grille[i] = &tmp[i*height];
    for (i=0; i<width; i++)
	for (j=0; j<width; j++)
	    statuts->grille[i][j].statut = A_TRAITER;
}

void jvs_delete(PlateauStatut* statuts)
{
    free(statuts->grille[0]);
    free(statuts->grille);
}

int jvs_assigne(PlateauStatut* statuts, int x, int y, int width, int height, SOCKET client)
{
    if (!jvs_zoneLibre(statuts, x, y, width, height))
	return -1;
    int i,j;
    for (i=x; i<x+width; i++)
	for (j=y; j<y+height; j++)
	{
	    statuts->grille[i][j].statut = EN_TRAITEMENT;
	    statuts->grille[i][j].client = client;
	}
    return 0;
}

void jvs_reprend(PlateauStatut* statuts, SOCKET client)
{
    int i,j;
    for (i=0; i<statuts->width; i++)
	for (j=0; j<statuts->height; j++)
	    if (statuts->grille[i][j].client == client)
		statuts->grille[i][j].statut = A_TRAITER;
}

int jvs_termine(PlateauStatut* statuts, int x, int y, int width, int height)
{
    int i,j;
    for (i=x; i<x+width; i++)
	for (j=y; j<y+height; y++)
	    statuts->grille[i][j].statut = TRAITEE;
    return 0;
}

int jvs_finGen(PlateauStatut* statuts)
{
    int i,j;
    for (i=0; i<statuts->width; i++)
	for (j=0; j<statuts->height; j++)
	    if (statuts->grille[i][j].statut != TRAITEE)
		return 0;
    return 1;
}

int jvs_nextGen(PlateauStatut* statuts)
{
    if (!jvs_finGen(statuts))
	return -1;
    int i,j;
    for (i=0; i<statuts->width; i++)
	for (j=0; j<statuts->height; j++)
	    statuts->grille[i][j].statut = A_TRAITER;
    statuts->generation ++;
    return statuts->generation;
}
