#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "jvie.h"
#include "commande.h"
#include "affichage.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/select.h>
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define h_addr h_addr_list[0]
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;

#define CRLF "\r\n"
#define PORT 2014
#define BUF_SIZE 1024

typedef enum etat_c etat_c;
enum etat_c { DEMANDE, ENVOYE, RECU, REFU };

static void app(const char *address);
static int init_connection(const char *address);
static void end_connection(int sock);

#endif
