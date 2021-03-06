#ifndef __KERNEL_CONF__
#define __KERNEL_CONF__
#include "Configuration.h"

typedef struct {
	char PUERTO_PROG[5];
	char PUERTO_CPU[5];
	char IP_MEMORIA[16];
	char PUERTO_MEMORIA[5];
	char IP_FS[16];
	char PUERTO_FS[5];
	int QUANTUM;
	int QUANTUM_SLEEP;
	char ALGORITMO[4];
	int GRADO_MULTIPROG;
	int STACK_SIZE;
	int PAG_SIZE;
	char** SEM_IDS;
	char** SEM_INIT;
	char** SHARED_VARS;
	char IP_PROPIA[16];
	char log[100];

} configFile;

bool configKernel(char*);
void handleConfigFile(t_config*);
configFile* readConfigFile(t_config* configHandler);
void destruirConfig(configFile*);
#endif
