#ifndef SERVER_H
#define SERVER_H

#ifdef WIN32

#include <winsock2.h>
#define errno WSAGetLastError()
#define close(s) closesocket(s)

#elif defined (linux)

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;

#else

#error not defined for this platform

#endif

#define CRLF "\r\n"
#define PORT 2014
#define MAX_CLIENTS 100
#define BUF_SIZE 1024

typedef struct
{
    SOCKET sock;
    int generation;
    int x;
    int y;
    int width;
    int height;
}Client;

static void init(void);
static void end(void);
static void app(void);
static int init_connection(void);
static void end_connection(int sock);
static void send_message_to_all_clients(Client *clients, Client client, int actual, const char *buffer, char from_server);
static void remove_client(Client *clients, int to_remove, int *actual);
static void clear_clients(Client *clients, int actual);

#endif
