#include "jvie.h"


int jv_posValide(int x, int y, plateau* p)
{
    if (x<0 || y<0 || p->width<x || p->height<y)
	return 0;
    else
	return 1;
}

int jv_getVoisines(int x, int y, plateau* p)
{
    int v=0;
    for (int i=-1; i<=1; i++)
	for (int j=-1; j<=1; j++)
	    if (jv_posValide(i+x, j+y, p) && p->grille[i+x][j+y])
		v++;
    return v;
}

char jv_getEtatSuivant(int x, int y, plateau* p)
{
    int v = jv_getVoisines(x, y, p);
    if (v == 3)
	return 1;
    else if (v == 2 && p->grille[x][y])
	return 1;

    return 0;
}

void jv_initPlat(plateau *p)
{
    for (int i = 0; i < p->width; i++)
	for (int j = 0; j < p->height; j++)
	    p->grille[i][j] = 0;
}

plateau* jv_newPlat(int width, int height)
{
    plateau* p = (plateau*)malloc(sizeof(plateau));
    p->width = width;
    p->height = height;
    p->grille = (char**)malloc(sizeof(char*)*width);
    char* tmp = (char*)malloc(sizeof(char)*width*height);
    for (int i = 0; i < width; i++)
	p->grille[i] = &tmp[i*height];
    jv_initPlat(p);
    return p;
}

void jv_freePlat(plateau *p)
{
    free(p->grille[0]);
    free(p->grille);
}

plateau* jv_nextGen(plateau* p)
{
    plateau* ngen = jv_newPlat(p->width, p->height);
    for (int i = 0; i < ngen->width; i++)
	for (int j = 0; j < ngen->height; j++)
	    ngen->grille[i][j] = jv_getEtatSuivant(i, j, p);
    jv_freePlat(p);
    return ngen;
}

void jv_nextGen2(int width, int height, char* cells)
{
    plateau* p = jv_newPlat(width, height);
    for (int i = 0; i < width; i++)
	for (int j = 0; j < height; j++)
	    p->grille[i][j] = cells[i+j];

    for (int i = 0; i < width; i++)
	for (int j = 0; j < height; j++)
	    cells[i+j] = jv_getEtatSuivant(i, j, p);

    jv_freePlat(p);
}
