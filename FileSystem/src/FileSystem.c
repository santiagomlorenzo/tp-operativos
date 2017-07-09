#include "FileSystem.h"

configFile* config;
metadata* metad;

int main(int argc, char** argsv) {
	config = configurate("/home/utnso/Escritorio/tp-2017-1c-The-Kernels/FileSystem/Debug/filesystem.conf", leerArchivoConfig, keys);
	levantarPaths();
	levantarBitmap();
	puts("bitmap levantado");
	//return EXIT_SUCCESS;
	levantarBloquesEnUso();
	puts("bitmap al dia");
	//testsmuycabezitas();
	//while(1){puts("listo");}
	kernel = getBindedSocket("127.0.0.1", config->puerto);
	lListen(kernel, 5);
	puts("ESPERANDO AL KERNEL");
	conexion = lAccept(kernel, KERNEL_ID);
	esperarOperacion();
	close(kernel);
	free(config);
	free(metad);
	destruirBitmap();
	puts("fin");
	return EXIT_SUCCESS;
}

void levantarPaths()
{
	ruta_CMeta = string_from_format("%s/Metadata/", config->punto_montaje);
	ruta_Meta = string_from_format("%sMetadata.bin", ruta_CMeta);
	ruta_BitM = string_from_format("%s/Metadata/Bitmap.bin", config->punto_montaje);
	ruta_Arch = string_from_format("%s/Archivos", config->punto_montaje);
	ruta_Blqs = string_from_format("%s/Bloques/", config->punto_montaje);
//	tamRuta_Conf = strlen(ruta_Conf);
	tamRuta_Meta = strlen(ruta_Meta);
	tamRuta_BitM = strlen(ruta_BitM);
	tamRuta_CMeta = strlen(ruta_CMeta);
	tamRuta_Blqs = strlen(ruta_Blqs);
	tamRuta_Arch = strlen(ruta_Arch);
	metad = configurate(ruta_Meta, leerArchivoMetadata, metaKeys);
	cantBloq= metad->cantidad_Bloques;
	tamBloq= metad->tamanio_Bloques;
}


void esperarOperacion()
{
	puts("Esperando Operacion");
	while(1)
	{
		Mensaje* mensaje = lRecv(conexion);
		puts("Operacion recibida");
		int sizePath;
		//char* path;
		int offset;
		int  size;
		int sizeBuffer;
		//char* buffer;
		switch(mensaje->header.tipoOperacion){
			case -1:
				puts("MURIO EL KERNEL /FF");
				exit(EXIT_FAILURE);
				break;
			case 1:
			{
				// verificar archivo
				puts("VERIFICAR ARCHIVO");
				char* path = agregarBarraCero(mensaje->data, mensaje->header.tamanio);
				// LO QUE HAYA QUE HACER PARA VALIDAR EL ARCHIVO
				// DEVOLVER CON lSend OPERACION 104 y EL ARCHIVO ES VALIDO, SINO
				// NUMERO NEGATIVO QUE NO SEA -1

				int retorno = validarArchivo(path);
				if(retorno==-1)
					lSend(conexion, NULL, -5,0);
				else
					lSend(conexion, NULL, 104, 0);
				free(path);
				break;
			}


			/*case 2:{
				//Op.abrir
				memcpy(flagC,mensaje->data,sizeof(int));
				memcpy(flagR,mensaje->data + sizeof(int),sizeof(int));
				memcpy(flagW,mensaje->data + sizeof(int)*2,sizeof(int));
				memcpy(sizePath, mensaje->data + sizeof(int)*3, sizeof(int));
				memcpy(path, mensaje->data + sizeof(int)*4, sizePath);
				int retorno = validarArchivo(path);
				if(retorno==0){
					retornoDePath(path);
				}
				if(retorno==-1){
					if(flagC==1){
						int result = crearArchivo(path);
						if(result==-1){
							lSend(kernel, 0, 1, sizeof(int));
						}
						retornoDePath(path);
					}
					else{
						lSend(kernel, 0, 1, sizeof(int));
					}
				}
				break;
			}*/
			/*case 1:{
				//Op.crear
				memcpy(flagC,mensaje->data,sizeof(int));
				memcpy(flagR,mensaje->data + sizeof(int),sizeof(int));
				memcpy(flagW,mensaje->data + sizeof(int)*2,sizeof(int));
				memcpy(sizePath, mensaje->data + sizeof(int)*3, sizeof(int));
				memcpy(path, mensaje->data + sizeof(int)*4, sizePath);
				crearArchivo(path);
				break;
			}*/
			case 2:{
				//Op.leer
				char* path;
				memcpy(sizePath, mensaje->data, sizeof(int));
				path = agregarBarraCero(mensaje->data + sizeof(int), sizePath);
				memcpy(offset, mensaje->data+ sizeof(int) +sizePath, sizeof(int));
				memcpy(size, mensaje->data + sizeof(int)*2 + sizePath, sizeof(int));
				// NO SE VALIDA, SE VALIDO CUANDO SE ABRIO EL ARCHIVO, SI EL PEDIDO ES INCORRECTO YA LO CHEQUEO EL KERNEL
			/*	int retorno = validarArchivo(path);
				if(retorno==-1){
					lSend(kernel, 0, 1, sizeof(int));
					break;
				}*/
				char* buffer = leerArchivo(path,offset,size);
				// ESTE CHEQUEO NO SE QUE ES PERO CAPAZ NO ES NECESARIO, POR AHORA EL KERNEL LO IGNORA
				if(buffer=="-1"){
					lSend(conexion, 0, 1, sizeof(int));
					free(buffer);
					break;
				}
				//enviar el buffer
				lSend(conexion, buffer, 2, sizeof(char)*size);
				free(buffer);
				break;
			}
			case 3:{
				//Op.escribir
				char* buffer;
				memcpy(sizePath, mensaje->data, sizeof(int));
				char* path = agregarBarraCero(mensaje->data + sizeof(int), sizePath);
				memcpy(offset, mensaje->data+ sizeof(int) +sizePath, sizeof(int));
				memcpy(size, mensaje->data + sizeof(int)*2 +sizePath, sizeof(int));
				memcpy(buffer, mensaje->data + sizeof(int)*3 + sizePath, size);
				// IDEM LEER
			/*	int retorno = validarArchivo(path);
				if(retorno==-1){
					lSend(kernel, 0, 1, sizeof(int));
					break;
				}*/
				// IDEM LEER, PUEDE NO SER NECESARIO
				int result = escribirArchivo(path,offset,size,buffer);
				if(result==-1){
					lSend(conexion, 0, 1, sizeof(int));
					break;
				}
				// WHY? EL KERNEL YA CONOCE EL PATH
				//retornoDePath(path);
				break;
			}
			case 5:{
				//Op.borrar
				memcpy(sizePath, mensaje->data, sizeof(int));
				char* path = agregarBarraCero(mensaje->data+sizeof(int), sizePath);
				// IDEM
			/*	int retorno = validarArchivo(path);
				if(retorno==-1){
					lSend(kernel, 0, 1, sizeof(int));
					break;
				}*/
				int result = borrarArchivo(path);
				// IDEM
				if(result==-1){
					lSend(conexion, 0, 1, sizeof(int));
					break;
				}
				// CREO QUE NO LO NECESITA
				//lSend(kernel, 1, 1, sizeof(int));
				break;
			}
			case 4:
			{
				// CREAR ARCHIVO
				puts("CREAR ARCHIVO");
				char* path = agregarBarraCero(mensaje->data, mensaje->header.tamanio);
				if(crearArchivo(path) == -1)
					puts("ALGO FALLO");
				free(path);
			}
		}
		destruirMensaje(mensaje);
	}
}

