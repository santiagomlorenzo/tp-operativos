#ifndef __PROCESS__
#define __PROCESS__

#include "Configuration.h"
#include "globales.h"
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <parser/parser.h>
#include <parser/metadata_program.h>
#include <math.h>
#include "CapaFileSystem.h"
#include "ConnectionCore.h"


ProcessControl* PIDFind(int);
ProcessControl* PIDFindAndRemove(int);

PCB* fromNewToFinished(int pid);
PCB* fromNewToReady();
PCB* fromReadyToExecute();
PCB* fromReadyToFinished();
PCB* fromExecuteToReady(int);
PCB* fromExecuteToFinished();
PCB* fromExecuteToBlocked(int);
PCB* createProcess(char*, int);
PCB* fromBlockedToFinished(int);
PCB* fromBlockedToReady(int);
PCB* removePcbFromList(int,t_list*);
PCB* _fromTo(t_queue*, t_queue*, int);
PCB* _fromQueueToList(t_queue*, t_list*, int);
PCB* _fromQueueToQueue(t_queue*, t_queue*, int);
PCB* _fromListToList(t_list*, t_list*, int, int);
PCB* _fromListToQueue(t_list*, t_queue*, int PID, int);

void mostrarPaginaHeap(PageOwnership* po);
void killProcess(int,int);
void freeProcessPages(int);
void matarSiCorresponde(int pid);
void modifyProcessState(int, int);
void freePage(PageOwnership*, int, int);
void newProcess(PCB*, int, char*, int);
void destroyPageOwnership(PageOwnership*);
void destruirProcessControl(ProcessControl* pc);
void _processChangeStateToList(t_list*, PCB*, int);
void _processChangeStateToQueue(t_queue*, PCB*, int);

int replacePCBinQueue(PCB*,t_queue*);

#endif

