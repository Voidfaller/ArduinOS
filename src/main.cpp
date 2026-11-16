// ArduinOS 0.1 - A simple operating system for Arduino
#include <Arduino.h>
#include <cli.h>
#include <fs.h>
#include <mm.h>
#include <pm.h>
#include <ps.h>

#define VERSION "1.0"
void setup()
{
    fsInit();  // Initialize the filesystem
    cliInit(); // Initialize the CLI
    Serial.print(F("ArduinOS "));
    Serial.print(F(VERSION));
    Serial.println(F(" - A simple operating system for Arduino"));
    Serial.println(F("Type 'help' for a list of commands."));
}

void loop()
{
    readCLI();
    runProcesses();
}