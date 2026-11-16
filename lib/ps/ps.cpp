#include <ps.h>
#include <pm.h>
#include <Arduino.h>
#include <mm.h>
#include <fs.h>
#include <str.h>
#include <EEPROM.h>
#include <bi.h>
#include <instruction_set.h>

void runProcesses()
{
    for (int i = 0; i < noOfProcesses; i++)
    {
        Process &process = processTable[i];
        // Check if process is finished
        if (process.state == 'T')
        {
            // prepare args for killProcess
            char pidStr[12];
            itoa(process.pid, pidStr, 10);
            char *args[] = {nullptr, pidStr}; // Create an array of arguments for killProcess
            killProcess(2, args);             // Call killProcess to terminate the process
            continue;                         // Skip terminated processes
        }
        // Check if process is running
        if (process.state == 'R')
        {
            for (int j = 0; j < process.priority; j++)
            {
                execute(i);
            }

            // Execute the process instruction
        }
        // Check if process is suspended
        else if (process.state == 'S')
        {
            // Do nothing, just skip the process
            continue;
        }
    }
}