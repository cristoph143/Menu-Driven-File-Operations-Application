#ifndef UPDATE_H
#define UPDATE_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
// Define DEBUG mode
#define DEBUG 1

// Define debug print macro
#if DEBUG
#define DEBUG_PRINT(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
#else
#define DEBUG_PRINT(fmt, ...)
#endif

#define BUFFER_SIZE 1024
#define FILEID 0xFA
#define HEADER_SIZE 2
#define CRC_SIZE 1
#define MAXFILE_COUNT 3
#define FILE_FOLDER "sample/"

typedef struct {
  unsigned char fileID;
  unsigned char readCount;
  unsigned crc;
  unsigned char *payload;
  size_t payloadSize;
} HeaderInfo;

typedef struct {
  const char *data;
  int size;
} DataBlock;

typedef struct {
  char *data;      // Dynamic buffer to accumulate file data
  size_t size;     // Current size of the accumulated data
  size_t capacity; // Current capacity of the buffer
} Accumulator;
#define CHUNK_SIZE 1024 * 1024       // 1 MB for chunk processing
#define WRITE_CHUNK_SIZE 1024 * 1024 // 1 MB
#define MINIMUM_VALID_SIZE 10
// Main Functions
//  Function to write data to a file, using HeaderInfo structure
void WriteToFile(const char *fileName, HeaderInfo header);
// Function to read data from a file, returns a pointer to HeaderInfo structure
HeaderInfo *ReadFromFile(const char *filename);

// Helper Functions
// Initialization and Cleanup Functions
HeaderInfo *InitHeaderInfo(const HeaderInfo *pHeaderInfo);
void freeHeaderInfo(HeaderInfo *header);
void ProcessChunk(const char *chunkData, size_t chunkSize, void *context);
// Data Processing Functions
HeaderInfo *processValidData(char *validData, unsigned int dataSize);
char *prepareDataForWrite(HeaderInfo header, int *totalFileSize);

// Input/Output Helpers
void getUserInput(char *inputBuffer, size_t bufferSize, const char *prompt);
void printHeaderInfo(HeaderInfo *header);

// File Reading and Writing Helpers
void readFileData(const char *filename, char *arrFileData[],
                  unsigned int arrFileDataSize[]);
unsigned char findValidIndex(char *arrFileData[],
                             unsigned int arrFileDataSize[]);

void createDirectory(const char *path);
char *constructFilePath(const char *filename);
int FileExists(const char *filename);

// Utility Functions
unsigned char CalculateCRC(const char *pData, const int nDataSize);
void encryptDecrypt(char *pData, const int nDataSize);
bool PerformDataComparison(const DataBlock *dataBlock1,
                           const DataBlock *dataBlock2);
char *GetBackupFilename(const char *pFilename, const int nBackupID);

// Auxiliary function prototypes
void WriteDataFile(const char *fileName, const char *pData,
                   const unsigned int nDataSize);
char *ReadDataFromFile(const char *fileName, unsigned int *pFileSize);

HeaderInfo initializeHeader(const unsigned char *payload, size_t payloadSize);
char *prepareDataForWrite(HeaderInfo header, int *totalFileSize);
void readAndStoreFileData(const char *filename, char *data[MAXFILE_COUNT],
                          size_t dataSize[MAXFILE_COUNT]);
int findMatchingDataIndex(char *data[MAXFILE_COUNT],
                          size_t dataSize[MAXFILE_COUNT]);
void ensureBackupsExistAndAreValid(const char *filename,
                                   HeaderInfo *headerInfo);
bool isValidFile(const char *data, unsigned int dataSize);
void cleanupDataArrays(char *arrFileData[]);
#endif // UPDATE_H
