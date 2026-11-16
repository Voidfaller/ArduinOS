#include <fs.h>
#include <Arduino.h>
#include <EEPROM.h>
#include <str.h>
#include <global_var.h>

void fsInit()
{
    if (EEPROM.read(NO_OF_FILES_ADDR) == 255) // check if the eeprom is initialised
    {
        EEPROM.write(NO_OF_FILES_ADDR, 0); // Initialize the number of files to 0
    }
}

void writeFATEntry(int index, const File *entry)
{
    int addr = FAT_START + index * FAT_ENTRY_SIZE;
    EEPROM.put(addr, *entry);
}

void readFATEntry(int index, File *entry)
{
    int addr = FAT_START + index * FAT_ENTRY_SIZE;
    EEPROM.get(addr, *entry);
}

void deleteFile(int argc, char *argv[])
{
    if (argc < 2)
    {
        Serial.println(F("Invalid arguments. Usage: delete <filename>"));
        return;
    }

    const char *filename = argv[1];
    int no_of_files = EEPROM.read(NO_OF_FILES_ADDR);

    for (int i = 0; i < no_of_files; i++)
    {
        File entry;
        readFATEntry(i, &entry);

        if (strcompare(entry.name, filename))
        {
            int deletedSize = entry.length;

            // Shift FAT entries left
            for (int j = i; j < no_of_files - 1; j++)
            {
                File nextEntry;
                readFATEntry(j + 1, &nextEntry);
                writeFATEntry(j, &nextEntry);
            }

            EEPROM.write(NO_OF_FILES_ADDR, no_of_files - 1);

         // Adjust remaining FAT entries to reflect new data locations
            for (int j = i; j < no_of_files - 1; j++)
            {
                File shiftedEntry;
                readFATEntry(j, &shiftedEntry);
                shiftedEntry.location -= deletedSize;
                writeFATEntry(j, &shiftedEntry);
            }

            // Move actual data blocks in EEPROM
            for (int j = i; j < no_of_files - 1; j++)
            {
                File shiftedEntry;
                readFATEntry(j, &shiftedEntry);

                for (unsigned int byteOffset = 0; byteOffset < shiftedEntry.length; byteOffset++)
                {
                    byte data = EEPROM.read(shiftedEntry.location + deletedSize + byteOffset);
                    EEPROM.write(shiftedEntry.location + byteOffset, data);
                }
            }

            Serial.println(F("File deleted and memory compacted."));
            return;
        }
    }

    Serial.println(F("File not found."));
}

void listFiles(int argc, char *argv[])
{
    int no_of_files = EEPROM.read(NO_OF_FILES_ADDR);
    Serial.print(F("Number of files: "));
    Serial.println(no_of_files);
    for (int i = 0; i < no_of_files; i++)
    {
        File entry;
        readFATEntry(i, &entry);
        Serial.print(F("File "));
        Serial.print(i);
        Serial.print(F(": "));
        Serial.print(entry.name);
        Serial.print(F(", Location: "));
        Serial.print(entry.location);
        Serial.print(F(", Length: "));
        Serial.println(entry.length);
    }
}

void getFreeSpace(int argc, char *argv[])
{
    Serial.println(F("Flash:"));
    Serial.print(F("Total space: "));
    Serial.print(EEPROM.length());
    Serial.println(F(" bytes"));
    Serial.print(F("Used space: "));
    int no_of_files = EEPROM.read(NO_OF_FILES_ADDR);
    Serial.print(no_of_files * FAT_ENTRY_SIZE);
    Serial.println(F(" bytes"));
    Serial.print(F("Free space: "));
    Serial.print(EEPROM.length() - (no_of_files * FAT_ENTRY_SIZE));
    Serial.println(F(" bytes"));
    Serial.print(F("Max files: "));
    Serial.println(MAX_FILES);
    Serial.print(F("Max file size: "));
    Serial.println(EEPROM.length() - DATA_START);
    Serial.println(F(" bytes"));
}

void createFile(int argc, char *argv[])
{
    if (argc < 3)
    {
        Serial.println(F("Invalid arguments. Usage: store <filename> <length> <data>"));
        return;
    }

    const char *filename = argv[1];
    unsigned short length = atoi(argv[2]);
    const char *data = argv[3];

    if (strlength(filename) > 12)
    {
        Serial.println(F("Filename too long. Max length is 12 characters."));
        return;
    }

    int no_of_files = EEPROM.read(NO_OF_FILES_ADDR);
    if (no_of_files >= MAX_FILES)
    {
        Serial.println(F("No space left to store files."));
        return;
    }

    File entry;
    strcopy(entry.name, filename);
    entry.location = DATA_START;

    // Calculate the location of the new file
    for (int i = 0; i < no_of_files; i++)
    {
        File existingEntry;
        readFATEntry(i, &existingEntry);
        entry.location += existingEntry.length;
    }
    entry.length = length;

    // Check if the file already exists
    for (int i = 0; i < no_of_files; i++)
    {
        File existingEntry;
        readFATEntry(i, &existingEntry);
        if (strcompare(existingEntry.name, filename))
        {
            Serial.println(F("File already exists."));
            return;
        }
    }

    // Check if the file size exceeds the available space
    if (entry.location + length > EEPROM.length() - DATA_START)
    {
        Serial.println(F("Not enough space to store the file."));
        return;
    }

    // Write the new file entry to the FAT
    writeFATEntry(EEPROM.read(NO_OF_FILES_ADDR), &entry);
    EEPROM.write(NO_OF_FILES_ADDR, EEPROM.read(NO_OF_FILES_ADDR) + 1); // Increment the number of files

    // Write the file content to the EEPROM
    int dataAddr = entry.location;
    for (unsigned int i = 0; i < length; i++)
    {
        EEPROM.write(dataAddr + i, data[i]);
    }
}

