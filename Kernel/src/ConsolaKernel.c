#include "ConsolaKernel.h"

void recibir_comandos()
{
	while(morir == 0)
	{
		//0-> new, 1->ready, 2->execute, 3-> blocked, 4-> suspended, 9-> killed,
		char* entrada = leerCaracteresEntrantes();
		char** comando = string_split(entrada, " ");
		printf("COMANDO: %s ARG: %s\n", comando[0], comando[1]);
		if(!strcmp(comando[0], "cola"))
		{
			if(comando[1] == NULL)
			{
				puts("-------TODOS LOS PROCESOS (INCLUYE EXIT)------");
				list_iterate(process, mostrarPorConsola);
				puts("-------TODOS LOS PROCESOS (INCLUYE EXIT)------");
			}
			else if(!strcmp(comando[1], "new"))
			{
				puts("-------COLA NEW------");
				mostrarProcesosEnEstado(0);
				puts("-------COLA NEW------");
			}
			else if(!strcmp(comando[1], "ready"))
			{
				puts("-------COLA READY------");
				list_iterate(colaReady->elements, mostrarPID);
				puts("-------COLA READY------");
			}
			else if(!strcmp(comando[1], "execute"))
			{
				puts("-------COLA EXECUTE------");
				list_iterate(executeList, mostrarPID);
				puts("-------COLA EXECUTE------");
			}
			else if(!strcmp(comando[1], "blocked"))
			{
				puts("-------COLA BLOCKED------");
				list_iterate(blockedList, mostrarPID);
				puts("-------COLA BLOCKED------");
			}
			else if(!strcmp(comando[1], "exit"))
			{
				puts("-------COLA EXIT------");
				list_iterate(colaFinished->elements, mostrarPID);
				puts("-------COLA EXIT------");
			}
		}
		// FALTAN VARIOS CMDS QUE TODAVIA NO ESTA IMPLEMENTADO DONDE SE GUARDA ESA INFO
		else if(!strcmp(comando[0], "tablaArchivos"))
		{
			if(!strcmp(comando[1], "global"))
			{
				puts("-------TABLA DE ARCHIVOS GLOBAL-------");
				mostrarTablaDeArchivosGlobal();
				puts("-------TABLA DE ARCHIVOS GLOBAL-------");
			}
			else
			{
				int pid = atoi(comando[1]);
				if(pid > maxPID | pid < 0)
					puts("PID INVALIDO");
				else
				{
					printf("-------TABLA DE ARCHIVOS PID: %i-------\n", pid);
					mostrarTablaDeArchivosProceso(pid);
					printf("-------TABLA DE ARCHIVOS PID: %i-------\n", pid);
				}
			}
		}
		else if(!strcmp(comando[0], "multiprog"))
		{
			int multiprog = atoi(comando[1]);
			if(multiprog <= 0)
				puts("NO ES NU NUMERO VALIDO");
			else
			{
				pthread_mutex_lock(&mMultiprog);
				config->GRADO_MULTIPROG = multiprog;
				pthread_mutex_unlock(&mMultiprog);
				printf("NUEVO GRADO DE MULTIPROGRAMACION: %i\n", config->GRADO_MULTIPROG);
				int i;
				pthread_mutex_lock(&mColaNew);
				int new = queue_size(colaNew);
				pthread_mutex_unlock(&mColaNew);
				for(i = 0;i<new;i++)
					readyProcess();

			}
		}
		else if(!strcmp(comando[0], "kill"))
		{
			int pid = atoi(comando[1]);
			matarCuandoCorresponda(pid);
			printf("SE ENVIO A AJUSTICIAR EL PROCESO PID: %i. SE AJUSTICIARA CUANDO CORRESPONDA.\n", pid);
		}
		else if(!strcmp(comando[0], "togglePlanif"))
		{
			// NI PUTA IDEA YET
		}
		else if(!strcmp(comando[0], "exit"))
		{
			morir = 1;
		}
		else
			puts("COMANDO INVALIDO");
		free(comando[0]);
		free(comando[1]);
		free(comando);
		free(entrada);
	}

}

void mostrarPID(PCB* pcb)
{
	printf("PID: %i\n", pcb->pid);
}

char* leerCaracteresEntrantes() {
	int i, caracterLeido;
	char* cadena = malloc(30);
	for(i = 0; (caracterLeido= getchar()) != '\n'; i++)
		cadena[i] = caracterLeido;
	cadena[i] = '\0';
	return cadena;
}

void mostrarTablaDeArchivosProceso(int pid)
{
	tablaDeProceso* tabla = encontrarTablaDelProceso(pid);
	if(tabla == NULL)
		puts("NO TIENE ARCHIVOS");
	else
		list_iterate(tabla, mostrarEntradaTablaArchivoProceso);
}

void mostrarEntradaTablaArchivoProceso(entradaTablaFSProceso* entrada)
{
	puts("| CURSOR | FLAGS | APUNTA A (RUTA) |");
	printf("| %i | %s | %s |\n", entrada->cursor, entrada->flags, entrada->entradaGlobal->ruta);
}

void mostrarTablaDeArchivosGlobal()
{
	list_iterate(tablaGlobalFS,  mostrarEntradaTablaGlobalFS);
}

void mostrarEntradaTablaGlobalFS(entradaTablaGlobalFS* entrada)
{
	puts("| RUTA | INSTANCIAS |");
	printf("| %s | %i |\n",entrada->ruta, entrada->instancias);
}

void mostrarProcesosEnEstado(int estado)
{
	bool mismoEstado(ProcessControl* pc)
	{
		return estado == pc->state;
	}
	t_list* filtrados = list_filter(process, mismoEstado);
	list_iterate(filtrados, mostrarPorConsola);
}

void mostrarPorConsola(ProcessControl* pc)
{
	printf("PID %i | ESTADO: %i\n", pc->pid, pc->state);
}