char* agregarBarraCero(char* data, int tamanio)
{
	char* path = malloc(tamanio+1);
	memcpy(path, data, tamanio);
	path[tamanio] = '\0';
	return path;
}

int validarArchivo(char* pathRelativa){
	char* path = string_from_format("%s%s.bin", ruta_Arch, pathRelativa);
	FILE *arch;
	arch = fopen (path,"r");
	if (arch==NULL){
		return -1;
	}
	fclose(arch);
	free(path);
	return 0;
	/*archivo* arch;
	arch = configurate(path, leerArch, archKeys);
	enviarTamanioArchivo(arch->tamanio);
	free(arch);
	return 0;*/
}

/*enviarTamanioArchivo(int tamanio){
	lSend(conexion, "archivo existente", 1, sizeof(char)*17);
	lSend(conexion,  tamanio, 1, sizeof(int));
}*/

int crearArchivo(char* pathRelativa){
	char* path = string_from_format("%s%s.bin", ruta_Arch, pathRelativa);
	FILE *arch;
	arch = fopen (path,"w");
	if (arch==NULL){
		return -1;
	}
	//funcion tomar siguiente bloque de bitmap
	int bloque=0;
	int valor= bitarray_test_bit(Ebitarray, bloque);
	while(valor==1&&bloque<cantBloq){
		bloque++;
		valor = bitarray_test_bit(Ebitarray, bloque);
	}
	if(bloque>cantBloq){
		return-1;
	}
	bitarray_set_bit(Ebitarray,bloque);
	FILE *blq;
	char* ruta_Blq;
	char* Sbloque = string_itoa(bloque);
	/*char* Sbloqueaux;
	Sbloqueaux=malloc(sizeof(Sbloque));
	*Sbloqueaux= *Sbloque;*/
	//int digitos = log10(bloque)+1;
	int tam = 1 + strlen(ruta_Blqs) + strlen(Sbloque) + strlen(".bin") ;
	ruta_Blq = malloc(sizeof(char)*tam);
	int i;
	strcpy(ruta_Blq,ruta_Blqs);
	strcat(ruta_Blq,Sbloque);
	strcat(ruta_Blq,".bin");
	blq = open (ruta_Blq, O_CREAT, S_IRUSR|S_IWUSR);
	close(blq);
	//funcion tomar siguiente bloque de bitmap
	char stamanio[11]="TAMANIO=0\n";
	char* sbloques = NULL;
	sbloques = malloc(sizeof(char)*(strlen(Sbloque)+11));
	strcpy(sbloques,"BLOQUES=[");
	strcat(sbloques,Sbloque);
	strcat(sbloques,"]");
	int bloquesSize= strlen (sbloques);
	fwrite(stamanio,sizeof(char), sizeof(char)*10,arch);
	fwrite(sbloques,sizeof(char),sizeof(char)*bloquesSize,arch);
	fflush(arch);
	fclose(arch);
	free(ruta_Blq);
	free(Sbloque);
	free(sbloques);
	free(path);
	return 0;
}

char* leerArchivo(char* path, int offset, int size){
	char* buffer=NULL;
	int tam = getTamanio(path);
	if(tam < (offset+size)){
		return "-1";
	}
	bloques* bloqs;
	int* bloques;
	bloqs= getbloques(path);
	bloques = bloqs->bloques;
	int posBloqueInicio = 1 + ((offset-1)/tamBloq); //redondeo hacia arriba
	int bloqueInicio = bloques[posBloqueInicio-1];
	char* rutablq = rutabloque(bloqueInicio);
	int offsetBloque = offset - ((posBloqueInicio-1)*tamBloq);
	FILE* arch;
	arch = fopen(rutablq,"r");
	int i;
	char c;
	int cc = 0;
	for(i=0;cc<size;i++){
		c = fgetc(arch);
		if (i>=offsetBloque){
			buffer = realloc(buffer,sizeof(char)*(cc+1)+1);
			buffer[cc]=c;
			cc++;
		}
	}
	/*int j;
	for(j=0;j<=size;j++){
		c = fgetc(arch);
		srtcat(buffer,c);
	}*/
	fclose(arch);
	free(bloques);
	free(bloqs);
	free(rutablq);
	return buffer;
}