void readFile(int argc, char *argv[])
{
    if (argc < 2)
    {
        Serial.println(F("Invalid arguments. Usage: getfile <filename>"));
        return;
    }

    const char *filename = argv[1];
    int no_of_files = EEPROM.read(NO_OF_FILES_ADDR);
    for (int i = 0; i < no_of_files; i++)
    {
        File entry;
        readFATEntry(i, &entry);
        if (strcompare(entry.name, filename))
        {
            Serial.print(F("File found: "));
            Serial.print(entry.name);
            Serial.print(F(", Location: "));
            Serial.print(entry.location);
            Serial.print(F(", Length: "));
            Serial.println(entry.length);

            // Read the file content from the EEPROM
            char *content = new char[entry.length + 1];
            for (unsigned int j = 0; j < entry.length; j++)
            {
                content[j] = EEPROM.read(entry.location + j);
            }
            content[entry.length] = '\0'; // Null-terminate the string
            Serial.print(F("Content: "));
            Serial.println(content);
            delete[] content;
            return;
        }
    }
    Serial.println(F("File not found."));
}

void readByteFile(int argc, char *argv[])
{
    if (argc < 2)
    {
        Serial.println(F("Invalid arguments. Usage: getfile <filename>"));
        return;
    }

    const char *filename = argv[1];
    int no_of_files = EEPROM.read(NO_OF_FILES_ADDR);
    for (int i = 0; i < no_of_files; i++)
    {
        File entry;
        readFATEntry(i, &entry);
        if (strcompare(entry.name, filename))
        {
            Serial.print(F("File found: "));
            Serial.print(entry.name);
            Serial.print(F(", Location: "));
            Serial.print(entry.location);
            Serial.print(F(", Length: "));
            Serial.println(entry.length);

            Serial.println(F("Content (hex):"));
            for (unsigned int j = 0; j < entry.length; j++)
            {
                byte val = EEPROM.read(entry.location + j);
                if (val < 16)
                    Serial.print('0'); // pad single-digit hex
                Serial.print(val, HEX);
                Serial.print(' ');
            }
            Serial.println();
            return;
        }
    }
    Serial.println(F("File not found."));
}

File *getFile(const char *filename)
{
    int no_of_files = EEPROM.read(NO_OF_FILES_ADDR);
    File *entry = new File;
    for (int i = 0; i < no_of_files; i++)
    {
        readFATEntry(i, entry);
        if (strcompare(entry->name, filename))
        {
            return entry; // Return the found file entry
        }
        delete entry; // Clean up if not found
    }
    return nullptr; // Return nullptr if file not found
}

void storeFile(int argc, char *argv[])
{
    if (argc < 3)
    {
        Serial.println(F("Invalid arguments. Usage: store <filename> <length>"));
        return;
    }

    const char *filename = argv[1];
    int length = atoi(argv[2]);

    if (length <= 0)
    {
        Serial.println(F("Invalid length."));
        return;
    }

    if (strlength(filename) > 12)
    {
        Serial.println(F("Filename too long. Max 12 chars."));
        return;
    }

    int no_of_files = EEPROM.read(NO_OF_FILES_ADDR);
    if (no_of_files >= MAX_FILES)
    {
        Serial.println(F("No space left to store files."));
        return;
    }

    // Calculate starting location for the new file
    unsigned int startLocation = DATA_START;
    for (int i = 0; i < no_of_files; i++)
    {
        File existingEntry;
        readFATEntry(i, &existingEntry);
        startLocation += existingEntry.length;
    }

    // Check if enough EEPROM space is left
    if (startLocation + length > EEPROM.length())
    {
        Serial.println(F("Not enough EEPROM space."));
        return;
    }

    // Check if file already exists
    for (int i = 0; i < no_of_files; i++)
    {
        File existingEntry;
        readFATEntry(i, &existingEntry);
        if (strcompare(existingEntry.name, filename))
        {
            Serial.println(F("File already exists."));
            return;
        }
    }

    // Prepare file entry for when data is done uploading
    currentFile.length = length;
    currentFile.location = startLocation;
    strcopy(currentFile.name, filename);

    // // Setup for binary data mode
    binaryMode = true;
    binaryBytesLeft = length;

    Serial.println(F("Send binary data now"));
}

void factoryReset(int argc, char *argv[])
{
    Serial.println(F("Performing factory reset..."));
    EEPROM.write(NO_OF_FILES_ADDR, 0); // Reset the number of files to 0

    // Clear all FAT entriess
    for (int i = 0; i < MAX_FILES; i++)
    {
        File emptyEntry = {"", 0, 0};
        writeFATEntry(i, &emptyEntry);
    }

    // Clear all data blocks
    for (int i = DATA_START; i < EEPROM.length(); i++)
    {
        EEPROM.write(i, 0);
    }

    Serial.println(F("Factory reset complete."));
}