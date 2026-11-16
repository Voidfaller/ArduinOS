#include <pm.h>
#include <ps.h>
#include <fs.h>
#include<global_var.h>
#ifndef CLI_H
#define CLI_H

#define MAX_TOKENS 8 // max number of tokens in the CLI command
#define CLI_BUFFER_SIZE 64               // max number of characters in the CLI buffer
extern char cli_buffer[CLI_BUFFER_SIZE]; // Static buffer for CLI input



// Define a struct for commands
#define COMMAND_COUNT (sizeof(commands) / sizeof(Command)) // Number of commands

// Function prototypes
void readCLI();        // Function to check and read the CLI buffer
void processCommand();  // Function to process the command in the CLI buffer
void resetCLIBuffer(); // Function to reset the CLI buffer
void cliInit();        // Function to initialize the CLI
void helpCommand(int argc, char *argv[]);    // Function to display help information

typedef void (*CommandFunction)(int argc, char *argv[]); // Define a function pointer type for command functions


typedef struct
{
    const char name[12];
    CommandFunction function;
} Command;

// Declare the commands array
const Command commands[] = {
    {"help", helpCommand}, // Help command
    {"ls", listFiles},
    {"nano", createFile},
    {"rm", deleteFile},
    {"cat", readFile},
    {"diskfree", getFreeSpace},
    {"run", startProcess},
    {"suspend", suspendProcess},
    {"resume", resumeProcess},
    {"kill", killProcess},
    {"ps", listProcesses},
    {"store", storeFile},
    {"reset", factoryReset},
    {"catBin", readByteFile}
    
};
#endif // CLI_H