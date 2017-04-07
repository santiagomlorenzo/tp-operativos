#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>

typedef struct sockadrr sockAddr;
typedef struct addrinfo addrInfo;
typedef struct timeval timeVal;

typedef struct socketHandler{
	fd_set* readSockets;
	fd_set* writeSockets;
	int nfds;
}socketHandler;

int internalSocket(char*, char*,int (int,const struct sockaddr *,socklen_t));
void internalRecv(int, void*, int);
void internalSend(int, void*, int);
void _errorIf(int (int,int), int, int, char*);
struct addrinfo _getaddrinfocall(char*, char*);
int _isEqual(int, int);
int _isNotEqual(int, int);
void errorIfEqual(int, int, char*);
void errorIfNotEqual(int, int, char*);
void _sendHeader(int,int,int);
Header _createHeader(int,int);
timeVal _setTimeVal(int, int);
int _getFirstSocket(addrInfo*, int (int,const struct sockaddr *,socklen_t));