int escribirArchivo(char* path, int offset, int size, char* buffer){
	FILE* blq;
	FILE* blqt;
	int faltanbloques=0;
	int Tam = getTamanio(path);
	if(Tam < (offset)){
		return -1;
	}
	bloques* bloqs;
	bloques* bloqsN;
	int* bloques;
	bloqs= getbloques(path);
	bloques = bloqs->bloques;
	int cantbloques=bloqs->tamanio;
	int posBloqueInicio = 1 + ((offset-1)/tamBloq); //redondeo hacia arriba
	int bloqueInicio = bloques[posBloqueInicio-1];
	if(bloqueInicio==bloques[cantbloques-1]){ //Es el ultimo bloque
		int usadoBloque =tamBloq-(cantbloques*tamBloq - Tam) ;
		/*if(usadoBloque<0){
			usadoBloque=0;
		}*/
		if (offset>usadoBloque){
			return -1;
		}
		int DispBloque=tamBloq-usadoBloque;
		if(size>DispBloque){
			faltanbloques=1;
		}
		char* rutablq = rutabloque(bloqueInicio);
		char* pathtemp = NULL;
		pathtemp = malloc (strlen(ruta_Blqs)+13);
		strcpy(pathtemp,ruta_Blqs);
		strcat(pathtemp,"temporal.bin");
		blqt = fopen(pathtemp,"w");
		blq = fopen(rutablq,"r");
		char c;
		int i;
		for(i=0;i<offset;i++){
			c=fgetc(blq);
			fputc(c,blqt);
		}
		if(faltanbloques==0){
			for(i=0;i<size;i++){
				fputc(buffer[i],blqt);
				c=fgetc(blq);
			}
			if(usadoBloque>offset+size){
				for(i=0;i<usadoBloque-offset-size;i++){
					c=fgetc(blq);
					fputc(c,blqt);
				}
			}
				free(bloqs);
				free(bloques);
				fclose(blq);
				fclose(blqt);
				remove(rutablq);                    // borrar el original
				rename(pathtemp, rutablq);  // renombrar el temporal
				free(rutablq);
				free(pathtemp);
		}else{/////////////////////////revisar esta parte
			faltanbloques=0;
			for(i=0;i<DispBloque;i++){
				fputc(buffer[i],blqt);
			}
			free(bloqs);
			free(bloques);
			fclose(blq);
			fclose(blqt);
			free(rutablq);
			free(pathtemp);
			int SizeActual= size-DispBloque;
			int* bloquesN;
			while(SizeActual>0){
				addbloque(path);
				bloqsN= getbloques(path);
				bloquesN=bloqsN->bloques;
				int cantbloquesN=bloqsN->tamanio;
				char* rutablq = rutabloque(bloques[cantbloquesN]);
				char* pathtemp = NULL;
				pathtemp = malloc (strlen(ruta_Blqs)+13);
				strcpy(pathtemp,ruta_Blqs);
				strcat(pathtemp,"temporal.bin");
				blqt = fopen(pathtemp,"w");
				blq = fopen(rutablq,"r");
				for(i=0;i<offset;i++){
					c=fgetc(blq);
					fputc(c,blqt);
				}
				if(SizeActual-tamBloq>0){
					SizeActual=SizeActual-tamBloq;
					for(i=0;i<tamBloq;i++){
						fputc(c,blqt);
					}
				}
				else{
					for(i=0;i<SizeActual;i++){
						fputc(c,blqt);
					}
					SizeActual=SizeActual-tamBloq;
				}
				free(bloqs);
				free(bloqsN);
				free(bloques);
				free(bloquesN);
				fclose(blq);
				fclose(blqt);
				remove(rutablq);                    // borrar el original
				rename(pathtemp, rutablq);  // renombrar el temporal
				free(rutablq);
				free(pathtemp);
			}
		}
		if(offset+size>usadoBloque){//////////////anda solo para 1 bloque
		cambiarTamanio(path,offset+size);
		}
		return 0;
	}
}
										/////////////////////////////
	int escribirArchivomalos(char* path, int offset, int size, char* buffer){ // NO CONSIDERA LA SOBREESCRITURA EN OTROS BLOQUES QUE NO SEA EL UTILMO Y PROXIMOS
	int Tam = getTamanio(path);
	int Disponible = Tam - offset;
	int Actualsize = size;
	char * bufDis = malloc(sizeof(char)*(Disponible-1));
	int i;
	int c = 0;
	for(i=0;i<=(Disponible-1);i++){
		bufDis[i] = NULL;
	}
	bloques* bloqsv;
	bloqsv=malloc(sizeof(bloques));
	int* bloquesviejos;
	bloqsv= getbloques(path);
	bloquesviejos = bloqsv->bloques;
	int cantbloquesviejos = bloqsv->tamanio;
	if(Disponible>0){
		int j;
		for(j=0;j<Disponible;j++){
			if(buffer[c]!=NULL){
				strcat(bufDis,buffer[c]);
				c++;
			}
		}
		Actualsize = size -Disponible;
	}
	int newEspacio = 0;
	if(Disponible<0){
		newEspacio = Disponible;
	}
	int blqsadd = 0;
	while(Actualsize>newEspacio){
		addbloque(path);
		newEspacio= newEspacio + tamBloq;
		blqsadd++;
	}
	bloques* bloqs;
	bloqs=malloc(sizeof(bloques));
	int* bloques;
	bloqs= getbloques(path);
	bloques = bloqs->bloques;
	int cantbloques = bloqs->tamanio;
	FILE* blq;
	FILE* blqt;
	char* blqE = rutabloque(bloques[cantbloquesviejos]);
	blq = fopen(blqE,"r");
	char* pathtemp = NULL;
	pathtemp = malloc (strlen(ruta_Blqs)+13);
	strcpy(pathtemp,ruta_Blqs);
	strcat(pathtemp,"temporal.bin");
	blqt = fopen(pathtemp,"w");
	if(bufDis != NULL){
	fputs(bufDis,blqE);
	}
	fflush(blqt);
	fclose(blqt);
	fclose(blq);
	remove(blqE);                    // borrar el original
	rename(pathtemp, blqE);  // renombrar el temporal
	while(blqsadd!=0){
		FILE* blq;
		FILE* blqt;
		cantbloquesviejos++;
		char* blqE = rutabloque(bloques[cantbloquesviejos]);
		blq = fopen(blqE,"r");
		char* pathtemp = NULL;
		pathtemp = malloc (strlen(ruta_Blqs)+13);
		strcpy(pathtemp,ruta_Blqs);
		strcat(pathtemp,"temporal.bin");
		blqt = fopen(pathtemp,"w");
		bufDis = realloc(NULL,sizeof(char)*(tamBloq-1));
		int k=0;
		for(k=0;k<=(tamBloq-1);k++){
			if(buffer[c]!=NULL){
				strcat(bufDis,buffer[c]);
				c++;
			}
		}
		fputs(bufDis,blqt);
		blqsadd--;
		fflush(blqt);
		fclose(blqt);
		fclose(blq);
		remove(blqE);                    // borrar el original
		rename(pathtemp, blqE);  // renombrar el temporal
	}
	free(bufDis);
	free(bloques);
	free(bloquesviejos);
	free(bloqs);
	free(bloqsv);
	return 0;
}
											/////////////////////////////

