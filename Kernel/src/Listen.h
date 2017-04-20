#include "SocketLibrary.h"
#include "ConnectionCore.h"
#include "KernelConfiguration.h"
#include <commons/collections/list.h>


#define PUERTO "7171"
#define PUERTO2 "7172"
#define LOCALHOST "127.0.0.1"
#define BACKLOG 10

void handler(configFile* config);
void destroyConnHandler(connHandle*);
socketHandler updateSockets(connHandle);
connHandle initializeConnectionHandler();
void initialize(configFile*,t_list*, connHandle*);
