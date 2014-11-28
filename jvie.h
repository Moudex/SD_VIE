#ifndef JVIE_H
#define JVIE_H

#include <stdlib.h>
#include <stdio.h>

typedef struct plateau plateau;
struct plateau
{
    int width;
    int height;
    char** grille;
};

int jv_posValide(int x, int y, plateau* p);
int jv_getVoisines(int x, int y, plateau* p);
char jv_getEtatSuivant(int x, int y, plateau* p);
void jv_initPlat(plateau *p);
plateau* jv_newPlat(int width, int height);
void jv_freePlat(plateau *p);
plateau* jv_nextGen(plateau* p);
void jv_nextGen2(int width, int height, char* cells);
void jv_packCells(plateau* p, char* pack, int x, int y, int width, int height);


#endif
