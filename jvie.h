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

/* La position est elle sur le plateau */
int jv_posValide(int x, int y, plateau* p);

/* retourne le nb de voisins de la cellule */
int jv_getVoisines(int x, int y, plateau* p);

/* retourne l'etat suivant de la cellule */
char jv_getEtatSuivant(int x, int y, plateau* p);

/* Initialise le plateau a vide */
void jv_initPlat(plateau *p);

/* Alloue un nouveau plateau */
plateau* jv_newPlat(int width, int height);

/* Libere un plateau */
void jv_freePlat(plateau *p);

/* calcule la generation suivante du plateau */
void jv_nextGen(plateau* p);

/* Clone un plateau */
plateau* clone(plateau* p);

/* Compacte une portion de plateau pour l'envoi avec bordure supplémentaire*/
char* jv_pack_s(plateau* p, int x, int y, int width, int height);

/* compacte un plateau */
void jv_pack_c(plateau* p, char* pack);

/* decompacte une portion de plateau sans les bords */
void jv_unpack_s(plateau* p, char* pack, int x, int y, int width, int height);

/* decompacte un plateau */
plateau* jv_unpack_c(char* pack, int width, int height);

#endif
