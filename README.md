# tp-2017-1c-The-Kernels

PROTOCOLO

UNIVERSAL:
0 - HANDSHAKE
-1 - DECONEXION ABRUPTA
104 - OK (Si quieren, viene de 10-4 xdDdDDDDDDdddDdDD)

CONSOLA-KERNEL:
1 - INICIAR PROCESO
9 - MATAR PROCESO

KERNEL-CONSOLA
-2: No hay espacio en memoria

KERNEL-MEMORIA:
1 - INICAR PROCESO

MEMORIA-KERNEL:
-2: No hay espacio en memoria

CPU-MEMORIA:
1 - CAMBIO DE PROCESO
2 - LEER
3 - ESCRIBIR
4 - DESCONEXION NORMAL DE CPU

CPU-KERNEL
1 - SE TERMINO LA EJECUCION DE UN PROCESO (NO HAY MAS INSTRUCCIONES)
