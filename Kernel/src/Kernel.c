/*
 ============================================================================
 Name        : Kernel.c
 Author      : Santiago M. Lorenzo
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "Listen.h"
#include "globales.h"
#include "tests/KernelTest.h"
#include "KernelConfiguration.h"
#include "ConsolaKernel.h"
const char* keys[15] = {"PUERTO_PROG", "PUERTO_CPU", "IP_MEMORIA", "PUERTO_MEMORIA", "IP_FS", "PUERTO_FS", "QUANTUM", "QUANTUM_SLEEP", "ALGORITMO", "GRADO_MULTIPROG", "SEM_IDS", "SEM_INIT", "SHARED_VARS", "STACK_SIZE", "NULL"};


int main(int argc, char** argsv) {

	puts("!!!Hello Kernel!!!\n"); /* prints !!!Hello World!!! */
	config = configurate("/home/utnso/Escritorio/tp-2017-1c-The-Kernels/Kernel/Debug/config.conf", readConfigFile, keys);
	//config = configurate("/home/utnso/workspace/tp-2017-1c-The-Kernels/Kernel/Debug/config.conf", readConfigFile, keys);
	if(argc > 1 && strcmp(argsv[1],"-test")==0){
		test=1;
		kernelTest(1);
		return EXIT_SUCCESS;
	}
	test=0;
	morir =0;

	process= list_create();

	colaCPUS = queue_create();

	colaNew = queue_create();
	colaReady = queue_create();
	blockedList = list_create();
	executeList = list_create();
	colaFinished=queue_create();
	tablaGlobalFS = list_create();
	tablasDeProcesosFS = list_create();
	crearListaDeColasSemaforos();
	pthread_create(&consolaKernel, NULL, (void*) recibir_comandos, NULL);
	handler();
	pthread_join(consolaKernel, NULL);
	morirDecentemente();
	// EN ALGUN MOMENTO DESTRUIR ESAS COLAS
	//list_clean_and_destroy_elements(destruirPCB);
	return 0;
}

void morirDecentemente()
{
	destruirConfig(config);
	// TO DO
}
