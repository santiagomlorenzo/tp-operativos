#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/config.h>
#include "Configuration.h"
#include "SocketLibrary.h"
#define CONFIG_FILE "FileSistem.conf"

const char* keys[3] = {"PUERTO", "PUNTO_MONTAJE" , "NULL"};

typedef struct {
	char puerto[5];
	char punto_montaje [15];
} configFile;

configFile* leerArchivoConfig(t_config* configHandler)
{
	configFile* config= malloc(sizeof(configFile));
	strcpy(config->puerto, config_get_string_value(configHandler, "PUERTO"));
	strcpy(config->punto_montaje,config_get_string_value(configHandler, "PUNTO_MONTAJE"));
	config_destroy(configHandler);
	return config;
}

void imprimirConfig(configFile* config)
{
	puts("--------PROCESO FILESISTEM--------");
	printf("ESCUCHANDO EN PUERTO: %s | PUNTO_MONTAJE %s\n", config->puerto, config->punto_montaje);
	puts("--------PROCESO FILESISTEM--------");
}

int main(int argc, char** argsv) {
	configFile* config;
	config = configurate("/home/utnso/tp-2017-1c-The-Kernels/FileSistem/Debug/FileSistem.conf", leerArchivoConfig, keys);
		int socket = getBindedSocket("127.0.0.75", config->puerto);
		free(config);
		lListen(socket, 5);
		int conexion = lAccept(socket);
		if(recibirHandShake(conexion) != 0){
			close(conexion);
			puts("Proceso equivocado");
		}
		else{
			int* confirmacion = malloc(sizeof(int));
			*confirmacion = 1;
			lSend(conexion, 0, confirmacion, sizeof(confirmacion));
			free(confirmacion);
			int operacion;
			char* mensaje = lRecv(conexion, &operacion);
			printf("MENSAJE: %s", mensaje);
			free(mensaje);
		}
	return EXIT_SUCCESS;
}
