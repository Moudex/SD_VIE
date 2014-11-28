#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "server.h"
#include "commande.h"
#include "jvie.h"

static void init(void)
{
#ifdef WIN32
    WSDATA wsa;
    int err = WSAStartup(MAKEWORD(2, 2), &wsa);
    if(err < 0)
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

static void app(void)
{
    SOCKET sock = init_connection();
    char buffer[BUF_SIZE];
    int actual = 0;
    int max = sock;
    Client clients[MAX_CLIENTS];

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
			    /* Demande une tache */
			    case CMD_REQUEST_TASK:

				break;

			    /* Envoi un resultat */
			    case CMD_TASK:

				break;

			    default:

			}
		    }
		    break;
		}
	    }
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
    init();
    app();
    end();
    return EXIT_SUCCESS;
}
