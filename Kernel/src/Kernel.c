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
#include "KernelConfiguration.h"


int main(int argc, char** argsv) {
	puts("!!!Hello Kernel!!!\n"); /* prints !!!Hello World!!! */
	configKernel("/home/utnso/Escritorio/tp-2017-1c-The-Kernels/Kernel/Debug/config.conf");
	handler();
	puts("Todo ok\n");
	return 0;
}

/*
 * Por lo que estoy pensando se requeriría id de proceso Y id de operacion
 * porque uno poodría querer manejar de cierta manera una operacion por un proceso
 * y de otra manera distinta una operacion por otro proceso
 *
 */

