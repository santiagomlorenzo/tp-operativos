#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>
#include <netdb.h>

#include "Header.h"
#include "InternalSocketFunctions.h"

int lAccept(int);
int getBindedSocket(char*, char*);
int isReading(int, socketHandler*);
int isWriting(int, socketHandler*);
int getConnectedSocket(char*, char*);
//-----------------------------------------------------//
void lListen(int,int);
void lRecv(int, void*);
void clrReaders(socketHandler*);
void clrWriters(socketHandler*);
void clrHandler(socketHandler*);
void lSend(int, const void*, int);
void addReadSocket(int, socketHandler*);
void rmvReadSocket(int, socketHandler*);
void addWriteSocket(int, socketHandler*);
void rmvWriteSocket(int, socketHandler*);
void handleResults(int,int,socketHandler*,socketHandler*);//agregarle una lista al Handler que tenga los listeners

//-----------------------------------------------------//
socketHandler* lSelect(socketHandler*, int);



