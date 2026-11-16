#include <Arduino.h>
#include <str.h>
#include <cli.h>
#include <fs.h>
#include <ps.h>
#include <EEPROM.h>

int buffer_index = 0;

bool binaryMode = false; // Flag to indicate if the CLI is in binary mode
int binaryBytesLeft = 0; // Number of bytes left in binary mode
File currentFile;        // Current file being processed in binary mode

char cli_buffer[CLI_BUFFER_SIZE]; // Static buffer for CLI input

void cliInit()
{
    Serial.begin(9600); // Initialize serial communication at 9600 baud rate
    resetCLIBuffer();   // Reset the CLI buffer
}

void helpCommand(int argc, char *argv[])
{

    Serial.println(F("Available commands:"));
    for (uint8_t i = 0; i < COMMAND_COUNT; i++)
    {
        Serial.print(F("-"));
        Serial.println(commands[i].name);
    }
}

void resetCLIBuffer()
{
    for (int i = 0; i < CLI_BUFFER_SIZE; i++)
    {
        cli_buffer[i] = '\0'; // Reset each character to null terminator
    }
    buffer_index = 0; // Reset the buffer index to 0
}

void readCLIBinary()
{
    while (Serial.available() > 0 && binaryBytesLeft > 0)
    {
        byte incomingByte = Serial.read();

        int writeAddr = currentFile.location + (currentFile.length - binaryBytesLeft);
        EEPROM.write(writeAddr, incomingByte);

        binaryBytesLeft--;

        // Optionally echo a progress indicator:
    }

    // Once done:
    if (binaryBytesLeft <= 0)
    {
        // Write the new file entry to the FAT
        int no_of_files = EEPROM.read(NO_OF_FILES_ADDR);
        writeFATEntry(no_of_files, &currentFile);
        EEPROM.write(NO_OF_FILES_ADDR, no_of_files + 1);

        Serial.println();
        Serial.print(F("File stored: "));
        Serial.println(currentFile.name);

        binaryMode = false; // Exit binary mode
    }
}

void readCLINormal()
{
    if (Serial.available() > 0)
    {
        char c = Serial.read(); // Read a character from the serial port
        Serial.print(c);        // Echo the character back to the serial monitor

        if (c == '\n')
        {                     // Check for newline or carriage return
            processCommand(); // Process the command if a newline is detected
            resetCLIBuffer(); // Reset the CLI buffer after processing
        }
        else if (c != '\r') // Ignore carriage return
        {
            // Check for backspace (ASCII 8) to delete the last character
            if (c == 8 && buffer_index > 0)
            {
                buffer_index--;                  // Decrease the buffer index to remove the last character
                cli_buffer[buffer_index] = '\0'; // Null-terminate the string
                Serial.print(F(" \b"));          // Rewrite character with empty space
            }
            else if (c == 127 && buffer_index > 0) // Handle delete key (ASCII 127)
            {
                buffer_index--;                  // Decrease the buffer index to remove the last character
                cli_buffer[buffer_index] = '\0'; // Null-terminate the string
            }

            else if (c >= 32 && c <= 126) // Check for printable characters
            {
                // Add the character to the CLI buffer if it's not full
                if (buffer_index < CLI_BUFFER_SIZE - 1)
                {
                    cli_buffer[buffer_index++] = c;  // Add the character to the buffer
                    cli_buffer[buffer_index] = '\0'; // Null-terminate the string
                }
            }
        }
    }
}

void readCLI()
{
    if (binaryMode)
    {
        readCLIBinary(); // Read binary data if in binary mode
    }
    else
    {
        readCLINormal(); // Read normal CLI input
    }
}

char *tokens[MAX_TOKENS]; // Array to hold token pointers
int tokenizeBuffer(char *buffer)
{
    int tokenCount = 0;
    bool inToken = false;
    bool inString = false;

    for (int i = 0; buffer[i] != '\0'; i++)
    {
        if (buffer[i] == '"')
        {
            buffer[i] = '\0'; // Remove the quote from the input
            inString = !inString;

            if (!inToken)
            {
                tokens[tokenCount++] = &buffer[i + 1]; // Start of quoted token
                inToken = true;
            }

            continue;
        }

        if (buffer[i] == ' ' && !inString)
        {
            buffer[i] = '\0'; // Split token on space (but only outside quotes)
            inToken = false;
        }
        else if (!inToken)
        {
            tokens[tokenCount++] = &buffer[i]; // Start of a new token
            inToken = true;
        }
    }

    return tokenCount;
}

void processCommand()
{
    int argc = tokenizeBuffer(cli_buffer);
    if (argc == 0)
        return;

    const char *cmd = tokens[0];

    for (uint8_t i = 0; i < COMMAND_COUNT; i++)
    {
        if (strcompare(cmd, commands[i].name))
        {
            commands[i].function(argc, tokens); // Call matching command
            return;
        }
    }
    Serial.println(F("Unknown command"));
}
