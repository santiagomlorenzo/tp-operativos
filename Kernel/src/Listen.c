#include "Listen.h"

void handler(){
	socketHandler* sHandlerMaster;
	//socketHandler sHandlerControl;
	socketHandler* sHandlerResult;
	int listener= getBindedSocket(LOCALHOST,PUERTO);
	int listener2= getBindedSocket(LOCALHOST,PUERTO2);
	lListen(listener,BACKLOG);
	lListen(listener2,BACKLOG);
	addReadSocket(listener,sHandlerMaster);
	addReadSocket(listener2,sHandlerMaster);
	while(1){
		puts("Entro al while 1");
		sHandlerResult= lSelect(sHandlerMaster,2);
		handleResults(listener,listener2,sHandlerMaster,sHandlerResult);
	}
}
