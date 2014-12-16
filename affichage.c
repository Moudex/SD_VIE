#include "affichage.h"


void aff_init(void)
{
    initscr();
    if (!has_colors())
    {
	endwin();
	perror("Ce terminal ne gere pas la couleur");
	exit(1);
    }
    if (start_color() != OK)
    {
	endwin();
	perror("impossible d'initialiser les couleurs");
    }

    init_pair(1, COLOR_WHITE, COLOR_MAGENTA); /* Non traite V */
    init_pair(2, COLOR_WHITE, COLOR_BLUE); /* Non tarite H */
    init_pair(3, COLOR_WHITE, COLOR_BLACK); /* Non traite */

    init_pair(4, COLOR_MAGENTA, COLOR_YELLOW); /* En cour de traitement v */
    init_pair(5, COLOR_BLUE, COLOR_YELLOW); /* En cour de traitement h */
    init_pair(6, COLOR_WHITE, COLOR_YELLOW); /* En cour de traitement */

    init_pair(7, COLOR_WHITE, COLOR_MAGENTA); /* traite v */
    init_pair(8, COLOR_WHITE, COLOR_BLUE); /* traite h */
    init_pair(9, COLOR_WHITE, COLOR_GREEN); /* traite */
}

void aff_end(void)
{
    endwin();
}

void aff_plateau_c(plateau* p)
{
    int i,j;
    for (i=0; i<p->width; i++)
	for (j=0; j<p->height; j++)
	{
	    if(p->grille[i][j])
		mvprintw(i,j, "O");
	    else
		mvprintw(i,j, ".");
	}
    refresh();
}

void aff_plateau_s(plateau* p, PlateauStatut* statuts)
{
    int i, j;
    for (i=0; i<p->width; i++)
	for (j=0; j<p->height; j++)
	{
	    char c = '.';
	    if (p->grille[i][j])
		c = 'O';

	    attrset(COLOR_PAIR(statuts->grille[i][j].statut+1));
	    mvprintw(i,j, "%c", c);
	}
    refresh();
}

void aff_maj_s(plateau* p, PlateauStatut* statuts, int x, int y, int width, int height)
{
    int i, j;
    for (i=x; i<p->width; i++)
	for (j=y; j<p->height; j++)
	{
	    char c = '.';
	    if (p->grille[i][j])
		c = 'O';

	    switch (statuts->grille[i][j].statut)
	    {
		case A_TRAITER:
		    attrset(COLOR_PAIR(1));
		    break;
		case EN_TRAITEMENT:
		    attrset(COLOR_PAIR(2));
		    break;
		case TRAITEE:
		    attrset(COLOR_PAIR(3));
		    break;
	    }

	    mvprintw(i,j, "%c", c);
	}
    refresh();
}
