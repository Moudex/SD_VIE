#ifndef COMMANDE_H
#define COMMANDE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef enum
{
    CMD_START_COMMUNICATION = 1,
    CMD_REQUEST_TASK = 2,
    CMD_NO_TASK = 3,
    CMD_TASK = 4,
    CMD_END_COMMUNICATION = 5,
    CMD_LIST_CELL = 6,
    CMD_HEAL = 7,
    CMD_VIRUS = 8
} CommandType;

typedef struct
{
    int width;
    int height;
} CmdHeal;

typedef struct
{
    int width;
    int height;
} CmdVirus;

typedef struct
{
    int x;
    int y;
    char cell;
} coord;

typedef struct
{
    CommandType type;
    int nb;
    coord* list;
} CmdListCell;

typedef struct
{
    long iteration;
} CmdStartCommunication;

typedef struct
{
} CmdRequestTask;

typedef struct
{
    long waitingTime;
} CmdNoTask;

typedef struct
{
    int width;
    int height;
    char* cells;
} CmdTask;

typedef struct
{
} CmdEndCommunication;

typedef struct
{
    CommandType type;
    union
    {
	CmdStartCommunication	startCom;
	CmdRequestTask		request;
	CmdNoTask		noTask;
	CmdTask			task;
	CmdEndCommunication	endCom;
	CmdListCell		listCell;
	CmdHeal			heal;
	CmdVirus		virus;
    };
} Command;


int writeCmd(int socket, Command* cmd);

int readCmd(int socket, Command* cmd);

#endif
