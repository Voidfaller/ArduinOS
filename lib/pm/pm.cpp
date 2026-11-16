#include <pm.h>
#include <Arduino.h>
#include <mm.h>
#include <fs.h>
#include <str.h>
#include <EEPROM.h>

#define MAX_PROCESSES 10             // Maximum number of processes
Process processTable[MAX_PROCESSES]; // Array to hold processes
int noOfProcesses = 0;               // Number of processes
int pidCounter = 0;

void startProcess(int argc, char *argv[])
{
    if (argc < 2)
    {
        Serial.println(F("Usage: run <process_name> <priority>"));
        return;
    }

    if (noOfProcesses >= MAX_PROCESSES)
    {
        Serial.println(F("Process table is full."));
        return;
    }

    const char *Filename = argv[1];

    // Check if file exists
    File *file = getFile(Filename);
    if (file == nullptr)
    {
        Serial.println(F("File not found."));
        return;
    }

    int priority = 1; // Default priority
    if (argc > 2)
    {
        priority = atoi(argv[2]);
        if (priority < 1 || priority > 100)
        {
            Serial.println(F("Invalid priority. Must be between 1 and 100."));
            delete file;
            return;
        }
    }

    // Create and populate new process struct
    Process newProcess;
    strcopy(newProcess.name, file->name);
    newProcess.fp = -1;
    newProcess.fileSize = -1;
    newProcess.fc = 0;
    newProcess.rootAddress = file->location;
    newProcess.pc = 0;
    newProcess.state = 'R';
    newProcess.pid = pidCounter + 1;
    newProcess.loopAddress = 0;
    newProcess.priority = priority;

    processTable[noOfProcesses] = newProcess;
    noOfProcesses++;
    pidCounter++;

    delete file; // Clean up
}

int findProcessIndex(int pid)
{
    for (int i = 0; i < noOfProcesses; i++)
    {
        if (processTable[i].pid == pid)
        {
            return i; // Return the index of the process
        }
    }
    return -1; // Process not found
}

void updateProcessState(int pid, char newState)
{
    int index = findProcessIndex(pid);
    if (index == -1)
    {
        Serial.println(F("Process not found."));
        return;
    }
    if (processTable[index].state == 'T')
    {
        Serial.println(F("Cannot change state of a terminated process."));
        return;
    }
    if (newState != 'R' && newState != 'S' && newState != 'T')
    {
        Serial.println(F("Invalid state. Use 'R' for Running, 'S' for Suspended, or 'T' for Terminated."));
        return;
    }

    processTable[index].state = newState; // Update the state of the process
}

void suspendProcess(int argc, char *argv[])
{
    if (argc < 2)
    {
        Serial.println(F("Usage: suspend <pid>"));
        return;
    }

    int pid = atoi(argv[1]);
    findProcessIndex(pid);

    if (pid <= 0)
    {
        Serial.println(F("Invalid PID."));
        return;
    }
    updateProcessState(pid, 'S'); // Update the process state to Suspended
}

void resumeProcess(int argc, char *argv[])
{
    if (argc < 2)
    {
        Serial.println(F("Usage: resume <pid>"));
        return;
    }

    int pid = atoi(argv[1]);
    if (pid <= 0)
    {
        Serial.println(F("Invalid PID."));
        return;
    }
    updateProcessState(pid, 'R'); // Update the process state to Running
}

void killProcess(int argc, char *argv[])
{
    if (argc < 2)
    {
        Serial.println(F("Usage: kill <pid>"));
        return;
    }

    int pid = atoi(argv[1]);
    if (pid <= 0)
    {
        Serial.println(F("Invalid PID."));
        return;
    }

    int index = findProcessIndex(pid);
    if (index == -1)
    {
        Serial.println(F("Process not found."));
        return;
    }

    // Free process resources
    deleteVariables(pid);
    // Shift de rest van de processTable op
    for (int i = index; i < noOfProcesses - 1; i++)
    {
        processTable[i] = processTable[i + 1];
    }
    noOfProcesses--; // verklein het aantal actieve processen
}

void listProcesses(int argc, char *argv[])
{
    if (noOfProcesses == 0)
    {
        Serial.println(F("No processes running."));
        return;
    }

    Serial.println(F("Running Processes:"));
    for (int i = 0; i < noOfProcesses; i++)
    {
        Process p = processTable[i];
        Serial.print(F("PID: "));
        Serial.print(p.pid);
        Serial.print(F(", Name: "));
        Serial.print(p.name);
        Serial.print(F(", Priority: "));
        Serial.print(p.priority);
        Serial.print(F(", State: "));
        Serial.print(p.state);
        Serial.print(F(", PC: "));
        Serial.print(p.pc);
        Serial.print(F(", FP: "));
        Serial.println(p.fp);
    }
}