int borrarArchivo(char* path){
	bloques* bloqs;
	int* bloques;
	bloqs= getbloques(path);
	bloques = bloqs->bloques;
	int len= bloqs->tamanio;
	int i;
	for(i=0;i<len;i++){
		char * pathbloque=rutabloque(bloques[i]);
		remove(pathbloque);
		/*if(remove(pathbloque)!=0){
			return -1;
		}*/
		bitarray_clean_bit(Ebitarray, bloques[i]);
		free(pathbloque);
	}
	if(remove(path)!=0){
		free(bloques);
		free(bloqs);
		return -1;
	}
	free(bloques);
	free(bloqs);
	return 0;
}

char* rutabloque(bloque){
	char* resultado=NULL;
	char* sbloque = string_itoa(bloque);
	resultado = malloc(strlen(ruta_Blqs)+strlen(sbloque)+5);
	strcpy(resultado,ruta_Blqs);
	strcat(resultado,sbloque);
	strcat(resultado,".bin");
	free (sbloque);
	return resultado;
}

bloques* getbloques(char* path){
	bloques* bloqs;
	bloqs=malloc(sizeof(bloques));
	int cont=0;
	t_config* Archconf=config_create(path);
	char** sBloques =config_get_array_value(Archconf,"BLOQUES");
	while(sBloques[cont]!=NULL){
		cont++;
	}
	int*bloques=malloc(sizeof(int)*cont);
	cont=0;
	while(sBloques[cont]!=NULL){
		bloques[cont]=atoi(sBloques[cont]);
		cont++;
		}
	bloqs->bloques=bloques;
	bloqs->tamanio=cont;
	config_destroy(Archconf);
	free(sBloques);
	return bloqs;
}
	/*bloques* bloqs;
	bloqs=malloc(sizeof(bloques));
	FILE* arch;
	arch = fopen(path,"r");
	int* bloques = NULL;
	char * buffer = NULL;
	char c = 0;
	while(c!='['){
		c = fgetc(arch);
	}
	int i = 1;
	int j = 1;
	int k = 0;
	int l = 0;
	c = fgetc(arch);
	while(c!=']'){
		if(c!=','){
			buffer = realloc(buffer, sizeof(char)*i);
			buffer[i-1] = c;
			i++;
		}
		else{
			j++;
			bloques = realloc(bloques, sizeof(int)*i);
			bloques[k]=0;
			for(l=0;l<(i-1);l++){
				bloques[k] = bloques[k]*10+buffer[l]-'0';
			}
			k++;
			i=1;
		}
		c = fgetc(arch);
	}
	bloques = realloc(bloques, sizeof(int)*i);
	bloques[k]=0;
	for(l=0;l<(i-1);l++){
		bloques[k] = bloques[k]*10+buffer[l]-'0';
	}
	k++;
	fclose(arch);
	bloqs->bloques=bloques;
	bloqs->tamanio=k;
	free(buffer);
	return bloqs;
}*/
		/*char* retorno;
		strcpy(retorno,buffer);
		free (buffer);
		return retorno;
        buffer = (char*)realloc(NULL, sizeof(char));
	        c = fgetc(archivo);
	        i = 0;
	        while( c != '\n')
	        {
	            buffer[i] = c;
	            i++;
	            buffer = (char*)realloc(buffer, (i+1)*sizeof(char));
	            c = fgetc(archivo);
	        }
	        //Agrego el \n al buffer
	        buffer = (char*)realloc(buffer, (i+1)*sizeof(char));
	        buffer[i] = c;
	       //Trabajar con el buffer
	       free(buffer);
	}*/

