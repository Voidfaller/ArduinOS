#include <mm.h>
#include <Arduino.h>
#include <stdlib.h>
#include <instruction_set.h>

int noOfVars = 0;         // Initialize the number of variables
Variable memoryTable[25]; // Array to hold variables
byte memory[256];
byte heapPointer = 0;

extern int __heap_start, *__brkval;
void printMemoryInfo()
{
    int v;
    Serial.print(F("Free RAM: "));
    Serial.println((int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval));
    Serial.print(F("Stack ptr: "));
    Serial.println((int)&v, HEX);
    Serial.print(F("Heap ptr (__brkval): "));
    Serial.println((unsigned int)__brkval, HEX);
}

void pushByte(Stack *s, byte value)
{
    if (s->sp >= STACK_SIZE)
    {
        Serial.println(F("Stack overflow!"));
        return; // Handle overflow
    }
    s->data[s->sp++] = value;
}

byte popByte(Stack *s)
{
    if (s->sp <= 0)
    {
        Serial.println(F("Stack underflow!"));
        return 0; // Handle underflow
    }
    return s->data[--s->sp]; // Decrement stack pointer and return the top value
}

void pushInt(Stack *s, int value)
{
    pushByte(s, highByte(value));
    pushByte(s, lowByte(value));
    // push the data type
    pushByte(s, INT);
}

void pushFloat(Stack *s, float value)
{
    byte *b = (byte *)&value;
    pushByte(s, b[0]); // Least significant byte first
    pushByte(s, b[1]);
    pushByte(s, b[2]);
    pushByte(s, b[3]); // Most significant byte last
    pushByte(s, FLOAT);
}

void pushChar(Stack *s, char value)
{
    // check if the stack has enough space for the char and the data type
    if (s->sp + 2 >= STACK_SIZE)
    {
        Serial.println(F("Stack overflow!"));
        return; // Handle overflow
    }

    pushByte(s, value); // Push the char value
    // push the data type
    pushByte(s, CHAR);
}

void pushString(Stack *s, const char *value)
{
    int length = strlen(value); // length excludes the null terminator

    // Check if the stack has enough space for the string, length byte, and type byte
    if (s->sp + length + 2 >= STACK_SIZE)
    {
        Serial.println(F("Stack overflow!"));
        return; // Handle overflow
    }

    // Push each character of the string (without null terminator)
    for (int i = 0; i < length; i++)
    {
        pushByte(s, value[i]);
    }

    pushByte(s, length); // Push the length of the string (without null terminator)
    pushByte(s, STRING); // Push the data type
}

char *popString(Stack *s, int length)
{
    if (s->sp == 0)
    {
        Serial.println(F("Stack underflow!"));
        return nullptr; // Handle underflow
    }
    if (length <= 0 || length > s->sp)
    {

        Serial.println(F("Invalid string length!"));
        Serial.print(F("Length: "));
        Serial.println(length);
        return nullptr; // Handle invalid length
    }

    char *str = (char *)&s->data[s->sp - length]; // Get the string from the stack
    s->data[s->sp] = '\0';                        // Null-terminate the string

    s->sp -= length; // Adjust stack pointer to account for the string and its length byte
    return str;
}

int popInt(Stack *s)
{
    byte low = popByte(s);
    byte high = popByte(s);
    return word(high, low);
}

float popFloat(Stack *s)
{
    byte b[4];
    b[3] = popByte(s);
    b[2] = popByte(s);
    b[1] = popByte(s);
    b[0] = popByte(s);
    float value;
    memcpy(&value, b, sizeof(float));
    return value;
}

float popVal(Stack *s, int type)
{
    switch (type)
    {
    case CHAR: // Char
        return (float)popByte(s);
    case INT: // Int
        return (float)popInt(s);

    case FLOAT: // Float
        return popFloat(s);
        Serial.println(F("Popped a float value from the stack."));
        break;

    default:
        break;
    }

    return 0.0f; // Default return value for unsupported types
}

byte peekStack(Stack *s)
{
    if (s->sp == 0)
    {
        Serial.println(F("Stack is empty!"));
        return 0; // Handle empty stack
    }
    return s->data[s->sp - 1]; // Return the top value without popping it
}

int freeRam()
{
    extern int __heap_start, *__brkval;
    int v;
    return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}

