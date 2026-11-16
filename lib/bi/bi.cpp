#include<bi.h>
#include <ps.h>
#include <pm.h>
#include <Arduino.h>
#include <mm.h>
#include <fs.h>
#include <str.h>
#include <EEPROM.h>
#include <instruction_set.h>


void unaryOperation(Stack *s, uint8_t operation)
{
    uint8_t type = popByte(s); // Get the type of the value to operate on

    if (type == STRING)
    {
        Serial.println(F("Cannot perform unary operation on a string!"));
        return; // Handle unary operation on string case
    }

    float value = popVal(s, type); // Pop the value from the stack

    switch (operation)
    {

    case INCREMENT: // INCREMENT
        value++;
        break;
    case DECREMENT:
        value--;
        break;
    case UNARYMINUS:
        value = -value;
        break;
    case LOGICALNOT:
        if (type == FLOAT)
        {
            value = (value == 0) ? 1 : 0; // Logical NOT for float
        }
        else if (type == INT)
        {
            value = (value == 0) ? 1 : 0; // Logical NOT for int
        }
        else if (type == CHAR)
        {
            value = (value == 0) ? 1 : 0; // Logical NOT for char
        }
        break;
    case BITWISENOT:
        if (type == INT)
        {
            value = ~((int)value); // Bitwise NOT for int
        }
        else if (type == CHAR)
        {
            value = ~((char)value); // Bitwise NOT for char
        }
        else
        {
            Serial.println(F("Bitwise NOT only supports INT and CHAR types!"));
            return; // Handle type mismatch for bitwise NOT
        }
        break;

    case TOCHAR:
        if (type == INT)
        {
            value = (char)value; // Cast int to char
        }
        else if (type == FLOAT)
        {
            value = (char)value; // Cast float to char
        }
        else
        {
            Serial.println(F("TOCHAR only supports INT and FLOAT types!"));
            return; // Handle type mismatch for TOCHAR
        }
        break;
    case TOINT:
        if (type == CHAR)
        {
            value = (int)value; // Cast char to int
        }
        else if (type == FLOAT)
        {
            value = (int)value; // Cast float to int
        }
        else
        {
            Serial.println(F("TOINT only supports CHAR and FLOAT types!"));
            return; // Handle type mismatch for TOINT
        }
        break;
    case TOFLOAT:
        if (type == CHAR)
        {
            value = (float)value; // Cast char to float
        }
        else if (type == INT)
        {
            value = (float)value; // Cast int to float
        }
        else
        {
            Serial.println(F("TOFLOAT only supports CHAR and INT types!"));
            return; // Handle type mismatch for TOFLOAT
        }
        break;
    case ROUND:
        if (type == FLOAT)
        {
            value = round(value); // Round the float value
        }
        else if (type == INT)
        {
            value = round((float)value); // Round the int value as float
        }
        else if (type == CHAR)
        {
            value = round((float)value); // Round the char value as float
        }
        break;

    case FLOOR:
        if (type == FLOAT)
        {
            value = floor(value); // Round down the float value
        }
        else if (type == INT)
        {
            value = floor((float)value); // Round down the int value as float
        }
        else if (type == CHAR)
        {
            value = floor((float)value); // Round down the char value as float
        }
        break;
    case CEIL:
        if (type == FLOAT)
        {
            value = ceil(value); // Round up the float value
        }
        else if (type == INT)
        {
            value = ceil((float)value); // Round up the int value as float
        }
        else if (type == CHAR)
        {
            value = ceil((float)value); // Round up the char value as float
        }
        break;
    case ABS:
        if (type == FLOAT)
        {
            value = fabs(value); // Take the absolute value of the float
        }
        else if (type == INT)
        {
            value = abs((int)value); // Take the absolute value of the int
        }
        else if (type == CHAR)
        {
            value = abs((char)value); // Take the absolute value of the char
        }
        break;
    case SQ:
        if (type == FLOAT)
        {
            value = value * value; // Square the float value
        }
        else if (type == INT)
        {
            value = (int)value * (int)value; // Square the int value
        }
        else if (type == CHAR)
        {
            value = (char)value * (char)value; // Square the char value
        }
        break;
    case SQRT:
        if (value < 0)
        {
            Serial.println(F("Cannot take square root of a negative number!"));
            return; // Handle square root of negative number case
        }
        value = sqrt(value); // Take the square root of the value
        break;
    case ANALOGREAD:
    {
        if (type == STRING)
        {
            Serial.println(F("Analog read only supports INT type!"));
            return; // Handle type mismatch for analog read
        }
        int pin = (int)value;   // Cast the value to int for pin number
        if (pin < 0 || pin > 5) // Assuming valid analog pins are 0-15
        {
            Serial.println(F("Invalid pin number for analog read!"));
            return; // Handle invalid pin number
        }
        value = analogRead(pin); // Read the analog value from the specified pin
        break;
    }

    case DIGITALREAD:
    {
        if (type == STRING)
        {
            Serial.println(F("Digital read only supports INT type!"));
            return; // Handle type mismatch for digital read
        }
        int pin = (int)value;    // Cast the value to int for pin number
        if (pin < 0 || pin > 13) // Assuming valid digital pins are 0-13
        {
            Serial.println(F("Invalid pin number for digital read!"));
            return; // Handle invalid pin number
        }
        value = digitalRead(pin);
        break;
    }
    default:
        Serial.println(F("Unknown unary operation!"));
        return; // Handle unknown unary operation
    }

    if (type == INT)
    {
        pushInt(s, (int)value); // Push the value back onto the stack as an int
    }
    else if (type == FLOAT)
    {
        pushFloat(s, value); // Push the float value back onto the stack
    }
    else if (type == CHAR)
    {
        pushChar(s, (char)value); // Push the char value back onto the stack
    }
}

