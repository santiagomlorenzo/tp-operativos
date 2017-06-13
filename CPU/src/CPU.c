/*
 ============================================================================
 Name        : CPU.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include "CPU.h"

int main(int argc, char** argsv) {
	config = configurate("/home/utnso/Escritorio/tp-2017-1c-The-Kernels/CPU/Debug/CPU.conf", leer_archivo_configuracion, keys);
	iniciarConexiones();
	while(1) // EN UN FUTURO ESTO SE CAMBIA POR EL ENVIO DE LA SEÑAL SIGUSR1
	{
		if(esperarPCB() == -1)
		{
			puts("MURIO EL KERNEL");
			break;
		}
		informarAMemoriaDelPIDActual();
		while(finaliza == 0) // HARDCODEADO
		{
			char* linea = pedirInstruccionAMemoria(pcb, tamanioPagina);
			printf("[PEDIR INSTRUCCION NRO %i]: %s", pcb->programCounter, linea);
			analizadorLinea(linea, &primitivas, &primitivas_kernel);
			free(linea);
			pcb->programCounter++;
		}
		serializado pcbSerializado = serializarPCB(pcb);
		lSend(kernel, pcbSerializado.data, 1, pcbSerializado.size);
		free(pcb);
	}
	free(config);
	close(kernel);
	close(memoria);
	return EXIT_SUCCESS;

}

void iniciarConexiones()
{
	kernel = getConnectedSocket(config->ip_Kernel, config->puerto_Kernel, CPU_ID);
	memoria = getConnectedSocket(config->ip_Memoria, config->puerto_Memoria, CPU_ID);
	puts("Esperando Tamaño Paginas");
	Mensaje* tamanioPaginas = lRecv(kernel);
	memcpy(&tamanioPagina, tamanioPaginas->data, sizeof(int));
	Mensaje* algoritmo = lRecv(kernel);
	memcpy(&quantum,algoritmo->data, sizeof(int));
	free(tamanioPaginas);
}

int esperarPCB()
{
	puts("Esperando PCB");
	Mensaje* mensaje = lRecv(kernel);
	if(mensaje->header.tipoOperacion == -1)
		return mensaje->header.tipoOperacion;
	puts("PCB RECIBIDO");
	pcb = deserializarPCB(mensaje->data);
	//pcb->indiceStack = crearIndiceDeStack();// AGUJEROS
	int op = mensaje->header.tipoOperacion;
	destruirMensaje(mensaje);
	return op;
}


void informarAMemoriaDelPIDActual()
{
	char* pid = malloc(sizeof(int));
	memcpy(pid, &pcb->pid, sizeof(int));
	lSend(memoria, pid, 1, sizeof(int));
	free(pid);
}

char* pedirInstruccionAMemoria(PCB* pcb, int tamanioPagina)
{
	int pagina = pcb->indiceCodigo[pcb->programCounter].offset/tamanioPagina;
	int offset = pcb->indiceCodigo[pcb->programCounter].offset%tamanioPagina;
	int size = pcb->indiceCodigo[pcb->programCounter].longitud;
	int total = offset + size;
	int segundoSize = -1;
	// EMPROLIJAR; VER SI SE DELEGA A SOLICITAR BYTES. FALTA HACER LO MISMO PARA STACK
	if(total > tamanioPagina)
	{
		segundoSize = total-tamanioPagina;
		size -= segundoSize;
	}
	printf("[PEDIR INSTRUCCION]: PAG: %i | OFFSET: %i | SIZE: %i\n", pagina, offset, size);
	posicionEnMemoria posicion = obtenerPosicionMemoria(pagina, offset, size);
	char* instruccion = leerEnMemoria(posicion);
	instruccion[size] = '\0';
	if(segundoSize != -1)
	{
		posicionEnMemoria posicion = obtenerPosicionMemoria(pagina+1, 0, segundoSize);
		char* segundaParteInstruccion = leerEnMemoria(posicion);
		segundaParteInstruccion[segundoSize] = '\0';
		printf("PRIM PARTE: %s SEG PARTE: %s\n", instruccion, segundaParteInstruccion);
		string_append(&instruccion, segundaParteInstruccion);
	}
	return instruccion;
}

posicionEnMemoria obtenerPosicionMemoria(int pagina, int offset, int size)
{
	posicionEnMemoria posicion;
	posicion.pagina = pagina;
	posicion.offset= offset;
	posicion.size = size;
	return posicion;
}

configFile* leer_archivo_configuracion(t_config* configHandler){

	configFile* config = malloc(sizeof(configFile));
	strcpy(config->puerto_Kernel, config_get_string_value(configHandler, "PUERTO_KERNEL"));
	strcpy(config->puerto_Memoria, config_get_string_value(configHandler, "PUERTO_MEMORIA"));
	strcpy(config->ip_Kernel, config_get_string_value(configHandler, "IP_KERNEL"));
	strcpy(config->ip_Memoria, config_get_string_value(configHandler, "IP_MEMORIA"));
	config_destroy(configHandler);
	imprimir(config);
	return config;
}

void imprimir(configFile* c){
	puts("--------PROCESO CPU--------");
	printf("IP KERNEL: %s\n", c->ip_Kernel);
	printf("PUERTO KERNEL: %s\n", c->puerto_Kernel);
	printf("IP MEMORIA: %s\n", c->ip_Memoria);
	printf("PUERTO MEMORIA: %s\n", c->puerto_Memoria);
	puts("--------PROCESO CPU--------");

}

// FUNCIONES ANSISOP PARSER
t_puntero definirVariable(t_nombre_variable identificador){
	printf("[DEFINIR VARIABLE - STACK LEVEL: %i]: '%c'\n", pcb->nivelDelStack, identificador);
	posicionEnMemoria unaPosicion = calcularPosicion(pcb->nivelDelStack);
	variable* unaVariable = malloc(sizeof(variable));
	unaVariable->identificador = identificador;
	unaVariable->posicion = unaPosicion;
	if(isdigit(unaVariable->identificador))
		list_add(pcb->indiceStack[pcb->nivelDelStack].argumentos, unaVariable);
	else if(isalpha(unaVariable->identificador))
		list_add(pcb->indiceStack[pcb->nivelDelStack].variables, unaVariable);
	t_puntero direccionReal = convertirADireccionReal(unaVariable->posicion);
	printf("[DEFINIR VARIABLE - STACK LEVEL: %i]: '%c' | PAG: %i | OFFSET: %i | Size: %i:\n", pcb->nivelDelStack, unaVariable->identificador, unaVariable->posicion.pagina, unaVariable->posicion.offset, unaVariable->posicion.size);
	return direccionReal;

}

t_puntero obtenerPosicionVariable(t_nombre_variable identificador){
	printf("[OBTENER POSICION VARIABLE - STACK LEVEL: %i]: '%c'\n", pcb->nivelDelStack, identificador);
	t_list* lista;
	if(isalpha(identificador))
		lista = pcb->indiceStack[pcb->nivelDelStack].variables;
	else
		lista = pcb->indiceStack[pcb->nivelDelStack].argumentos;
	bool elIdEsElMismo(variable* variable)
	{
		return variable->identificador == identificador;
	}
	variable* unaVariable = list_find(lista, elIdEsElMismo);
	t_puntero direccionReal = convertirADireccionReal(unaVariable->posicion);
	printf("[OBTENER POSICION VARIABLE - STACK LEVEL: %i]: '%c' -> %i\n", pcb->nivelDelStack, identificador, direccionReal);
	return direccionReal;
}

t_valor_variable asignarValorCompartida(t_nombre_compartida variable, t_valor_variable valor){//falta ersolver
	t_valor_variable valorAsignado=valor;
	lSend(kernel, (t_valor_variable) valorAsignado,ASIGNARCOMPARTIDA,sizeof(t_valor_variable));
	return valorAsignado;

}

t_valor_variable obtenerValorCompartida(t_nombre_compartida nombre){
	int tamanio = strlen(nombre)+1;
	char * nombreVariable = malloc(tamanio);
	char* bCero= "/0";

	memcpy(nombreVariable, nombre, strlen(nombre));
	memcpy(nombreVariable+(strlen(nombre)), bCero,strlen(bCero));


	lSend(kernel, (char*) nombreVariable, OBTENERCOMPARTIDA,tamanio);

	Mensaje *m = lRecv(kernel);
	t_valor_variable valor = (t_valor_variable) m->data;

	return valor;
}

void asignar(t_puntero direccionReal, t_valor_variable valor)
{
	printf("[ASIGNAR VARIABLE - STACK LEVEL: %i]\n", pcb->nivelDelStack);
	posicionEnMemoria posicion = convertirADireccionLogica(direccionReal);
	escribirEnMemoria(posicion, valor);
	printf("[ASIGNAR VARIABLE - STACK LEVEL: %i]: PAG: %i | OFFSET: %i | SIZE: %i | VALOR: %i\n", pcb->nivelDelStack, posicion.pagina, posicion.offset, posicion.size, valor);


}


posicionEnMemoria calcularPosicion(int nivelDelStack)
{
	posicionEnMemoria unaPosicion;
	unaPosicion.size = 4;
	if(nivelDelStack == 0)
	{
		if(list_is_empty(pcb->indiceStack[0].variables))
		{
			unaPosicion.pagina = pcb->cantPaginasCodigo;
			unaPosicion.offset = 0;
		}
		else
			unaPosicion = generarPosicionEnBaseAUltimaVariableDe(pcb->indiceStack[0].variables);
	}
	else
	{
		if(list_is_empty(pcb->indiceStack[nivelDelStack].variables))
		{
			if(!list_is_empty(pcb->indiceStack[nivelDelStack].argumentos))
				unaPosicion = generarPosicionEnBaseAUltimaVariableDe(pcb->indiceStack[nivelDelStack].argumentos);
			else
				unaPosicion = calcularPosicion(nivelDelStack-1);
		}
		else
			unaPosicion = generarPosicionEnBaseAUltimaVariableDe(pcb->indiceStack[nivelDelStack].variables);
	}
	return unaPosicion;
}



posicionEnMemoria generarPosicionEnBaseAUltimaVariableDe(t_list* lista)
{
	posicionEnMemoria posicion;
	posicion.size = 4;
	variable* ultimaVariable = obtenerUltimaVariable(lista);
	posicionEnMemoria posicionUltimaVariable = ultimaVariable->posicion;
	posicion.offset = posicionUltimaVariable.offset+4;
	if(posicion.offset > tamanioPagina) // ESTO ESTA MAL, HAY QUE CAMBIARLO. UNA INSTRUCCION PUEDE QUEDAR PARTE EN UNA PAGINA Y PARTE EN OTRA
	{
		posicion.pagina = posicionUltimaVariable.pagina+1;
		posicion.offset = 0;
	}
	else
		posicion.pagina = posicionUltimaVariable.pagina;
	return posicion;

}

t_puntero convertirADireccionReal(posicionEnMemoria unaPosicion)
{
	return (unaPosicion.pagina*tamanioPagina) + unaPosicion.offset;
}

posicionEnMemoria convertirADireccionLogica(t_puntero posicionReal)
{
	posicionEnMemoria posicion;
	posicion.pagina = floor((double)posicionReal/(double)tamanioPagina);
	posicion.offset = posicionReal % tamanioPagina;
	posicion.size = 4;
	return posicion;
}

variable* obtenerUltimaVariable(t_list* listaVariables)
{
	int ultimaPos = list_size(listaVariables)-1;
	variable* unaVariable = list_get(listaVariables, ultimaPos);
	return unaVariable;
}


void escribirEnMemoria(posicionEnMemoria posicion, t_valor_variable valor)
{
	pedidoEscrituraMemoria* pedido = malloc(sizeof(pedidoEscrituraMemoria));
	pedido->posicion = posicion;
	pedido->valor = valor;
	lSend(memoria, pedido, 3,sizeof(pedidoEscrituraMemoria));
	free(pedido);
}

char* leerEnMemoria(posicionEnMemoria posicion)
{
	lSend(memoria, &posicion, LEER, sizeof(posicionEnMemoria));
	Mensaje* respuesta = lRecv(memoria);
	char* data = malloc(respuesta->header.tamanio);
	memcpy(data, respuesta->data, respuesta->header.tamanio);
	destruirMensaje(respuesta);
	return data;

}

void llamarConRetorno(t_nombre_etiqueta etiqueta, t_puntero dondeRetornar)
{
	printf("[LLAMAR CON RETORNO - STACK LEVEL: %i]\n", pcb->nivelDelStack);
	pcb->nivelDelStack++;
	pcb->indiceStack = realloc(pcb->indiceStack, sizeof(indStk)*(pcb->nivelDelStack+1));
	int returnpos = pcb->programCounter;
	//pcb->programCounter = metadata_buscar_etiqueta(etiqueta, pcb->indiceEtiqueta, pcb->sizeIndiceEtiquetas);
	variable var;
	var.identificador = '\0';
	var.posicion= convertirADireccionLogica(dondeRetornar);
	pcb->indiceStack[pcb->nivelDelStack].posicionDeRetorno = returnpos;
	pcb->indiceStack[pcb->nivelDelStack].variableDeRetorno = var;
	pcb->indiceStack[pcb->nivelDelStack].variables = list_create();
	pcb->indiceStack[pcb->nivelDelStack].argumentos = list_create();
	printf("[LLAMAR CON RETORNO - STACK LEVEL: %i a %i]: LLAMA A '%s' - RETURN POS: %i | RETURN VAR: [PAG: %i | OFFSET: %i | SIZE: %i]\n",pcb->nivelDelStack-1, pcb->nivelDelStack, etiqueta, returnpos, var.posicion.pagina, var.posicion.offset, var.posicion.size);
	irAlLabel(etiqueta);
}

void llamarSinRetorno(t_nombre_etiqueta etiqueta)
{
	printf("[LLAMAR SIN RETORNO - STACK LEVEL: %i]\n", pcb->nivelDelStack);
}

t_valor_variable dereferenciar(t_puntero posicion)
{
	printf("[DEREFERENCIAR - STACK LEVEL: %i]\n", pcb->nivelDelStack);
	posicionEnMemoria direccionLogica = convertirADireccionLogica(posicion);
	char* info = leerEnMemoria(direccionLogica);
	t_valor_variable valor = (int)(*info);
	printf("[DEREFERENCIAR - STACK LEVEL: %i]: OBTENGO DE MEMORIA EL SIGUIENTE VALOR: %i\n", pcb->nivelDelStack, valor);
	free(info);
	return valor;

}

void finalizar(void)
{
	printf("[FINALIZAR - STACK LEVEL: %i]\n", pcb->nivelDelStack);
	if(pcb->nivelDelStack == 0)
		finaliza = 1;
	else
	{
		pcb->programCounter = pcb->indiceStack[pcb->nivelDelStack].posicionDeRetorno;
		list_clean_and_destroy_elements(pcb->indiceStack[pcb->nivelDelStack].argumentos, free);
		list_clean_and_destroy_elements(pcb->indiceStack[pcb->nivelDelStack].variables, free);
		pcb->nivelDelStack--;
		printf("[FINALIZAR - STACK LEVEL: %i A %i]\n", pcb->nivelDelStack+1, pcb->nivelDelStack);
		pcb->indiceStack = realloc(pcb->indiceStack, sizeof(indStk)*(pcb->nivelDelStack+1));
	}

}
void irAlLabel(t_nombre_etiqueta nombre){
	printf("[IR A LABEL - STACK LEVEL: %i]\n", pcb->nivelDelStack);
	t_puntero_instruccion instruccionParaPCB;
	instruccionParaPCB = metadata_buscar_etiqueta(nombre, pcb->indiceEtiqueta, pcb->sizeIndiceEtiquetas);
	pcb->programCounter = instruccionParaPCB-1;
	printf("[IR A LABEL - STACK LEVEL: %i]: '%s' | PC: %i\n", pcb->nivelDelStack, nombre, pcb->programCounter);
}

void retornar(t_valor_variable valorDeRetorno){
	printf("[RETORNAR - STACK LEVEL: %i]\n", pcb->nivelDelStack);
	t_puntero dirReal = convertirADireccionReal(pcb->indiceStack[pcb->nivelDelStack].variableDeRetorno.posicion);
	asignar(dirReal,valorDeRetorno);
	printf("[RETORNAR - STACK LEVEL: %i]: RETORNO VALOR: %i\n", pcb->nivelDelStack, valorDeRetorno);

}
	//PRIMITIVAS ANSISOP KERNEL

void wait(t_nombre_semaforo nombre){
	int tamanio = strlen(nombre)+1;
	char * nombreSemaforo = malloc(tamanio);
	char* bCero= "/0";
	memcpy(nombreSemaforo, nombre, strlen(nombre));
	memcpy(nombreSemaforo+(strlen(nombre)), bCero,strlen(bCero));
	lSend(kernel, (char*) nombreSemaforo, WAIT, strlen(nombreSemaforo));
	Mensaje *m =lRecv(kernel);
	int bloqueado = (int) m->data;//No se si va esto
	if(bloqueado){
		puts("Se bloqueo maestro");
	}
	free(nombreSemaforo);
}

void signal(t_nombre_semaforo nombre){
	int tamanio = strlen(nombre)+1;
	char * nombreSemaforo = malloc(tamanio);
	char* bCero= "/0";
	memcpy(nombreSemaforo, nombre, strlen(nombre));
	memcpy(nombreSemaforo+(strlen(nombre)), bCero,strlen(bCero));
	lSend(kernel, (char*) nombreSemaforo, SIGNAL, strlen(nombreSemaforo));
}

t_puntero reservar(t_valor_variable nroBytes){
	int tamanioAReservar = nroBytes;
	lSend(kernel, &tamanioAReservar, RESERVAR_MEMORIA_HEAP, sizeof(tamanioAReservar));

	Mensaje *m =lRecv(kernel);
	t_puntero puntero= (t_puntero) m->data;
	if(puntero<0){
		puts("No se pudo reservar memoria en el heap");
	}

	return puntero;

}


void liberar(t_puntero puntero ){

	lSend(kernel, (t_puntero) &puntero, LIBERAR_PUNTERO, sizeof(t_puntero));
	return;
}

/*	t_descriptor_archivo abrir(t_direccion_archivo, t_banderas){
		return;
	}

	void borrar(t_descriptor_archivo){
		return;
	}

	void cerrar(t_descriptor_archivo){
		return;
	}

	void moverCursor(t_descriptor_archivo, t_valor_variable){
		return;
	}

	void escribir(t_descriptor_archivo, void*, t_valor_variable){
		return;
	}

	void leer(t_descriptor_archivo, t_puntero, t_valor_variable){
		return;
	}



*/
