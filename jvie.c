#include "jvie.h"

/* La position est elle sur le plateau */
int jv_posValide(int x, int y, plateau* p)
{
    return (p!=NULL && x>=0 && y>=0 && x<p->width && y<p->height);
}

/* retourne le nb de voisins de la cellule */
int jv_getVoisines(int x, int y, plateau* p)
{
    if (p==NULL)
	return -1;

    int i,j,v;
    v=0;
    for (i=-1; i<=1; i++)
	for (j=-1; j<=1; j++)
	    if (jv_posValide(i+x, j+y, p) && p->grille[i+x][j+y])
		v++;
    return v;
}

/* retourne l'etat suivant de la cellule */
char jv_getEtatSuivant(int x, int y, plateau* p)
{
    if (!jv_posValide(x,y,p))
	return -1;

    int v = jv_getVoisines(x, y, p);
    if (v == 3)
	return 1;
    else if (v == 2 && p->grille[x][y])
	return 1;

    return 0;
}

/* initialise le plateau a vide */
void jv_initPlat(plateau *p)
{
    if (p==NULL)
	return ;

    int i,j;
    for (i = 0; i < p->width; i++)
	for (j = 0; j < p->height; j++)
	    p->grille[i][j] = 0;
}

/* Alloue un nouveau plateau */
plateau* jv_newPlat(int width, int height)
{
    if (width<1 || height<1)
	return NULL;

    plateau* p = (plateau*)malloc(sizeof(plateau));
    p->width = width;
    p->height = height;
    p->grille = (char**)malloc(sizeof(char*)*width);
    char* tmp = (char*)malloc(sizeof(char)*width*height);
    int i;
    for (i = 0; i < width; i++)
	p->grille[i] = &tmp[i*height];
    jv_initPlat(p);
    return p;
}

/* Libere un plateau */
void jv_freePlat(plateau *p)
{
    if (p==NULL)
	return;

    free(p->grille[0]);
    free(p->grille);
}

/* Clone un plateau */
plateau* clone(plateau* p)
{
    plateau* c = jv_newPlat(p->width, p->height);
    int i,j;
    for (i=0; i<p->width; i++)
	for (j=0; j<p->height; j++)
	    c->grille[i][j] = p->grille[i][j];
    return c;
}

/* calcule la generation suivante du plateau */
void jv_nextGen(plateau* p)
{
    if (p==NULL)
	return;
    plateau* tmp = clone(p);

    int i,j;
    for (i = 0; i < p->width; i++)
	for (j = 0; j < p->height; j++)
	    p->grille[i][j] = jv_getEtatSuivant(i, j, tmp);
    jv_freePlat(tmp);
}

/* Compacte une portion de plateau pour l'envoi avec bordure supplémentaire*/
char* jv_pack_s(plateau* p, int x, int y, int width, int height)
{
    if (x<0 || y<0 || x+width-2 > p->width || y+height-2 > p->height)
	return NULL;
    

    char* pack = (char*)malloc(sizeof(char)*width*height);
    int i,j;
    for (i=0; i<width; i++)
	for (j=0; j<height; j++)
	{
	    if (jv_posValide(x+i-1,y+j-1,p))
		pack[i*height+j] = p->grille[x+i-1][y+j-1];
	    else
		pack[i*height+j] = 0;
	}
    return pack;
}

/* compacte un plateau */
void jv_pack_c(plateau* p, char* pack)
{
    int i,j;
    for (i=0; i<p->width; i++)
	for (j=0; j<p->height; j++)
	    pack[i*p->height+j] = p->grille[i][j];
    free(p);
}

/* decompacte une portion de plateau sans les bords */
void jv_unpack_s(plateau* p, char* pack, int x, int y, int width, int height)
{
    if (width<3 || height<3 || pack==NULL)
	return;

    int i,j;
    for (i=0; i<width; i++)
	for (j=0; j<height; j++)
	    if (i>0 && i< width-1 && j>0 && j<height-1)
		p->grille[i+x-1][j+y-1] = pack[i*height+j];
    free(pack);
}

/* decompacte un plateau */
plateau* jv_unpack_c(char* pack, int width, int height)
{
    if (width<3 || height<3 || pack==NULL)
	return NULL;

    plateau* p = jv_newPlat(width, height);
    int i,j;
    for (i=0; i<width; i++)
	for (j=0; j<height; j++)
	    p->grille[i][j] = pack[i*height+j];
    return p;
}