void binaryOperation(Stack *s, uint8_t operation)
{
    uint8_t type2 = popByte(s);
    float value2 = popVal(s, type2);
    uint8_t type1 = popByte(s);
    float value1 = popVal(s, type1);
    bool isBoolean = false; // Flag to check if the operation is boolean
    if (type1 == STRING || type2 == STRING)
    {
        Serial.println(F("Cannot perform binary operation on a string!"));
        return; // Handle binary operation on string case
    }

    float result;

    switch (operation)
    {
    case PLUS:
        result = value1 + value2; // Add the two values
        break;
    case MINUS:
        result = value1 - value2; // Subtract the two values
        break;
    case TIMES:
        result = value1 * value2; // Multiply the two values
        break;
    case DIVIDEDBY:
        if (value2 == 0)
        {
            Serial.println(F("Division by zero!"));
            return; // Handle division by zero case
        }
        result = value1 / value2; // Divide the two values
        break;
    case MODULUS:
        if (value2 == 0)
        {
            Serial.println(F("Modulus by zero!"));
            return; // Handle modulus by zero case
        }
        result = fmod(value1, value2); // Calculate the modulus of the two values
        break;
    case EQUALS:
        if (type1 == FLOAT || type2 == FLOAT)
        {
            result = (value1 == value2) ? 1.0f : 0.0f; // Check equality for float
        }
        else if (type1 == INT || type2 == INT)
        {
            result = ((int)value1 == (int)value2) ? 1.0f : 0.0f; // Check equality for int
        }
        else if (type1 == CHAR || type2 == CHAR)
        {
            result = ((char)value1 == (char)value2) ? 1.0f : 0.0f; // Check equality for char
        }
        else
        {
            Serial.println(F("EQUALS operation only supports INT, FLOAT, and CHAR types!"));
            return; // Handle type mismatch for EQUALS operation
        }
        isBoolean = true; // Set flag to indicate boolean result
        break;

    case NOTEQUALS:
        if (type1 == FLOAT || type2 == FLOAT)
        {
            result = (value1 == value2) ? 0.0f : 1.0f; // Check equality for float
        }
        else if (type1 == INT || type2 == INT)
        {
            result = ((int)value1 == (int)value2) ? 0.0f : 1.0f; // Check equality for int
        }
        else if (type1 == CHAR || type2 == CHAR)
        {
            result = ((char)value1 == (char)value2) ? 0.0f : 1.0f; // Check equality for char
        }
        else
        {
            Serial.println(F("EQUALS operation only supports INT, FLOAT, and CHAR types!"));
            return; // Handle type mismatch for EQUALS operation
        }
        isBoolean = true; // Set flag to indicate boolean result
        break;

    case LESSTHAN:
        if (type1 == FLOAT || type2 == FLOAT)
        {
            result = (value1 < value2) ? 1.0f : 0.0f; // Check less than for float
        }
        else if (type1 == INT || type2 == INT)
        {
            result = ((int)value1 < (int)value2) ? 1.0f : 0.0f; // Check less than for int
        }
        else if (type1 == CHAR || type2 == CHAR)
        {
            result = ((char)value1 < (char)value2) ? 1.0f : 0.0f; // Check less than for char
        }
        else
        {
            Serial.println(F("LESSTHAN operation only supports INT, FLOAT, and CHAR types!"));
            return; // Handle type mismatch for LESSTHAN operation
        }
        isBoolean = true; // Set flag to indicate boolean result
        break;
    default:
        Serial.println(F("Unknown binary operation!"));
        return; // Handle unknown binary operation
    }

    if (isBoolean)
    {
        pushByte(s, result);
    }
    else if (type1 == FLOAT || type2 == FLOAT)
    {
        pushFloat(s, result); // Push the result as a float if either operand is a float
    }
    else if (type1 == INT || type2 == INT)
    {
        pushInt(s, (int)result); // Push the result as an int if either operand is an int
    }
    else if (type1 == CHAR || type2 == CHAR)
    {
        pushChar(s, (char)result); // Push the result as a char if either operand is a char
    }
}

