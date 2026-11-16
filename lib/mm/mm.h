#include <Arduino.h>
//mm.c
#ifndef MM_H
#define MM_H


#define MEMORY_SIZE 256
#define STACK_SIZE 32
//define types for memory management

typedef struct{
    byte name;
    int pid;
    int type; // Type of the variable (CHAR, INT, FLOAT, STRING)
    int size; // Size of the variable in bytes
    void *address; 
} Variable;

typedef struct 
{
    byte sp = 0; // Stack pointer
    byte data[STACK_SIZE];
}Stack;


// TYPES:
// Char: 1
// Int: 2
// Float: 3
// String: 4



extern Variable memoryTable[25];
extern int noOfVars;

void createVariable(Stack* s, byte name, int pid);
void deleteVariables(int pid);
void listVariables();
void getVariable(Stack* s, byte name, int pid);
void pushString(Stack* s, const char *value);
void pushInt(Stack* s, int value);
void pushFloat(Stack* s, float value);
void pushChar(Stack* s, char value);
void pushByte(Stack* s, byte value);
void printStack(Stack* s);
byte popByte(Stack* s);
float popVal(Stack* s, int type);
char* popString(Stack* s, int length); 
byte peekStack(Stack* s);

#endif // MM_H