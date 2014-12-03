#include "server.h"

static void app(void)
{
    SOCKET sock = init_connection();
    int actual = 0;
    int max = sock;
    Client clients[MAX_CLIENTS];
    
    /* Initialiser la plateau de jeu de la vie
     * et son plateau d'etats
     */
    
    plateau* p_vie = jv_newPlat(GAME_SIZE, GAME_SIZE);
    plateau* p_vie_next = jv_newPlat(GAME_SIZE, GAME_SIZE);
    PlateauStatut* p_statuts = jvs_newPlat(GAME_SIZE, GAME_SIZE);

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
	    
	    writeCmd(csock, &cmd);
	    max = csock > max ? csock : max;

	    FD_SET(csock, &rdfs);
	    Client c;
	    c.sock = csock;
	    c.generation = -1;
	    c.x = -1;
	    c.y = -1;
	    c.width = -1;
	    c.height = -1;
	    clients[actual] = c;
	    actual ++;
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
			    	com.type = CMD_TASK;
			    	
			    	/* TODO verifier si le process n'est pas déja sur un block non traité ou en traitement */
			    	
				//printf("%d demande un calcul\n", i);

			    	Block b = jvs_getBlock(p_statuts, BLOCK_SIZE, BLOCK_SIZE);
			    	if (b.x == -1)
			    	{
			    		/* erreur, aucun block libre */
					Command att;
					att.type = CMD_NO_TASK;
					att.noTask.waitingTime = 5.0;
					writeCmd(clients[i].sock, &att);
			    		break;
			    	}
			    	 /* pack des cellules */
				 char* pack = jv_pack_s(p_vie, b.x, b.y, b.width+2, b.height+2);
			    	 /* Construction et envoi de la commande */
			    	 com.task.width = b.width+2;
			    	 com.task.height = b.height+2;
			    	 com.task.cells = pack;
			    	 if (writeCmd(clients[i].sock, &com)>0)
				 {
				     //printf("Calcul envoye\n");
				    /* Assigne le block au client */
				     printf("x=%d y=%d w=%d h=%d\n", b.x, b.y, b.width, b.height);
				   jvs_assigne(p_statuts, b.x, b.y, b.width, b.height, clients[i].sock);
				   clients[i].generation = p_statuts->generation;
				   clients[i].x = b.x; clients[i].y = b.y; clients[i].width = b.width; clients[i].height = b.height;		    	 
				 }
			    	 free(pack);
			    	
				break;

			    /* Client envoi un resultat */
			    case CMD_TASK:
				/* Recevoir les cellules
				 * verifier la génération
				 * Les insérer dans le tableau next gen
				 * libérer l'etat du block
				 */
				if (clients[i].generation != p_statuts->generation)
				{
				    /* enlever le client */
				    close(clients[i].sock);
				    remove_client(clients, i, &actual);
				}
				
				jv_unpack_s(p_vie_next, cmd.task.cells, clients[i].x, clients[i].y, cmd.task.width, cmd.task.height);
				jvs_termine(p_statuts, clients[i].x, clients[i].y, clients[i].width, clients[i].height);

				//printf("%d a termine son calcul avec succes\n", i);
				break;

			    default:
				break;

			}
		    }
		    break;
		}
	    }
	}

	/* verifier changement generations */
	if (jvs_nextGen(p_statuts) != -1)
	{
	    printf("Passe a la generation %d\n", p_statuts->generation);
	    p_vie = p_vie_next;
	}

    }
    clear_clients(clients, actual);
    end_connection(sock);
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
