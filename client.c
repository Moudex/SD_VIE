#include "client.h"
#include "jvie.h"
#include "commande.h"

static void init(void)
{
#ifdef WIN32
    WSADATA wsa;
    int err = WSAStartup(MAKEWORD(2, 2), &wsa);
    if(err<0)
    {
	puts("WSAStartup failed !");
	exit(EXIT_FAILURE);
    }
#endif
}

static void end(void)
{
#ifdef WIN32
    WSACleanup();
#endif
}

static void app(const char *address)
{
    SOCKET sock = init_connection(address);
    fd_set rdfs;
    etat_c et = ENVOYE;

    /* A changer avec cmdStartCom */
    Command* c  = (Command*)malloc(sizeof(Command));
    c->type = CMD_START_COMMUNICATION;
    int start = writeCmd(sock, c);

	while(start)
	{
	    FD_ZERO(&rdfs);
	    FD_SET(STDIN_FILENO, &rdfs);
	    FD_SET(sock, &rdfs);

	    if(select(sock + 1, &rdfs, NULL, NULL, NULL) == -1)
	    {
		perror("select()");
		exit(errno);
	    }
	    
	    /* Demander un calcul */
	    if (et == ENVOYE)
	    {
		Command* cmd = (Command*)malloc(sizeof(Command));
		cmd->type = CMD_REQUEST_TASK;
		int n = writeCmd(sock, cmd);
		if (n > 0) et = DEMANDE;
		free(cmd);
	    }

	    /* Recois un calcul */
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
		    et = ENVOYE;
		}

		/* recois calculs, le fait et l'envoi */
		else if (cmd->type == CMD_TASK)
		{
		    jv_nextGen2(cmd->task.width, cmd->task.height, cmd->task.cells);
		    writeCmd(sock, cmd);
		    et = ENVOYE;
		}

		else 
		{
		    et = ENVOYE;
		}

	    }
	}

    /* ajouter cmdEndCom */
    c->type = CMD_END_COMMUNICATION;
    writeCmd(sock, c);
    end_connection(sock);
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
    if(argc<2)
    {
	printf("Usage : %s [address]\n", argv[0]);
	return EXIT_FAILURE;
    }

    init();
    app(argv[1]);
    end();
    return EXIT_SUCCESS;
}
