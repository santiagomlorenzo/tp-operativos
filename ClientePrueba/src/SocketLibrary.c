#include "SocketLibrary.h"


/*int getSocket(char* ip, char* port){
	return internalSocket(ip,port,*empty);
}
//Está para ser habilitado solo en caso de que por alguna razón uno necesite un socket que no bindea o conecta
void empty(int a, sockAddr b, int c){}*/


//-----------------------------------------MAIN FUNCTIONS-------------------------------------------------------------------//

int getBindedSocket(char* ip, char* port){
	int(*action)(int,const struct sockaddr*,socklen_t)=&bind;
	return internalSocket(ip,port,action);
}

int getConnectedSocket(char* ip, char* port){
	int(*action)(int,const struct sockaddr*,socklen_t)=&connect;
	return internalSocket(ip,port,action);
}

void lListen(int socket,int backlog){//hay que cambiarle el nombre
	errorIfEqual(listen(socket,backlog),-1,"Listen");//cantidad de conexiones que acepta, osea de sockets que voy a manejar
}

int lAccept(int sockListener){
	int newSocket;
	struct sockaddr* addr;
	errorIfEqual(newSocket= accept(sockListener,addr,sizeof(struct sockaddr)),-1,"accept");
	return newSocket;
}

void lRecv(int reciever, void* buf){
	Header* header;
	internalRecv(reciever,header,sizeof(Header));
	buf=malloc(header->tamanio);
	internalRecv(reciever,buf,header->tamanio);
}

void lSend(int sender, const void* msg, int len){
	_sendHeader(sender,1,len);//tipo de proceso hardcodeado, hay que ver de donde pija se saca
	internalSend(sender,msg,len);
}

void handleResults(int listener, int listener2,socketHandler* list, socketHandler* result){//agregarle una lista al Handler que tenga los listeners
	int p;
	void* data;
	for(p=0;p<list->nfds;p++){
		if(isReading(p,result)){
			if(p==listener || p==listener2){
				addReadSocket(lAccept(p),list);
				printf("new connection");
			}
			else{
				lRecv(p,data);
				printf("Data Recibida: %s",*((char*) data));
			}
		free(data);
		}
	}
}

socketHandler* lSelect(socketHandler* handler, int duration){
	timeVal time= _setTimeVal(duration,0);
	int status= select(handler->nfds,handler->readSockets,handler->writeSockets,NULL,&time);
	errorIfEqual(status,-1,"select");
	return handler;
}

//---------------------------------------------------------------------------------------------//

void addReadSocket(int reader,socketHandler* handler){
	if(handler->nfds-1<reader)handler->nfds=reader+1;
	FD_SET(reader,handler->readSockets);
}

void addWriteSocket(int writer, socketHandler* handler){
	if(handler->nfds-1<writer)handler->nfds=writer+1;
	FD_SET(writer,handler->writeSockets);
}

void rmvReadSocket(int reader, socketHandler* handler){
	FD_CLR(reader,handler->readSockets);
}

void rmvWriteSocket(int writer, socketHandler* handler){
	FD_CLR(writer,handler->writeSockets);
}

void clrReaders(socketHandler* handler){
	FD_ZERO(handler->readSockets);
}

void clrWriters(socketHandler* handler){
	FD_ZERO(handler->writeSockets);
}

void clrHandler(socketHandler* handler){
	clrReaders(handler);
	clrWriters(handler);
	handler->nfds=0;
}

int isReading(int reader, socketHandler* handler){
	return FD_ISSET(reader,handler->readSockets);
}

int isWriting(int writer, socketHandler* handler){
	return FD_ISSET(writer,handler->writeSockets);
}