int addbloque(char* path){
	//funcion tomar siguiente bloque de bitmap
	int bloque=0;
		int valor= bitarray_test_bit(Ebitarray, bloque);
		while(valor==1&&bloque<cantBloq){
			bloque++;
			valor = bitarray_test_bit(Ebitarray, bloque);
		}
	if(bloque>cantBloq){
		return-1;
	}
	bitarray_set_bit(Ebitarray,bloque);
	FILE *blq;
	char* ruta_Blq;
	char* Sbloque = string_itoa(bloque);
	/*char* Sbloqueaux;
	Sbloqueaux=malloc(sizeof(Sbloque));
	*Sbloqueaux= *Sbloque;*/
	//int digitos = log10(bloque)+1;
	int tam = 1 + strlen(ruta_Blqs) + strlen(Sbloque) + strlen(".bin") ;
	ruta_Blq = malloc(sizeof(char)*tam);
	int i;
	strcpy(ruta_Blq,ruta_Blqs);
	strcat(ruta_Blq,Sbloque);
	strcat(ruta_Blq,".bin");
	blq = open (ruta_Blq, O_CREAT, S_IRUSR|S_IWUSR);
	close(blq);
	//funcion tomar siguiente bloque de bitmap
	FILE* arch;
	FILE* archt;
	arch = fopen(path,"r");
	char* pathtemp = NULL;
	pathtemp = malloc (strlen(ruta_Arch)+13);
	strcpy(pathtemp,ruta_Arch);
	strcat(pathtemp,"temporal.bin");
	char* ultblq = NULL;
	ultblq = malloc (strlen(Sbloque)+3);
	strcpy( ultblq,",");
	strcat( ultblq,Sbloque);
	strcat( ultblq,"]");
	archt = fopen(pathtemp,"w");
	char c;
	int end=0;
	while(end!=1){
		c = fgetc(arch);
		if(c==']'){
			fputs(ultblq,archt);
			end=1;
		}
		else{
			putc(c,archt);
		}
	}
	fflush(archt);
	fclose(arch);
	fclose(archt);
	remove(path);             // borrar el original
	rename(pathtemp, path);  // renombrar el temporal
	free(pathtemp);
	free(ruta_Blq);
	free(Sbloque);
	free(ultblq);
	return bloque;
}

quitarUltimoBloque(char* path){
	bloques* bloqs;
	bloqs= getbloques(path);
	int* bloques = bloqs->bloques;
	/*int j;
	for(j=0;j<34;j++){
	printf("%d",bloques[j]);
	}
	int meme[40];
	int meme2 = sizeof(meme);*/
	int cantbloques = bloqs->tamanio;
	FILE* arch;
	FILE* archt;
	arch = fopen(path,"r");
	char* pathtemp = NULL;
	pathtemp = malloc (strlen(ruta_Arch)+13);
	strcpy(pathtemp,ruta_Arch);
	strcat(pathtemp,"temporal.bin");
	archt = fopen(pathtemp,"w");
	char c;
	int cc = 0;
	while(cc<cantbloques-1){
		c = fgetc(arch);
		if(c==','){
			cc++;
			if(cc==cantbloques-1){
				putc(']',archt);
			}
			else{
				putc(c,archt);
			}
		}
		else{
			putc(c,archt);
		}
	}
	char * pathbloque=rutabloque(bloques[(cantbloques-1)]);
	remove(pathbloque);
	bitarray_clean_bit(Ebitarray, bloques[(cantbloques-1)]);
	fflush(archt);
	fclose(arch);
	fclose(archt);
	remove(path);             // borrar el original
	rename(pathtemp, path);  // renombrar el temporal
	free(pathbloque);
	free(pathtemp);
	free(bloques);
	free(bloqs);
	return;
}

int getTamanio(char * path){
		t_config* Archconf=config_create(path);
		int tamanio =config_get_int_value(Archconf,"TAMANIO");
		config_destroy(Archconf);
		return tamanio;
	/*FILE* arch;
	arch = fopen(path,"r");
	int* buffer = NULL;
	char c;
	while(c!='='){
		c = fgetc(arch);
	}
	int i=0;
	while(c!='/n'){
		i++;
		buffer =realloc(buffer, sizeof(char)*i);
		buffer[i]=c;
		c = fgetc(arch);
	}
		//char* retorno;
		//strcpy(retorno,buffer);
		//free (buffer);
	int retorno = string_itoa(buffer);
	fclose(arch);
	free(buffer);
	return retorno;*/
}

int cambiarTamanio(char* path,int tam){
	FILE* arch;
	FILE* archt;
	int tamviejo = getTamanio(path);
	char* stamviejo= string_itoa(tamviejo);
	arch = fopen(path,"r");
	char* pathtemp = NULL;
	pathtemp = malloc (strlen(ruta_Arch)+13);
	strcpy(pathtemp,ruta_Arch);
	strcat(pathtemp,"temporal.bin");
	archt = fopen(pathtemp,"w");
	char c;
	while(c!='='){
		c = fgetc(arch);
		putc(c,archt);
	}
	char* stam= string_itoa(tam);
	int j;
	for(j=0;j<strlen(stam);j++){
		fputc(stam[j],archt);
	}
	int i;
	for(i=0;i<strlen(stamviejo);i++){
		c = fgetc(arch);
	}
	while(c!=']')
	{
		c = fgetc(arch);
		putc(c,archt);
	}
	fflush(archt);
	fclose(arch);
	fclose(archt);
	remove(path);            // borrar el original
	rename(pathtemp, path);  // renombrar el temporal
	free(pathtemp);
	free(stam);
	free(stamviejo);
	return 0;
}

int levantarBitmap(){
	///cantBloq=80;////Para test
	FILE* arch0;
	FILE *bM;
	char* pathtemp = NULL;
	pathtemp = malloc(sizeof(char)*(tamRuta_CMeta + strlen("temporal.bin")+1));
	strcpy(pathtemp,ruta_CMeta);
	//pathtemp = malloc(sizeof(char)*strlen("temporal.bin"));
	strcat(pathtemp,"temporal.bin");
	char c;
	arch0 = fopen(pathtemp,"w+");
	c = fgetc (arch0);
	fclose(arch0);
	bM = fopen(ruta_BitM,"w+");
	if(bM == NULL){
		puts("Error ruta bitmap");
		return -1;
	}
	int i;
	for(i=0;i<cantBloq/8;i++){
		fputc(c,bM);
		//fwrite("/0",1,sizeof(char),bM);
		//system("dd if=/dev/zero bs=5 of=/home/utnso/tp-2017-1c-The-Kernels/mnt/FS_SADICA/Metadata/Bitmap.bin");
	}
	fflush(bM);
	fclose(bM);
	remove(pathtemp);
	free(pathtemp);

	  bM = open(ruta_BitM, O_RDWR);
	  if(bM == NULL){
	  	return -1;
	  }

	  bitarray = mmap(0,tamBloq,PROT_READ|PROT_WRITE,MAP_SHARED,bM,0);
	  Ebitarray = bitarray_create_with_mode(bitarray,cantBloq/8,LSB_FIRST);
	  int j;
	  for(j=0; j<=cantBloq;j++){
		  bitarray_clean_bit(Ebitarray, j);
	  }

	return 1;
}


