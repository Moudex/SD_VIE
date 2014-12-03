#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include "commande.h"
#include "jvie.h"
#include "jvie_serv.h"

#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;

#define CRLF "\r\n"
#define PORT 2014
#define MAX_CLIENTS 100
#define BLOCK_SIZE 5
#define GAME_SIZE 64

typedef struct
{
    SOCKET sock;
    int generation;
    int x;
    int y;
    int width;
    int height;
}Client;

static void app(void);
static int init_connection(void);
static void end_connection(int sock);
static void remove_client(Client *clients, int to_remove, int *actual);
static void clear_clients(Client *clients, int actual);

#endif
