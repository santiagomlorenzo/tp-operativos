#include "Listen.h"

socketHandler a();

void handler(){
	socketHandler sHandlerMaster= a();
	//socketHandler sHandlerControl;
	socketHandler sHandlerResult=a();
	int listener= getBindedSocket(LOCALHOST,PUERTO);
	lListen(listener,BACKLOG);
	addReadSocket(listener,&sHandlerMaster);
	while(1){
		puts("Entro al while 1");
		sHandlerResult= lSelect(&sHandlerMaster,15);
		puts("El lSelect parece haber funcionado\n");
		handleResults(listener,&sHandlerMaster,&sHandlerResult);
		puts("El handleResults parece haber funcionado\n");
	}
		free(sHandlerMaster.readSockets);
		free(sHandlerMaster.writeSockets);
		free(sHandlerResult.readSockets);
		free(sHandlerResult.writeSockets);
}

/*void handler(){
	socketHandler sHandlerMaster= a();
	//socketHandler sHandlerControl;
	socketHandler* sHandlerResult;
	int listener= getBindedSocket(LOCALHOST,PUERTO);
	lListen(listener,BACKLOG);
	addReadSocket(listener,&sHandlerMaster);
	void* msg;
	while(1){
		puts("Entro al while 1");
		int newSocket= lAccept(listener);
		printf("Socket: %d",newSocket);
		if(newSocket<0){puts("Accept Error");}
		lRecv(newSocket,msg);
		//printf("%s",((char*)msg));
	}
		free(sHandlerMaster.readSockets);
		free(sHandlerMaster.writeSockets);
}*/


socketHandler a(){
	socketHandler b;
	b.nfds=0;
	b.readSockets=malloc(250*sizeof(fd_set));
	b.writeSockets=malloc(250*sizeof(fd_set));
	return b;
}

/*void lRecv2(int reciever, char a[]){
	a=malloc(sizeof(Header));
		internalRecv2(reciever,a,sizeof(Header));
}

void internalRecv2(int reciever, char buf[], int size){
	int status;
	printf("%d",status= recv(reciever,buf,size,0));
	errorIfEqual(status,0,"Connection Closed");
}*/



//sHandlerResult= lSelect(sHandlerMaster,2);
		//handleResults(listener,listener2,sHandlerMaster,sHandlerResult);
		/*struct sockaddr_storage* addr;
		int size= sizeof(struct sockaddr);
		int newSocket= accept(listener,&addr,&(size));
		recv(newSocket,msg,5*sizeof(char),0);

		buf=malloc(5*sizeof(char));
		internalRecv(reciever,buf,5*sizeof(char));

		printf("%s",msg);
		*/