//char* bitarray;
//t_bitarray* Ebitarray = bitarray_create_with_mode(bitarray,cantBloq/8,LSB_FIRST);
//levantarBloquesEnUso();
/*bitarray->size = sizeof(char);
bitarray->mode = LSB_FIRST;
off_t bit_index;
bit_index=
bitarray_set_bit(bitarray, bit_index);
*/

/*int levantarBloquesEnUso1(){
	FILE *bM;
		  bM = open(ruta_BitM, O_RDWR);
		  if(bM == NULL){
		  	return -1;
		  }
	 bitarray = mmap(0,tamBloq,PROT_READ|PROT_WRITE,MAP_SHARED,bM,0);
		  Ebitarray = bitarray_create_with_mode(bitarray,cantBloq/8,LSB_FIRST);
		  int i;
		  for(i=0; i<=cantBloq;i++){
			  bitarray_clean_bit(Ebitarray, i);
		  }
}*/

int levantarBloquesEnUso(){
	  DIR *dp;
	  struct dirent *ep;
	  dp = opendir (ruta_Blqs);

	  FILE *bM;
	  bM = open(ruta_BitM, O_RDWR);
	  if(bM == NULL){
	  	return -1;
	  }

	 /* bitarray = mmap(0,tamBloq,PROT_READ|PROT_WRITE,MAP_SHARED,bM,0);
	  Ebitarray = bitarray_create_with_mode(bitarray,cantBloq/8,LSB_FIRST);
	  int i;
	  for(i=0; i<=cantBloq;i++){
		  bitarray_clean_bit(Ebitarray, i);
	  }*/
	  /*if(bitarray_get_max_bit/=0){ //veo si el bitmap se encuentra bien iniciado
		  	  	  	  	  	  	   //quiero la suma de los bits la funcion esta me muestra el total creo
		  	  	  	  	  	  	   //podria ser un test moqueado
		  abort();
	  }*/

	  if (dp != NULL){///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//int* bloquesEnUso = malloc(sizeof(int));
		//int i = 1;
		char c = 0;
		int cont =0;
		//int suma20 = 0;
		//char *numaux;
		char *numerito =NULL;
		numerito = malloc(sizeof(char));
		//int conta = 0;
	    while (ep = readdir (dp)){
	    	cont = 0;
	    	free(numerito);
	    	numerito = NULL;
	    	//int tNombre = strlen(ep->d_name);
	    	//numerito = (char*) malloc(tNombre*sizeof(char));
	    	//numaux = ep->d_name;
	    	//puts(ep->d_name);
	    	//puts(numaux);
	    	//char *numerito;
	    	/*int j;
	    		for(j=0;j<=20;j++){
	    		numaux[j]=ep->d_name[j+20*suma20];
	    		//printf("%s",ep->d_name[j+20*suma20]);
	    	}
	    	suma20++;*/
	    	//printf("%s/n",numaux);
	    	c=ep->d_name[0];////////////////////////////////////////////////////
	    	while(isdigit(c)){
	    		numerito = realloc(numerito , 2+sizeof(char)*cont);
	    		numerito[cont]=c;
	    		cont++;
	    		c= ep->d_name[cont];
	    	}
	    	if(numerito!=NULL){
	    		//printf("%s\n",numerito);
	    		//int tnum;
				int j;
	    		int tam=strlen(numerito);
	    		char numi[tam];
	    		for(j=0;j<tam;j++){
	    			numi[j]=0;
	    		}
				strcpy(numi,numerito);
				int num =0;
				num = atoi(numi);
				//int prueba=0;
				//prueba = numerito - '0';
				/*for(j=1;j<tnum;j++){
					prueba = prueba * 10;
					prueba = numerito[j] - '0';
	    		}*/
				//printf("%d\n",num);
	    		bitarray_set_bit(Ebitarray, num);

	    	}///////////////////////////////////////////////////////////////
	    	/*int tNombre = strlen(ep->d_name[0]);
	    	printf("%s",ep->d_name[0]);
	    	printf("%d",tNombre);
	    	int i;
	    	for(i=0;i<tNombre;i++){
	    		c = ep->d_name[i];
	    		if(isdigit(c)=){
	    			numerito = realloc(NULL , 1+sizeof(numerito));
	    			numerito[cont]=c;
	    			cont++;
	    		}
	    	}
	    	if(numerito!=NULL){
	    		printf("%c",numerito);
	    		bitarray_set_bit(Ebitarray, numerito - '0');
	    	}*/
	    	//bloquesEnUso = realloc(bloquesEnUso,sizeof(int)*i);
	    	//puts(ep->d_name);
	    	//memcopy(bloquesEnUso, atoi(ep->d_name),sizeof(int));
	    	//i++;
	    }
	    (void) closedir (dp);
	    free(numerito);
	  }
	  else{
		//fflush(bM);
		//fclose(bM);
	    perror ("Error al abrir el directorio");
	    return -1;
	  }
	  //fflush(bM);
	  //fclose(bM);
	  return 1;
}

destruirBitmap(){
	bitarray_destroy(Ebitarray);
	//free(bitarray);			////lo hace solo el bitarray_destroy
	//free(Ebitarray);			////lo hace solo el bitarray_destroy
	remove(ruta_BitM);
	return;
}

retornoDePath(path){
	serializado sPath;
	sPath.size = sizeof(char)*strlen(path);
	sPath.data = malloc(sPath.size);
	char* puntero = sPath.data;
	memcpy(puntero, path, sizeof(char)*strlen(path));
	lSend(kernel, sPath.data, 1, sPath.size);
	free(sPath.data);
	free(puntero);
	return;
}