void execute(int processIndex)
{
    Process &process = processTable[processIndex];
    // Read the instruction from the process file
    uint8_t instruction = EEPROM.read(process.rootAddress + process.pc);

    // Uncomment the following lines for debugging
    Serial.print(F("Process ID: "));
    Serial.print(process.pid);
    Serial.print(F("Executing instruction: "));
    Serial.println(instruction, DEC); // Print the instruction in hexadecimal format
    switch (instruction)
    {

    case CHAR:
    {
        // Handle CHAR instruction
        byte charValue = EEPROM.read(process.rootAddress + process.pc + 1);
        pushChar(&process.stack, charValue);
        process.pc += 2; // Move to the next isnstruction
        break;
    }

    case INT:
    {
        // Handle INT instruction
        byte highByte = EEPROM.read(process.rootAddress + process.pc + 1); // Read low byte
        byte lowByte = EEPROM.read(process.rootAddress + process.pc + 2);  // Read high byte
        int intValue = (highByte << 8) | lowByte;                          // Combine bytes into an int
        pushInt(&process.stack, intValue);                                 // Push the int onto the stack
        process.pc += 3;                                                   // Move to the next instruction

        break;
    }

    case STRING:
    {
        char str[20];
        uint8_t i = 0;
        uint16_t readAddr = process.rootAddress + process.pc + 1;

        while (true)
        {
            byte charValue = EEPROM.read(readAddr++);
            if (charValue == '\0')
            {
                str[i] = '\0'; // Null-terminate
                break;
            }

            if (i >= sizeof(str) - 1)
            {
                Serial.println(F("String too long!"));
                return;
            }

            str[i++] = charValue;
        }

        pushString(&process.stack, str);

        // Update process.pc by amount of bytes read
        process.pc += 1 + i + 1; // opcode + string length + null terminator
        break;
    }

    case FLOAT:
    {
        union
        {
            byte b[4];
            float f;
        } floatData;

        for (int i = 0; i < 4; i++)
        {
            floatData.b[3 - i] = EEPROM.read(process.rootAddress + process.pc + 1 + i);
        }

        pushFloat(&process.stack, floatData.f);

        process.pc += 5;
        break;
    }

    case SET:
    {
        char varName = EEPROM.read(process.rootAddress + process.pc + 1); // Read variable name
        process.pc += 1;                                                  // Move to the next instruction
        int pid = process.pid;                                            // Get the process ID
        createVariable(&process.stack, varName, pid);                     // Create a variable in memory
        process.pc += 1;
        break;
    }

    case GET:
    {
        process.pc += 1;                                              // Move to the next instruction
        char varName = EEPROM.read(process.rootAddress + process.pc); // Read variable name
        getVariable(&process.stack, varName, process.pid);            // Get the variable from memory and push it onto the stack
        process.pc += 1;                                              // Move to the next instruction
        break;
    }

    case INCREMENT:
    {
        unaryOperation(&process.stack, INCREMENT);
        process.pc += 1; // Move to the next instruction
        break;
    }

    case DECREMENT:
    {
        unaryOperation(&process.stack, DECREMENT);
        process.pc += 1; // Move to the next instruction
        break;
    }

    case PLUS:
    {
        binaryOperation(&process.stack, PLUS); // Perform addition operation
        process.pc += 1;
        break;
    }

    case MINUS:
    {
        binaryOperation(&process.stack, MINUS); // Perform subtraction operation
        process.pc += 1;                        // Move to the next instruction
        break;
    }

    case TIMES:
    {
        binaryOperation(&process.stack, TIMES); // Perform multiplication operation
        process.pc += 1;                        // Move to the next instruction
        break;
    }

    case DIVIDEDBY:
    {
        binaryOperation(&process.stack, DIVIDEDBY); // Perform division operation
        process.pc += 1;                            // Move to the next instruction
        break;
    }

    case MODULUS:
    {
        binaryOperation(&process.stack, MODULUS); // Perform modulus operation
        process.pc += 1;                          // Move to the next instruction
        break;
    }

    case UNARYMINUS:
    {
        unaryOperation(&process.stack, UNARYMINUS); // Perform unary minus operation
        process.pc += 1;                            // Move to the next instruction
        break;
    }

    case EQUALS:
    {
        binaryOperation(&process.stack, EQUALS); // Perform equality check operation
        process.pc += 1;                         // Move to the next instruction
        break;
    }

    case NOTEQUALS:
    {
        binaryOperation(&process.stack, NOTEQUALS); // Perform inequality check operation
        process.pc += 1;                            // Move to the next instruction
        break;
    }

    case LESSTHAN:
    {
        binaryOperation(&process.stack, LESSTHAN); // Perform less than operation
        process.pc += 1;                           // Move to the next instruction
        break;
    }

    case LOGICALNOT:
    {
        unaryOperation(&process.stack, LOGICALNOT); // Perform logical NOT operation
        process.pc += 1;                            // Move to the next instruction
        break;
    }

    case BITWISENOT:
    {
        unaryOperation(&process.stack, BITWISENOT); // Perform bitwise NOT operation
        process.pc += 1;                            // Move to the next instruction
        break;
    }
    case TOCHAR:
    {
        unaryOperation(&process.stack, TOCHAR); // Perform TOCHAR operation
        process.pc += 1;                        // Move to the next instruction
        break;
    }
    case TOINT:
    {
        unaryOperation(&process.stack, TOINT); // Perform TOINT operation
        process.pc += 1;                       // Move to the next instruction
        break;
    }
    case TOFLOAT:
    {
        unaryOperation(&process.stack, TOFLOAT); // Perform TOFLOAT operation
        process.pc += 1;                         // Move to the next instruction
        break;
    }
    case ROUND:
    {
        unaryOperation(&process.stack, ROUND); // Perform ROUND operation
        process.pc += 1;                       // Move to the next instruction
        break;
    }
    case FLOOR:
    {
        unaryOperation(&process.stack, FLOOR); // Perform FLOOR operation
        process.pc += 1;                       // Move to the next instruction
        break;
    }
    case CEIL:
    {
        unaryOperation(&process.stack, CEIL); // Perform CEIL operation
        process.pc += 1;                      // Move to the next instruction
        break;
    }

    case ABS:
    {
        unaryOperation(&process.stack, ABS);
        process.pc += 1;
        break;
    }

    case SQ:
    {
        unaryOperation(&process.stack, SQ);
        process.pc += 1; // Move to the next instruction
        break;
    }

    case SQRT:
    {
        unaryOperation(&process.stack, SQRT);
        process.pc += 1; // Move to the next instruction
        break;
    }

    case DELAY:
    {
        uint8_t type = popByte(&process.stack);         // Get the type of the value to delay
        float delayTime = popVal(&process.stack, type); // Pop the value from the stack
        delay(delayTime);
        break;
    }

    case DELAYUNTIL:
    {
        uint8_t type = popByte(&process.stack);         // Get the type of the value to delay until
        float delayTime = popVal(&process.stack, type); // Pop the value from the stack
        float currentTime = (float)millis(); // Get the current time in milliseconds
        if (delayTime > currentTime)
        {
            pushFloat(&process.stack, delayTime);
        }
        else
        {
            process.pc += 1;
        }

        break;
    }

    case MILLIS:
    {
        pushFloat(&process.stack, millis());
        process.pc += 1; // Move to the next instruction
        break;
    }

    case PINMODE:
    {
        uint8_t type = popByte(&process.stack);            // Get the type of the value to set pin mode
        float pinModeValue = popVal(&process.stack, type); // Pop the pin mode value from the stack

        // Cast the float to int for pinMode (0 = INPUT, 1 = OUTPUT, 2 = INPUT_PULLUP)
        int mode = static_cast<int>(pinModeValue);

        // Pop the pin number from the stack
        uint8_t pinType = popByte(&process.stack);
        int pin = static_cast<int>(popVal(&process.stack, pinType));

        if (mode == 0 || mode == 1 || mode == 2)
        {
            pinMode(pin, mode);
            Serial.print(F("Pin "));
            Serial.print(pin);
            Serial.print(F(" set to mode "));
            if (mode == 0)
            {
                Serial.println(F("INPUT"));
            }
            else if (mode == 1)
            {
                Serial.println(F("OUTPUT"));
            }
            else if (mode == 2)
            {
                Serial.println(F("INPUT_PULLUP"));
            }
        }
        else
        {
            Serial.println(F("Invalid pin mode!"));
        }
        process.pc += 1;
        break;
    }
    case ANALOGREAD:
    {
        unaryOperation(&process.stack, ANALOGREAD); // Perform analog read operation
        process.pc += 1;                            // Move to the next instruction
        break;
    }

    case DIGITALREAD:
    {
        unaryOperation(&process.stack, DIGITALREAD); // Perform digital read operation
        process.pc += 1;                             // Move to the next instruction
        break;
    }

    case DIGITALWRITE:
    {
        uint8_t type = popByte(&process.stack);        // Get the value to write
        float pinValue = popVal(&process.stack, type); // Pop the value from the stack

        // Cast the float to int for pin value
        int valueToWrite = static_cast<int>(pinValue);

        uint8_t pinType = popByte(&process.stack);                   // Get the type of the pin number
        int pin = static_cast<int>(popVal(&process.stack, pinType)); // Pop the pin number from the stack

        if (valueToWrite == 0 || valueToWrite == 1)
        {
            digitalWrite(pin, valueToWrite);
        }
        else
        {
            Serial.println(F("Invalid value for digital write!"));
        }
        process.pc += 1; // Move to the next instruction
        break;
    }

    case PRINT:
    {
        uint8_t type = popByte(&process.stack); // Get the type of the value to print
        if (type == STRING)
        {
            uint8_t length = popByte(&process.stack);      // Get the length of the string
            char *str = popString(&process.stack, length); // Pop the string from the stack
            if (str == nullptr)
            {
                Serial.println(F("Failed to pop string from stack!"));
                return; // Handle string pop failure
            }
            if (str == NULL)
            {
                Serial.println(F("String is NULL!"));
                free(str); // Free the temporary string memory
                return;    // Handle NULL string case
            }
            Serial.print(str); // Print the string
        }
        else if (type == CHAR || type == INT || type == FLOAT)
        {
            Serial.print(popVal(&process.stack, type)); // Pop the value from the and print it
        }
        else
        {
            Serial.println(F("Unknown type to print!"));
        }
        process.pc += 1; // Move to the next instruction
        break;
    }

    case PRINTLN:
    {
        uint8_t type = popByte(&process.stack); // Get the type of the value to print
        if (type == STRING)
        {
            uint8_t length = popByte(&process.stack); // Get the length of the string
            Serial.println(popString(&process.stack, length));
        }
        else if (type == INT || type == FLOAT)
        {
            Serial.println(popVal(&process.stack, type)); // Pop the value from the stack and print it
        }
        else if (type == CHAR)
        {
            char charValue = popVal(&process.stack, type); // Pop the char value from the stack
            Serial.println(charValue);                     // Print the char value
        }
        else
        {
            Serial.println(F("Unknown type to print!"));
        }
        process.pc += 1; // Move to the next instruction
        break;
    }

    case OPEN:
    {
        uint8_t intType = popByte(&process.stack); // Get the type of the value to open

        float size = popVal(&process.stack, intType); // Pop the size of the file to open

        if (size < 0)
        {
            Serial.println(F("File size cannot be negative!"));
            break; // Handle negative file size case
        }

        uint8_t stringType = popByte(&process.stack); // Get the type of the value to open
        if (stringType != STRING)
        {
            Serial.println(F("OPEN instruction requires a STRING type!"));
            break; // Handle type mismatch for OPEN instruction
        }
        char *fileName = popString(&process.stack, popByte(&process.stack)); // Pop the file name from the stack

        if (fileName == nullptr)
        {
            Serial.println(F("Failed to pop file name from stack!"));
            return; // Handle file name pop failure
        }

        int no_of_files = EEPROM.read(NO_OF_FILES_ADDR); // Get the number of files in the file system
        bool fileExists = false;
        // Check if the file already exists
        for (int i = 0; i < no_of_files; i++)
        {
            File existingEntry;
            readFATEntry(i, &existingEntry);
            if (strcompare(existingEntry.name, fileName))
            {
                process.fp = existingEntry.location;
                process.fc = 0;                          // Set the file count to zero
                process.fileSize = existingEntry.length; // Set the file length
                Serial.print(F("File already exists!"));
                fileExists = true;
                break;
            }
        }

        if (!fileExists)
        {
            char sizeBuffer[12];
            itoa((int)size, sizeBuffer, 10); // Convert size to string

            char *argv[4];
            argv[0] = (char *)"createFile"; // Command to create a file
            argv[1] = fileName;             // File name
            argv[2] = sizeBuffer;           // File size as string
            argv[3] = (char *)"";           // Null-terminate the argument list
            createFile(4, argv);

            no_of_files = EEPROM.read(NO_OF_FILES_ADDR); // Update the number of files after creation

            for (int i = 0; i < no_of_files; i++)
            {
                File existingEntry;
                readFATEntry(i, &existingEntry);
                if (strcompare(existingEntry.name, fileName))
                {
                    process.fp = existingEntry.location;     // Set the file pointer to the new file location
                    process.fc = 0;                          // Set the file count to zero
                    process.fileSize = existingEntry.length; // Set the file length
                    break;
                }
            }
        }

        Serial.print(F("Opening file with size: "));
        Serial.println(size);
        Serial.print(F("File name: "));
        Serial.println(fileName);
        process.pc += 1;
        break;
    }

    case WRITE:
    {
        Serial.print(F("Writing at absolute address: "));
        Serial.println(process.fp + process.fc, DEC); // absolute EEPROM address

        uint8_t type = popByte(&process.stack);
        Serial.print(F("Type to write: "));
        Serial.println(type, DEC); // Print the type to write
        int size = 0;
        if (type == STRING)
        {
            uint8_t length = popByte(&process.stack);
            char *str = popString(&process.stack, length);
            if (!str)
            {
                Serial.println(F("Failed to pop string from stack!"));
                return;
            }

            if (process.fc + length > process.fileSize)
            {
                Serial.println(F("Write exceeds file bounds!"));
                free(str);
                return;
            }

            for (int i = 0; i < length; i++)
                EEPROM.write(process.fp + process.fc + i, str[i]);

            EEPROM.write(process.fp + process.fc + length, '\0'); // Null-terminate the string in EEPROM
            process.fc += length + 1;
            free(str);
        }

        else if (type == INT || type == FLOAT || type == CHAR)
        {
            size = (type == FLOAT) ? 4 : (type == INT ? 2 : 1);
            float value = popVal(&process.stack, type);

            Serial.print(F("Value to write: "));
            Serial.println(value, DEC); // Print the value to write

            if (process.fc + size > process.fileSize)
            {
                Serial.println(F("Write exceeds file bounds!"));
                return;
            }

            if (type == CHAR)
            {
                EEPROM.write(process.fp + process.fc, (char)value); // Write char value to EEPROM
                Serial.print(F("Writing char: "));
                Serial.println((char)value);
            }
            else if (type == INT)
            {
                EEPROM.write(process.fp + process.fc, (int)value & 0xFF);            // Write low byte
                EEPROM.write(process.fp + process.fc + 1, ((int)value >> 8) & 0xFF); // Write high byte
                Serial.print(F("Writing int: "));
                Serial.println((int)value);
            }
            else if (type == FLOAT)
            {
                union
                {
                    byte b[4];
                    float f;
                } floatData;
                floatData.f = value;
                for (int i = 0; i < 4; i++)
                    EEPROM.write(process.fp + process.fc + i, floatData.b[i]); // Write float bytes to EEPROM
                Serial.print(F("Writing float: "));
                Serial.println(value);
            }
            process.fc += size; // Update file count
        }
        else
        {
            Serial.println(F("Unknown type to write!"));
        }

        process.pc += 1; // advance PC
        break;
    }

    case READINT:
    {
        int value = 0;
        if (process.fc + 2 > process.fileSize)
        {
            Serial.println(F("Read exceeds file bounds!"));
            return; // Handle read exceeding file bounds
        }
        value = EEPROM.read(process.fp + process.fc) | (EEPROM.read(process.fp + process.fc + 1) << 8); // Read two bytes as int
        process.fc += 2;                                                                                // Increment the file count
        pushInt(&process.stack, value);                                                                 // Push the int onto the stack
        process.pc += 1;                                                                                // Move to the next instruction
        break;
    }

    case READCHAR:
    {
        char value = 0;
        if (process.fc >= process.fileSize)
        {
            Serial.println(F("Read exceeds file bounds!"));
            return; // Handle read exceeding file bounds
        }
        value = EEPROM.read(process.fp + process.fc); // Read a single byte as char
        process.fc++;                                 // Increment the file count
        pushChar(&process.stack, value);              // Push the char onto the stack
        process.pc += 1;                              // Move to the next instruction
        break;
    }
    case READFLOAT:
    {
        if (process.fc + 4 > process.fileSize)
        {
            Serial.println(F("Read exceeds file bounds!"));
            return; // Handle read exceeding file bounds
        }
        union
        {
            byte b[4];
            float f;
        } floatData;

        for (int i = 0; i < 4; i++)
        {
            floatData.b[i] = EEPROM.read(process.fp + process.fc + i); // Read four bytes as float
        }
        process.fc += 4;                        // Increment the file count
        pushFloat(&process.stack, floatData.f); // Push the float onto the stack
        process.pc += 1;                        // Move to the next instruction
        break;
    }
    case READSTRING:
    {
        char c = 0;
        char str[20] = ""; // Initialize an empty string
        int idx = 0;       // Index for the string
        do
        {
            c = EEPROM.read(process.fp + process.fc); // Read a character from the file
            process.fc++;                             // Increment the file count
            if (idx < sizeof(str) - 1)
            {
                str[idx++] = c; // Add the character to the string
            }
            else
            {
                Serial.println(F("String too long!"));
                break; // Handle string overflow case
            }
        } while (c != '\0' && process.fc < process.fileSize);

        str[idx] = '\0';                 // Null-terminate the string
        pushString(&process.stack, str); // Push the string onto the stack
        process.pc += 1;                 // Move to the next instruction
        break;
    }
    case IF:
    {
        uint8_t jumpAddress = EEPROM.read(process.rootAddress + process.pc + 1); // Read the jump address
        bool condition = popByte(&process.stack);
        process.pc += 2; // Move to the next instruction
        if (!condition)  // If the condition is true
        {
            process.pc += jumpAddress; // Jump to the specified address
        }
        pushByte(&process.stack, condition ? 1 : 0); // Push the condition result onto the stack
        break;
    }

    case ELSE:
    {
        uint8_t jumpAddress = EEPROM.read(process.rootAddress + process.pc + 1); // Read the jump address
        bool condition = popByte(&process.stack);                                // Pop the condition from the stack
        process.pc += 2;                                                         // Move to the next instruction
        if (condition)                                                           // If the condition is false
        {
            process.pc += jumpAddress; // Jump to the specified address
        }
        pushByte(&process.stack, condition ? 1 : 0); // Push the negated condition result onto the stack
        break;
    }

    case ENDIF:
    {
        // End of IF block, just move to the next instruction
        popByte(&process.stack); // Pop the condition from the stack
        process.pc += 1;         // Move to the next instruction
        break;
    }

    case WHILE:
    {
        bool condition = popByte(&process.stack); // Pop the condition from the stack
        process.pc += 1;
        uint8_t negativeJumpAddress = process.pc - EEPROM.read(process.rootAddress + process.pc);

        process.pc += 1;
        uint8_t positiveJumpAddress = EEPROM.read(process.rootAddress + process.pc); // Read the positive jump address
        if (condition)
        {
            pushByte(&process.stack, negativeJumpAddress); // Push the negative jump address onto the stack
            process.pc += 1;
        }

        else
        {
            process.pc += positiveJumpAddress + 2; // Jump to the specified address if the condition is false
        }
        break;
    }

    case ENDWHILE:
    {
        process.pc = popByte(&process.stack) - 1; // Jump back to the loop address
        break;
    }
    case LOOP:
    {

        process.loopAddress = process.pc; // Read the jump address
        process.pc += 1;                  // Move to the next instruction
        break;
    }

    case ENDLOOP:
    {
        process.pc = process.loopAddress; // Jump back to the loop address
        break;
    }

    case STOP:
    {

        updateProcessState(process.pid, 'T'); // Terminate the process
        process.pc += 1;                      // Move to the next instruction
        break;
    }

    case FORK:
    {
        int argc = 0;
        char *argv[12];                         // Array to hold the arguments for the new process
        uint8_t type = popByte(&process.stack); // Get the type of the value to fork
        if (type != STRING)
        {
            Serial.println(F("FORK instruction requires a STRING type!"));
            return; // Handle type mismatch for FORK instruction
        }

        char *command = popString(&process.stack, popByte(&process.stack)); // Pop the command string from the stack

        if (command == nullptr)
        {
            Serial.println(F("Failed to pop command from stack!"));
            return; // Handle command pop failure
        }
        argv[argc++] = (char *)"run";
        argv[argc++] = command; // Add the command to the arguments array

        startProcess(argc, argv); // Start a new process with the command
        Serial.print(F("Forked new process with command: "));
        Serial.println(command);             // Print the command of the new process
        pushInt(&process.stack, pidCounter); // Push the new process ID onto the stack
        process.pc += 1;                     // Move to the next instruction
    }

    case WAITUNTILDONE:
    {
        uint8_t type = popByte(&process.stack);        // Get the type of the value to wait for
        float pidValue = popVal(&process.stack, type); // Pop the process ID from the stack

        int pid = static_cast<int>(pidValue); // Cast the float to int for process
        pushInt(&process.stack, pid);         // Push the process ID back onto the stack
        for (int i = 0; i < noOfProcesses; i++)
        {
            if (processTable[i].pid == pid)
            {
                if (processTable[i].state == 'T')
                {
                    process.pc += 1;
                    break; // Exit the loop if the process is already terminated
                }
                else
                {
                    return; // Wait until the process is done
                }
            }
        }
        break;
    }
    default:
    {
        Serial.print(F("Unknown instruction: 0x"));
        Serial.println(instruction, HEX);
        // Handle unknown instruction
        process.pc += 1; // Move to the next instruction
        break;
    }
    }
    printStack(&process.stack); // Print the stack for debugging
}