// function to push variable from the stack to heap memory
void createVariable(Stack *s, byte name, int pid)
{
    if (noOfVars >= 25)
    {
        Serial.println(F("Memory table is full!"));
        return; // Handle memory table overflow
    }

    // Check if variable already exists
    for (int i = 0; i < noOfVars; i++)
    {
        Variable &existing = memoryTable[i];
        if (existing.name == name && existing.pid == pid)
        {
            // Overwrite existing variable
            int type = popByte(s);

            if (type != existing.type)
            {
                Serial.println(F("Type mismatch on SET!"));
                return;
            }

            if (type == FLOAT || type == INT || type == CHAR)
            {
                float value = popVal(s, type);
                if (type == FLOAT)
                    *(float *)existing.address = value;
                else if (type == INT)
                    *(int *)existing.address = (int)value;
                else if (type == CHAR)
                    *(char *)existing.address = (char)value;
            }
            else if (type == STRING)
            {
                int size = popByte(s);
                char *str = popString(s, size);
                if (!str)
                {
                    Serial.println(F("Failed to pop string for overwrite!"));
                    return;
                }
                if (size != existing.size)
                {
                    // Allocate new space in memory[]
                    if (heapPointer + size + 1 >= MEMORY_SIZE)
                    {
                        Serial.println(F("Not enough heap memory for reallocation!"));
                        return;
                    }

                    existing.address = &memory[heapPointer];
                    existing.size = size;
                    heapPointer += size + 1; // include space for null terminator
                }

                strncpy((char *)existing.address, str, size);
                ((char *)existing.address)[size] = '\0';
            }
            return;
        }
    }
    Variable var;
    var.name = name; // Assuming single character names for simplicity
    var.pid = pid;
    var.type = 0;          // Default type, can be set later
    var.size = 0;          // Default size, can be set later
    var.address = nullptr; // Address will be set when the variable is created

    int type = popByte(s); // Get the type from the stack
    var.type = type;

    // check if the type is a number or a string
    if (type == FLOAT || type == INT || type == CHAR)
    {
        switch (type)
        {
        case INT:
            var.size = 2; // Size of int
            break;

        case FLOAT:
            var.size = 4; // Size of float
            break;
        case CHAR:
            var.size = 1; // Size of char
            break;

        default:
            break;
        }


        float value = popVal(s, type);

        var.address = &memory[heapPointer];
        heapPointer += var.size;
        // Store the value in the allocated memory
        if (type == FLOAT)
        {
            *(float *)var.address = value; // Store float value
        }
        else if (type == INT)
        {
            *(int *)var.address = (int)value; // Store int value
        }
        else if (type == CHAR)
        {
            *(char *)var.address = (char)value; // Store char value
        }
    }
    // String type
    else
    {
        var.size = popByte(s);              // Pop the length of the string
        char *str = popString(s, var.size); // Pop the string from the stack
        if (!str)
        {
            Serial.println(F("Failed to pop string from stack!"));
            return; // Handle string pop failure
        }
        var.address = &memory[heapPointer];
        heapPointer += var.size + 1;
        if (var.address == nullptr)
        {
            Serial.println(F("Memory allocation failed!"));
            return; // Handle memory allocation failure
        }

        strncpy((char *)var.address, str, var.size); // Copy string to allocated memory
        ((char *)var.address)[var.size] = '\0';  // Ensure null-terminated
    }
    memoryTable[noOfVars++] = var;
}

void printStack(Stack *s)
{
    Serial.print(F("Stack contents: "));
    for (int i = 0; i < s->sp; i++)
    {
        Serial.print(s->data[i], DEC);
        Serial.print(F(" "));
    }
    Serial.println();
}

// function to get variable from heap and push it to the stack
void getVariable(Stack *s, byte name, int pid)
{
    if (noOfVars == 0)
    {
        Serial.println(F("No variables in memory!"));
        return; // Handle no variables case
    }

    for (int i = 0; i < noOfVars; i++)
    {
        if (memoryTable[i].name == name && memoryTable[i].pid == pid)
        {
            // Variable found, push its value to the stack
            switch (memoryTable[i].type)
            {
            case CHAR:
                pushChar(s, *(char *)memoryTable[i].address);
                break;
            case INT:
                pushInt(s, *(int *)memoryTable[i].address);
                break;
            case FLOAT:
                pushFloat(s, *(float *)memoryTable[i].address);
                break;
            case STRING:
                pushString(s, (char *)memoryTable[i].address);
                break;
            default:
                Serial.println(F("Unknown variable type!"));
                return; // Handle unknown type
            }
            return; // Exit after finding the variable
        }
    }
}

void deleteVariables(int pid)
{
    for (int i = 0; i < noOfVars; i++)
    {
        if (memoryTable[i].pid == pid)
        {
            // Free the allocated memory for the variable
            heapPointer -= memoryTable[i].size; // Adjust heap pointer
            // Shift remaining variables down
            for (int j = i; j < noOfVars - 1; j++)
            {
                memoryTable[j] = memoryTable[j + 1];
            }
            noOfVars--; // Decrease the count of variables
            i--;        // Adjust index after deletion
        }
    }
}

void listVariables()
{

    if (noOfVars == 0)
    {
        Serial.println(F("No variables in memory."));
        return; // Handle no variables case
    }
    Serial.println(F("Variables in memory:"));
    for (int i = 0; i < noOfVars; i++)
    {
        Serial.print(F("Name: "));
        Serial.print((char)memoryTable[i].name);
        Serial.print(F(", PID: "));
        Serial.print(memoryTable[i].pid);
        Serial.print(F(", Type: "));
        switch (memoryTable[i].type)
        {
        case CHAR:
            Serial.print(F("CHAR"));
            break;
        case INT:
            Serial.print(F("INT"));
            break;
        case FLOAT:
            Serial.print(F("FLOAT"));
            break;
        case STRING:
            Serial.print(F("STRING"));
            break;
        default:
            Serial.print(F("UNKNOWN"));
            break;
        }
        Serial.print(F(", Size: "));
        Serial.print(memoryTable[i].size);
        Serial.print(F(", Address: "));
        Serial.println((unsigned long)memoryTable[i].address, HEX);
    }
}