configFile* leerArchivoConfig(t_config* configHandler){
	configFile* config= malloc(sizeof(configFile));
	config->puerto = config_get_string_value(configHandler, "PUERTO");
	printf("FFSFASFAS: %s\n", config->puerto);
	config->punto_montaje = config_get_string_value(configHandler, "PUNTO_MONTAJE");
	//config_destroy(configHandler);
	imprimirConfig(config);
	return config;
}

metadata* leerArchivoMetadata(t_config* configHandler){
	metadata* metad= malloc(sizeof(metadata));
	metad->tamanio_Bloques = config_get_int_value(configHandler, "TAMANIO_BLOQUES");
	metad->cantidad_Bloques = config_get_int_value(configHandler, "CANTIDAD_BLOQUES");
	strcpy(metad->magic_Number,config_get_string_value(configHandler, "MAGIC_NUMBER"));
	config_destroy(configHandler);
	imprimirMetadata(metad);
	return metad;
}

archivo* leerArch(t_config* configHandler){
	archivo* arch= malloc(sizeof(archivo));
	arch->tamanio = config_get_int_value(configHandler, "TAMANIO");
	arch->bloques = config_get_array_value(configHandler, "BLOQUES");
	config_destroy(configHandler);
	return arch;
}

void imprimirConfig(configFile* config){
	puts("--------PROCESO FILESYSTEM--------");
	printf("ESCUCHANDO EN PUERTO: %s | PUNTO_MONTAJE %s\n", config->puerto, config->punto_montaje);
	puts("--------PROCESO FILESYSTEM--------\n");
}

void imprimirMetadata(metadata* metad){
	puts("--------FILESYSTEM METADATA--------");
	printf("TAMAÑO DE BLOQUES: %d | CANTIDAD DE BLOQUES: %d | MAGIC NUMBER: %s\n", metad->tamanio_Bloques, metad->cantidad_Bloques, metad->magic_Number);
	puts("--------FILESYSTEM METADATA--------");
}

char* IntToString(entero){
	char* chars = NULL;
	int* ints = NULL;
	int cont = 0;
	int i;
	while(entero!=0){
		int num=entero%10;
		entero = entero/10;
		cont++;
		ints = realloc(ints,sizeof(int)*cont);
		ints[cont-1] = num;
	}
	chars = malloc(sizeof(char)*cont);
	for(i=cont;i>0;i--){
		int cont2 = cont - i;
		chars[cont2]= (char)ints[i-1];
	}
	return chars;
}

