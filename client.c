#include "client.h"
#include <time.h>

static void app(const char *address)
{
    SOCKET sock = init_connection(address);
    fd_set rdfs;
    etat_c et = REFU;
    aff_init();
    srand(time(NULL));

    /* Envoi de la commande pour démarrer communication */
    Command* c  = (Command*)malloc(sizeof(Command));
    c->type = CMD_START_COMMUNICATION;
    int start = writeCmd(sock, c);

	while(1)
	{
	    FD_ZERO(&rdfs);
	    FD_SET(sock, &rdfs);

	    /* Si calcul envoye, on en demande un autre */
	    if (et == ENVOYE)
	    {
		Command* cmd = (Command*)malloc(sizeof(Command));
		cmd->type = CMD_REQUEST_TASK;
		int n = writeCmd(sock, cmd);
		if (n > 0) et = DEMANDE;
		free(cmd);
	    }

	    if(select(sock + 1, &rdfs, NULL, NULL, NULL) == -1)
	    {
		perror("select()");
		exit(errno);
	    }

	    /* Si pas encore accepte */
	    if (et == REFU)
	    {
		Command cmd;
		readCmd(sock, &cmd);
		if (cmd.type == CMD_START_COMMUNICATION)
		    et = ENVOYE;
	    }


	    /* Recois une commande */
	    else if (et == DEMANDE && FD_ISSET(sock, &rdfs))
	    {
		Command* cmd = (Command*)malloc(sizeof(Command));
		int n = readCmd(sock, cmd);
		if (n == 0)
		{
		    printf("Server disconnected !\n");
		    break;
		}
		
		/* recois ordre d'attente */
		if (cmd->type == CMD_NO_TASK)
		{
		    sleep(cmd->noTask.waitingTime);
		}

		/* recois calculs, le fait et l'envoi */
		else if (cmd->type == CMD_TASK)
		{
		    plateau* p = jv_unpack_c(cmd->task.cells, cmd->task.width, cmd->task.height);
		    jv_nextGen(p);
		    aff_plateau_c(p);
		    char* coucou = (char*)malloc(sizeof(char)*cmd->task.width*cmd->task.height);
		    jv_pack_c(p, coucou);
		    free(cmd->task.cells);
		    cmd->task.cells = coucou;
		    writeCmd(sock, cmd);
		    free(cmd->task.cells);
		}

		else if (cmd->type == CMD_HEAL)
		{
		    int x = rand()% cmd->heal.width;
		    int y = rand()% cmd->heal.height;
		    Command c;
		    c.type = CMD_LIST_CELL;
		    c.listCell.type = CMD_HEAL;
		    c.listCell.nb = 1;
		    c.listCell.list = (coord*)malloc(sizeof(coord));
		    c.listCell.list[0].x = x;
		    c.listCell.list[0].y = y;
		    writeCmd(sock, &c);
		    free(c.listCell.list);
		}

		else if (cmd->type == CMD_VIRUS)
		{
		    /* TODO générer les diaginales */
		    int x = rand()% cmd->heal.width;
		    int y = rand()% cmd->heal.height;
		    Command c;
		    c.type = CMD_LIST_CELL;
		    c.listCell.type = CMD_VIRUS;
		    c.listCell.nb = 1;
		    c.listCell.list = (coord*)malloc(sizeof(coord));
		    c.listCell.list[0].x = x;
		    c.listCell.list[0].y = y;
		    writeCmd(sock, &c);
		    free(c.listCell.list);
		}

		else if (cmd->type == CMD_LIST_CELL)
		{
		    if(cmd->listCell.type == VIRUS)
			cmd->listCell.list[0].cell = 0;
		    else
			cmd->listCell.list[0].cell = 1- cmd->listCell.list[0].cell;
		    cmd->listCell.type = CMD_TASK;
		    writeCmd(sock, cmd);
		}

		else 
		{
/*		    Command q;
		    readCmd(sock, &q);
		    printf("erreur cmd %d\n", cmd->type);
*/
		}
		free(cmd);
		et = ENVOYE;
	    }
	}

    /* Envoi de la commande de terminaison de communication */
    c->type = CMD_END_COMMUNICATION;
    writeCmd(sock, c);
    free(c);
    end_connection(sock);
    aff_end();
}

static int init_connection(const char *address)
{
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    SOCKADDR_IN sin = { 0 };
    struct hostent *hostinfo;

    if(sock == INVALID_SOCKET)
    {
	perror("socket()");
	exit(errno);
    }

    hostinfo = gethostbyname(address);
    if(hostinfo == NULL)
    {
	fprintf(stderr, "Unknown host %s.\n", address);
	exit(EXIT_FAILURE);
    }

    sin.sin_addr = *(IN_ADDR *) hostinfo->h_addr;
    sin.sin_port = htons(PORT);
    sin.sin_family = AF_INET;

    if(connect(sock,(SOCKADDR *) &sin, sizeof(SOCKADDR)) == SOCKET_ERROR)
    {
	perror("connect()");
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
    /*if(argc<2)
    {
	printf("Usage : %s [address]\n", argv[0]);
	return EXIT_FAILURE;
    }

    app(argv[1]);
*/ app("127.0.0.1");
    return EXIT_SUCCESS;
}
