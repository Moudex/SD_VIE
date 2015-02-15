#include "server.h"
#include <time.h>

static void app(void)
{
    SOCKET sock = init_connection();
    int actual = 0;
    int max = sock;
    Client clients[MAX_CLIENTS];
    srand(time(NULL));
    /* Initialiser la plateau de jeu de la vie
     * et son plateau d'etats
     */

    plateau* p_vie = jv_newPlat(GAME_SIZE, GAME_SIZE);
    jv_randPlat(p_vie);
    plateau* p_vie_next = jv_newPlat(GAME_SIZE, GAME_SIZE);
    PlateauStatut* p_statuts = jvs_newPlat(GAME_SIZE, GAME_SIZE);
    int fintache = 0;

    aff_init();

    fd_set rdfs;
    while(1)
    {
	int i = 0;
	FD_ZERO(&rdfs);
	FD_SET(STDIN_FILENO, &rdfs);
	FD_SET(sock, &rdfs);
	for(i=0; i<actual; i++)
	{
	    FD_SET(clients[i].sock, &rdfs);
	}

	if(select(max+1, &rdfs, NULL, NULL, NULL) == -1)
	{
	    perror("select()");
	    exit(errno);
	}

	if(FD_ISSET(STDIN_FILENO, &rdfs))
	{
	    /* stop when type on keyboard */
	    break;
	}
	else if(FD_ISSET(sock, &rdfs))
	{
	    /* new client */
	    SOCKADDR_IN csin = { 0 };
	    size_t sinsize = sizeof csin;
	    int csock = accept(sock, (SOCKADDR *)&csin, &sinsize);
	    if(csock == SOCKET_ERROR)
	    {
		perror("accept()");
		continue;
	    }

	    /* Lire la commande startCom */
	    Command cmd;
	    if(readCmd(csock, &cmd) == -1)
	    {
		continue;
	    }
	    if(cmd.type != CMD_START_COMMUNICATION)
	    {
		continue;
	    }
	    
	    if(writeCmd(csock, &cmd) > 0)
	    {
		max = csock > max ? csock : max;

		FD_SET(csock, &rdfs);
		Client c;
		c.sock = csock;
		c.type = 0;
		c.generation = -1;
		c.x = -1;
		c.y = -1;
		c.width = -1;
		c.height = -1;
		clients[actual] = c;
		actual ++;
	    }
	}
	else
	{
	    int i = 0;
	    for(i=0; i<actual; i++)
	    {
		/* Commande d'un client */
		if(FD_ISSET(clients[i].sock, &rdfs))
		{
		    Client client = clients[i];
		    Command cmd;
		    int c = readCmd(clients[i].sock, &cmd);
		    if(c==0)
		    {
			close(clients[i].sock);
			remove_client(clients, i, &actual);
		    }
		    else
		    {
			switch(cmd.type)
			{
			    Command com;
			    /* CLient demande une tache */
			    case CMD_REQUEST_TASK:
			    {	
			    	/* TODO verifier si le process n'est pas déja sur un block non traité ou en traitement */
			    	
				/* sélection aleatoire d'une commande */
				int ctp;
				if (!fintache)
				    ctp = rand() % 100;
				else
				    ctp = 1;

				/* Retirer une tache du sac */
				if (ctp < 80)
				{
				    Block b = jvs_getBlock(p_statuts, BLOCK_SIZE, BLOCK_SIZE);
				    if (b.x == -1)
				    {
					/* erreur, aucun block libre */
					Command att;
					att.type = CMD_NO_TASK;
					att.noTask.waitingTime = 0.5;
					writeCmd(clients[i].sock, &att);
					break;
				    }

				    /* verifier le type de bloc */
				    if (b.t == NORMAL)
				    {
					/* Construction et envoi de la commande */
					char* pack = jv_pack_s(p_vie, b.x, b.y, b.width+2, b.height+2);
					com.type = CMD_TASK;
					com.task.width = b.width+2;
					com.task.height = b.height+2;
					com.task.cells = pack;
					if (writeCmd(clients[i].sock, &com)>0)
					{
					    /* Assigne le block au client */
					    if (jvs_assigne(p_statuts, b.x, b.y, b.width, b.height, clients[i].sock) == -1)
						printf("erreur");
					    clients[i].generation = p_statuts->generation;
					    clients[i].x = b.x; clients[i].y = b.y; clients[i].width = b.width; clients[i].height = b.height;		    	 
					    clients[i].type = CMD_TASK;
					}
					else
					{
					    /* enlever le client */
					    close(clients[i].sock);
					    remove_client(clients, i, &actual);
					}
					free(pack);
				    }
				    else
				    {
					fintache = 1;
					/* construire la commande */
					com.type = CMD_LIST_CELL;
					if (b.t == HEAL) com.listCell.type = CMD_HEAL;
					else com.listCell.type = CMD_VIRUS;
					com.listCell.nb =1;
					com.listCell.list = (coord*)malloc(sizeof(coord));
					com.listCell.list[0].cell = p_vie->grille[b.x][b.y];
					com.listCell.list[0].x = b.x; com.listCell.list[0].y = b.y;
					/* envoyer la cellule */
					if (writeCmd(clients[i].sock, &com)>0)
					{
					    /* Assigner le traitement heal */
					    jvs_assigne_vh(p_statuts, b.x, b.y, clients[i].sock);
					    clients[i].generation = p_statuts->generation;
					    clients[i].x = b.x; clients[i].y = b.y; clients[i].width = b.width; clients[i].height = b.height;
					    if (b.t == HEAL) clients[i].type = CMD_HEAL;
					    else clients[i].type = CMD_VIRUS;
					}
					else
					{
					    /* Enelever le client */
					    close(clients[i].sock);
					    remove_client(clients, i, &actual);
					}
					free(com.listCell.list);
				    }
				}
				/* Générer vie */
				else if (ctp<98)
				{
				    /* Construction commande */
				    com.type = CMD_HEAL;
				    com.heal.width = p_vie->width;
				    com.heal.height = p_vie->height;
				    /* Envoi de la commande */
				    if (writeCmd(clients[i].sock, &com)<=0)
				    {
					/* Enlever le client */
					close(clients[i].sock);
					remove_client(clients, i, &actual);
				    }
				}
				/* Infection virale */
				else
				{
				    /* Construction commande */
				    com.type = CMD_VIRUS;
				    com.virus.width = p_vie->width;
				    com.virus.height = p_vie->height;
				    /* Envoi de la commande */
				    if (writeCmd(clients[i].sock, &com) <=0)
				    {
					/* Enlever client */
					close(clients[i].sock);
					remove_client(clients, i, &actual);
				    }
				}
				break;
			    }

			    /* Client envoi resultat generation */
			    case CMD_TASK:
			    if (p_statuts->generation == clients[i].generation)
			    {
				jv_unpack_s(p_vie_next, cmd.task.cells, clients[i].x, clients[i].y, cmd.task.width, cmd.task.height);
				jvs_termine(p_statuts, clients[i].x, clients[i].y, clients[i].width, clients[i].height);
				//jvs_termine2(p_statuts, clients[i].sock);
			    }	
				break;

			    /* autres types commandes */
			    case CMD_LIST_CELL:
			    {
				type t = VIRUS;
				switch (cmd.listCell.type)
				{
				case CMD_HEAL:
				    t=HEAL;
				case CMD_VIRUS:
				{
				    int k; 
				    for (k = 0; k <cmd.listCell.nb; k++)
				    {
					jvs_set_vh(p_statuts, t, cmd.listCell.list[k].x, cmd.listCell.list[k].y);
				    }
				    break;
				}
				case CMD_TASK:
				{
				    /* Récupérer l'etat de la cellule */
				    int k;
				    for (k=0; k<cmd.listCell.nb; k++)
				    {
					p_vie_next->grille[cmd.listCell.list[k].x][cmd.listCell.list[k].y] = cmd.listCell.list[k].cell;
					p_statuts->grille[cmd.listCell.list[k].x][cmd.listCell.list[k].y].statut = TRAITEE; 
				    }
				    break;
				}
				default:
				    break;
				}
				free(cmd.listCell.list);
			    }
			    default:
				break;
			    
			}
		    }
		    break;
		}
	    }
	}
	
	aff_plateau_s(p_vie, p_statuts);

	/* verifier changement generations */
	if (jvs_nextGen(p_statuts) != -1)
	{
	    p_vie = p_vie_next;
	    fintache = 0;
	}

    }
    clear_clients(clients, actual);
    end_connection(sock);
    aff_end();
}

static void clear_clients(Client *clients, int actual)
{
    int i = 0;
    for(i=0; i<actual; i++)
    {
	close(clients[i].sock);
    }
}

static void remove_client(Client *clients, int to_remove, int *actual)
{
    /* remove the client in the array */
    memmove(clients + to_remove, clients + to_remove + 1, (*actual - to_remove - 1) * sizeof(Client));
    /* nymber client -1 */
    (*actual)--;
}

static int init_connection(void)
{
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    SOCKADDR_IN sin = { 0 };

    if(sock == INVALID_SOCKET)
    {
	perror("socket()");
	exit(errno);
    }

    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port = htons(PORT);
    sin.sin_family = AF_INET;

    if(bind(sock,(SOCKADDR *) &sin, sizeof sin) == SOCKET_ERROR)
    {
	perror("bind()");
	exit(errno);
    }

    if(listen(sock, MAX_CLIENTS) == SOCKET_ERROR)
    {
	perror("listen()");
	exit(errno);
    }
    return sock;
}

static void end_connection(int sock)
{
    close(sock);
}

int main(int argc, char **argv)
{
    app();
    return EXIT_SUCCESS;
}
