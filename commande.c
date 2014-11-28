#include "commande.h"


/* Lis une commande */
int readCmd(int socket, Command* cmd)
{
    int n;
    memset(cmd, 0, sizeof(Command));

    n = read(socket, &(cmd->type), sizeof(CommandType));
    if (n <= 0) return -1;

    switch(cmd->type)
    {
	case CMD_START_COMMUNICATION:
	    n += read(socket, &(cmd->startCom), sizeof(CmdStartCommunication));
	    break;
	case CMD_REQUEST_TASK:
	    n += read(socket, &(cmd->request), sizeof(CmdRequestTask));
	    break;
	case CMD_NO_TASK:
	    n += read(socket, &(cmd->noTask), sizeof(CmdNoTask));
	    break;
	case CMD_TASK:
	    {
		int n2 = read(socket, &(cmd->task), sizeof(CmdTask) - sizeof(char*));
		if (n2 <= 0) return -1;
		const CSIZ = cmd->task.width * cmd->task.height;
		cmd->task.cells = (char*)malloc(CSIZ * sizeof(char));
		for (int i = 0; i < CSIZ; i++)
		{
		    n2 = read(socket, cmd->task.cells + i, sizeof(char));
		    n += n2;

		    if (n2 <= 0)
		    {
			free(cmd->task.cells);
			cmd->task.width = 0;
			cmd->task.height = 0;
			return -1;
		    }
		}
		break;
	    }
	case CMD_END_COMMUNICATION:
		n += read(socket, &(cmd->endCom), sizeof(CmdEndCommunication));
		break;
	default:
	    return -1;
    }
    return n;
}

int writeCmd(int socket, Command* cmd)
{
    int n;
    n = write(socket, &(cmd->type), sizeof(CommandType));
    if(n <= 0) return -1;
    
    switch(cmd->type)
    {
	case CMD_START_COMMUNICATION:
	    n += write(socket, &(cmd->startCom), sizeof(CmdStartCommunication));
	    break;
	case CMD_REQUEST_TASK:
	    n += write(socket, &(cmd->request), sizeof(CmdRequestTask));
	    break;
	case CMD_NO_TASK:
	    n += write(socket, &(cmd->noTask), sizeof(CmdNoTask));
	    break;
	case CMD_TASK:
	    {
	    int n2 = write(socket, &(cmd->task), sizeof(CmdTask) - sizeof(char*));
	    if (n2 <= 0) return -1;
	    n += n2;

	    const int CSIZ = cmd->task.width * cmd->task.height;
	    for (int i = 0; i < CSIZ; i++)
	    {
		n2 = write(socket, cmd->task.cells + i, sizeof(char));
		if (n2 <= 0) return -1;
		n += n2;
	    }
	    break;
	    }
	case CMD_END_COMMUNICATION:
	    n += write(socket, &(cmd->endCom), sizeof(CmdEndCommunication));
	    break;
	default:
	    return -1;
    }
    return n;
}
