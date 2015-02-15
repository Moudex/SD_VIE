#include <unistd.h>
#include <stdlib.h>
#include <ncurses.h>

#include "jvie.h"
#include "jvie_serv.h"

void aff_init(void);

void aff_end(void);

void aff_plateau_c(plateau* p);

void aff_plateau_s(plateau* p, PlateauStatut* statuts);

void aff_maj_s(plateau*p, PlateauStatut* statuts, int x, int y, int width, int height);
