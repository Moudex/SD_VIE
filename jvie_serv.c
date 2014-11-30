#include "jvie_serv.h"

/* La zone est elle sur le plateau */
int jvs_zoneValide(PlateauStatut* statuts, int x, int y, int width, int height)
{
    return (0<x && 0<y && statuts->width>x+width && statuts->height>y+height);
}

/* La zone est elle a traiter */
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

/* Initialise un plateau */
void jvs_init(PlateauStatut* statuts)
{
    int i, j;
    for (i=0; i<statuts->width; i++)
	for (j=0; j<statuts->height; j++)
	    statuts->grille[i][j].statut = A_TRAITER;
}

/* Construit un plateau */
PlateauStatut* jvs_newPlat(int width, int height)
{
    PlateauStatut* statuts = (PlateauStatut*)malloc(sizeof(PlateauStatut));
    statuts->width = width;
    statuts->height = height;
    statuts->generation = 0;
    statuts->grille = (cellstat**)malloc(sizeof(cellstat*)*width);
    cellstat* tmp = (cellstat*)malloc(sizeof(cellstat)*width*height);
    int i;
    for (i=0; i<width; i++)
	statuts->grille[i] = &tmp[i*height];
    jvs_init(statuts);
    return statuts;
}

/* Detruit un plateau */
void jvs_delete(PlateauStatut* statuts)
{
    free(statuts->grille[0]);
    free(statuts->grille);
    free(statuts);
}

/* Assigne un client à une zone */
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

/* Remet une zone asssigne a un client a traiter */
void jvs_reprend(PlateauStatut* statuts, SOCKET client)
{
    int i,j;
    for (i=0; i<statuts->width; i++)
	for (j=0; j<statuts->height; j++)
	    if (statuts->grille[i][j].client == client)
		statuts->grille[i][j].statut = A_TRAITER;
}

/* Termine le traitement d'un client */
int jvs_termine(PlateauStatut* statuts, int x, int y, int width, int height)
{
    int i,j;
    for (i=x; i<x+width; i++)
	for (j=y; j<y+height; y++)
	    statuts->grille[i][j].statut = TRAITEE;
    return 0;
}

/* La generation est elle terminee */
int jvs_finGen(PlateauStatut* statuts)
{
    int i,j;
    for (i=0; i<statuts->width; i++)
	for (j=0; j<statuts->height; j++)
	    if (statuts->grille[i][j].statut != TRAITEE)
		return 0;
    return 1;
}

/* Passe a la generation suivante */
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

/* retourne le premier block libre de taille width height */
Block jvs_getBlock(PlateauStatut* statuts, int width, int height)
{
	int i,j;
	if (width > 1 || height > 1)
	{
	for (i=0; i<statuts->width-width; i++)
		for (j=0; j<statuts->height-height; j++)
		{
			/* teste ce motif */
			int libre=1;
			int k=i;
			while (libre && k<i+width)
			{
				int l=j;
				while (libre && l<j+height)
				{
					if (statuts->grille[k][l].statut != A_TRAITER)
						libre = 0;
					l++;
				}
				k++;
			}
			
			/* Si on a trouvé un block */
			if (libre)
			{
				Block b;
				b.x = i;
				b.y = j;
				b.width = width;
				b.height = height;
				return b;
			}
		}
	}
	else
	{
	/* Sinon on retourne le premier block 1X1 dispo */
	for (i=0; i<statuts->width; i++)
		for (j=0; j<height; j++)
			if (statuts->grille[i][j].statut == A_TRAITER)
			{
				Block b;
				b.x = i;
				b.y = j;
				b.width = 1;
				b.height = 1;
				return b;	
			}
	}
	
	/* Aucun block de dispo */
	Block b;
	b.x = -1;
	b.y = -1;
	b.width = -1;
	b.height = -1;
	return b;
}
