//pm.c
#include <mm.h>
#include <Arduino.h>
#ifndef PM_H
#define PM_H
void listProcesses(int argc, char *argv[]);
void killProcess(int argc, char *argv[]);
void startProcess(int argc, char *argv[]);
void suspendProcess(int argc, char *argv[]);
void resumeProcess(int argc, char *argv[]);
void updateProcessState(int pid, char newState);

typedef struct{
    int pid;
    int priority = 1; // Process priority
    char name[12]; // Process name
    char state; // Process state: 'R' for Running, 'S' for Suspended, 'T' for Terminated
    int fp; // File pointer
    int fc; // File count
    int fileSize; // File size
    int pc; // Program counter
    int rootAddress; // Root address for the process
    Stack stack;
    int loopAddress; // Address to jump to for loops
} Process;

extern Process processTable[10]; // Array to hold processes
extern int noOfProcesses; // Number of processes
extern int pidCounter;

#endif //PM_H