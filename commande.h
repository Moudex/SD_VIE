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

struct CmdStartCommunication
{
    long iteration;
};
typedef struct CmdStartCommunication CmdStartCommunication;

struct CmdRequestTask
{
};
typedef struct CmdRequestTask CmdRequestTask;

struct CmdNoTask
{
    long waitingTime;
};
typedef struct CmdNoTask CmdNoTask;

struct CmdTask
{
    int width;
    int height;
    char* cells;
};
typedef struct CmdTask CmdTask;

struct CmdEndCommunication
{
};
typedef struct CmdEndCommunication CmdEndCommunication;

enum CommandType {
    CMD_START_COMMUNICATION = 1,
    CMD_REQUEST_TASK = 2,
    CMD_NO_TASK = 3,
    CMD_TASK = 4,
    CMD_END_COMMUNICATION = 5
};
typedef enum CommandType CommandType;

struct Command
{
    CommandType type;
    union
    {
	CmdStartCommunication	startCom;
	CmdRequestTask		request;
	CmdNoTask		noTask;
	CmdTask			task;
	CmdEndCommunication	endCom;
    };
};
typedef struct Command Command;


int writeCmd(int socket, Command* cmd);

int readCmd(int socket, Command* cmd);

#endif