testsmuycabezitas(){ //no se rian los estoy haciendo asi para comitearselos rapido
		bloques* bloqs;
		int *bloques;
		//test1 validarchexistente
		if (validarArchivo("/home/utnso/tp-2017-1c-The-Kernels/mnt/FS_SADICA/tests/existo.bin")==0){
			puts("piola test1");
		}
		else puts("error test1");
		//test1

		//test2 validarchnoexistente
		if (validarArchivo("/home/utnso/tp-2017-1c-The-Kernels/mnt/FS_SADICA/tests/NOexisto.bin")==0){
			puts("error test2");
		}
		else puts("piola test2");
		//test2

		//test3 crear
		crearArchivo("/home/utnso/tp-2017-1c-The-Kernels/mnt/FS_SADICA/tests/MeCreo.bin");
		bloqs=getbloques("/home/utnso/tp-2017-1c-The-Kernels/mnt/FS_SADICA/tests/MeCreo.bin");
		bloques = bloqs->bloques;
		char* rutablq = rutabloque(bloques[0]);
		//free (bloques);
		if (validarArchivo("/home/utnso/tp-2017-1c-The-Kernels/mnt/FS_SADICA/tests/MeCreo.bin")==0){
			if (validarArchivo(rutablq)==0){
				puts("piola test3");
			}
			else puts("error test3");
		}
		else puts("error test3");
		free(bloqs);
		free(bloques);
		//test3

		//test4 borrar creado
		 borrarArchivo("/home/utnso/tp-2017-1c-The-Kernels/mnt/FS_SADICA/tests/MeCreo.bin");
		 if (validarArchivo("/home/utnso/tp-2017-1c-The-Kernels/mnt/FS_SADICA/tests/MeCreo.bin")==0){
		 	puts("error test4");
		 }else if(validarArchivo(rutablq)==0){
			 puts("error test4");
		 		}
		 		else puts("piola test4");
		 free(rutablq);
		//test4

		//test5 crear archivo con 35 bloques
		 crearArchivo("/home/utnso/tp-2017-1c-The-Kernels/mnt/FS_SADICA/tests/MeCreo.bin");
		 int i;
		 int cont=0;
		 for(i=0;i<34;i++){
		 addbloque("/home/utnso/tp-2017-1c-The-Kernels/mnt/FS_SADICA/tests/MeCreo.bin");
		 }
		 bloqs=getbloques("/home/utnso/tp-2017-1c-The-Kernels/mnt/FS_SADICA/tests/MeCreo.bin");
		 bloques = bloqs->bloques;
		 if (validarArchivo("/home/utnso/tp-2017-1c-The-Kernels/mnt/FS_SADICA/tests/MeCreo.bin")==0){
			 int j;
			 for(j=0;j<=34;j++){
				char* rutablq = rutabloque(bloques[j]);
				if (validarArchivo(rutablq)==0){
					cont++;
				}
				free(rutablq);
			}
		 	if(cont==35){
		 		puts("piola test5");
		 	}
		 	else puts("error test5");
		 	}
		 else puts("error test5");
		free (bloqs);
		free (bloques);
		//test5

		//test6 quitar un bloque
		 quitarUltimoBloque("/home/utnso/tp-2017-1c-The-Kernels/mnt/FS_SADICA/tests/MeCreo.bin");
		 bloqs=getbloques("/home/utnso/tp-2017-1c-The-Kernels/mnt/FS_SADICA/tests/MeCreo.bin");
		 bloques = bloqs->bloques;
		 if(bloqs->tamanio==34){
			 puts("piola test6");
		 }
		 else puts("error test6");
		 free (bloqs);
		 free (bloques);
		//test6

		//test7 borrar archivo con muchos bloques
		 cont=0;
		 bloqs=getbloques("/home/utnso/tp-2017-1c-The-Kernels/mnt/FS_SADICA/tests/MeCreo.bin");
		 bloques = bloqs->bloques;
		 borrarArchivo("/home/utnso/tp-2017-1c-The-Kernels/mnt/FS_SADICA/tests/MeCreo.bin");
		 if (validarArchivo("/home/utnso/tp-2017-1c-The-Kernels/mnt/FS_SADICA/tests/MeCreo.bin")==0){
			puts("error test4");
		 }
		 else{
			int i;
			if (validarArchivo("/home/utnso/tp-2017-1c-The-Kernels/mnt/FS_SADICA/tests/MeCreo.bin")==-1){
				for(i=0;i<34;i++){
					char* rutablq = rutabloque(bloques[0]);
					if(validarArchivo(rutablq)==-1){
						cont++;
					}
					free(rutablq);
				}
			if(cont==34){
				puts("piola test7");
			}
			else	puts("error test7");
		}
		else puts("error test7");
		free (bloques);
		free(bloqs);
	}
		//test7

		//test8 //Leer
		char* buffer=leerArchivo("/home/utnso/tp-2017-1c-The-Kernels/mnt/FS_SADICA/tests/leeme.bin",0,4);
		char *hola="hola";
		if(*buffer==*hola){
			puts("piola test8");
		}
		else puts("error test8");
		free(buffer);
		//test8

		//test9 LEER con offset
		buffer=leerArchivo("/home/utnso/tp-2017-1c-The-Kernels/mnt/FS_SADICA/tests/leeme2.bin",5,4);
		if(*buffer==*hola){
		 	puts("piola test9");
		}
		else puts("error test9");
		free(buffer);
		//test9

		//test10 //Crea,escribe,lee(luego lo borra)
		crearArchivo("/home/utnso/tp-2017-1c-The-Kernels/mnt/FS_SADICA/tests/ParaEscribir.bin");
		escribirArchivo("/home/utnso/tp-2017-1c-The-Kernels/mnt/FS_SADICA/tests/ParaEscribir.bin",0,5,"piola");
		buffer=leerArchivo("/home/utnso/tp-2017-1c-The-Kernels/mnt/FS_SADICA/tests/ParaEscribir.bin",0,5);
		char *piola="piola";
		if(*buffer==*piola){
			puts("piola test10");
		}
		else puts("error test10");
		free(buffer);
		borrarArchivo("/home/utnso/tp-2017-1c-The-Kernels/mnt/FS_SADICA/tests/ParaEscribir.bin");
		//test10

		//test11 //Crea,escribe,lee(luego lo borra) (gran size)
		crearArchivo("/home/utnso/tp-2017-1c-The-Kernels/mnt/FS_SADICA/tests/ParaEscribir.bin");
		escribirArchivo("/home/utnso/tp-2017-1c-The-Kernels/mnt/FS_SADICA/tests/ParaEscribir.bin",0,10,"piolaaaaaa");
		buffer=leerArchivo("/home/utnso/tp-2017-1c-The-Kernels/mnt/FS_SADICA/tests/ParaEscribir.bin",0,10);
		piola="piolaaaaaa";
		if(*buffer==*piola){
			puts("piola test11");
		}
		else puts("error test11");
		free(buffer);
		borrarArchivo("/home/utnso/tp-2017-1c-The-Kernels/mnt/FS_SADICA/tests/ParaEscribir.bin");
		//test11

		//test12 //Crea,escribe,lee(luego lo borra) (gran size con offset)
		crearArchivo("/home/utnso/tp-2017-1c-The-Kernels/mnt/FS_SADICA/tests/ParaEscribir.bin");
		escribirArchivo("/home/utnso/tp-2017-1c-The-Kernels/mnt/FS_SADICA/tests/ParaEscribir.bin",0,10,"piolaaaaaa");
		escribirArchivo("/home/utnso/tp-2017-1c-The-Kernels/mnt/FS_SADICA/tests/ParaEscribir.bin",10,2,"xd");
		buffer=leerArchivo("/home/utnso/tp-2017-1c-The-Kernels/mnt/FS_SADICA/tests/ParaEscribir.bin",0,12);
		piola="piolaaaaaaxd";
		if(*buffer==*piola){
			puts("piola test12");
		}
		else puts("error test12");
		free(buffer);
		borrarArchivo("/home/utnso/tp-2017-1c-The-Kernels/mnt/FS_SADICA/tests/ParaEscribir.bin");
		//test12

		//test13 //Crea,escribe,lee(luego lo borra) (gran size con offset y sobreescritura)
		crearArchivo("/home/utnso/tp-2017-1c-The-Kernels/mnt/FS_SADICA/tests/ParaEscribir.bin");
		escribirArchivo("/home/utnso/tp-2017-1c-The-Kernels/mnt/FS_SADICA/tests/ParaEscribir.bin",0,10,"piolaaaaaa");
		escribirArchivo("/home/utnso/tp-2017-1c-The-Kernels/mnt/FS_SADICA/tests/ParaEscribir.bin",7,2,"xd");
		buffer=leerArchivo("/home/utnso/tp-2017-1c-The-Kernels/mnt/FS_SADICA/tests/ParaEscribir.bin",0,10);
		piola="piolaaaxda";
		if(*buffer==*piola){
			puts("piola test13");
		}
		else puts("error test13");
		free(buffer);
		borrarArchivo("/home/utnso/tp-2017-1c-The-Kernels/mnt/FS_SADICA/tests/ParaEscribir.bin");
		//test13
}
