/*
 ============================================================================
 Name        : ClientePrueba.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

#include "SocketLibrary.h"

#define LOCALHOST "127.0.0.1"
#define PUERTO "7171"

int main(void) {

	int s= getConnectedSocket(LOCALHOST,PUERTO);
	void* msg= "Me conecté";
	lSend(s,msg,sizeof(msg));

	return EXIT_SUCCESS;
}
