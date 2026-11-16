//filesystem.c
#ifndef FS_H
#define FS_H

#define MAX_FILES 10
#define FAT_ENTRY_SIZE 16
#define FAT_START 0
#define NO_OF_FILES_ADDR 160
#define DATA_START NO_OF_FILES_ADDR + 1
typedef struct
{
    char name[12];
    unsigned short location;
    unsigned short length;
} File;




void fsInit();
void readFile(int argc, char *argv[]);
void listFiles(int argc, char *argv[]);
void createFile(int argc, char *argv[]);
void deleteFile(int argc, char *argv[]);
void writeFile(const char *filename, const char *content);
void getFreeSpace(int argc, char *argv[]);
File *getFile(const char *filename);
void writeFATEntry(int index, const File* entry);
void readFATEntry(int index, File* entry);
void storeFile(int argc, char *argv[]);
void factoryReset(int argc, char *argv[]);
void readByteFile(int argc, char *argv[]);







#endif // FILESYSTEM_H