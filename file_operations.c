#include "file_operations.h"

// Write
void WriteToFile(const char *fileName, HeaderInfo header)
{
  if (fileName == NULL || header.payload == NULL)
    return;

  int nTotalFileSize;
  char *pDataToWrite = prepareDataForWrite(header, &nTotalFileSize);

  if (pDataToWrite == NULL)
    return;

  WriteDataFile(fileName, pDataToWrite, nTotalFileSize);
  for (int i = 1; i < MAXFILE_COUNT; ++i)
  {
    char *backupFilename = GetBackupFilename(fileName, i);
    WriteDataFile(backupFilename, pDataToWrite, nTotalFileSize);
  }
  free(pDataToWrite);
}

char *prepareDataForWrite(HeaderInfo header, int *totalFileSize)
{
  *totalFileSize = header.payloadSize + HEADER_SIZE + CRC_SIZE;
  printf("Total file size: %d\n", *totalFileSize);

  char *pDataToWrite = (char *)malloc(*totalFileSize);
  if (pDataToWrite == NULL)
  {
    printf("Memory allocation failed.\n");
    return NULL;
  }
  printf("Memory allocation successful.\n");

  memset(pDataToWrite, 0, *totalFileSize);

  pDataToWrite[0] = header.fileID;
  pDataToWrite[1] = header.readCount;

  // Copy payload data starting from index 2
  memcpy(&pDataToWrite[HEADER_SIZE], header.payload, header.payloadSize);

  // Calculate CRC on the prepared data excluding the CRC itself
  header.crc = CalculateCRC(pDataToWrite, *totalFileSize - CRC_SIZE);
  printf("CRC calculated: %d\n", header.crc);

  // Insert CRC at the end of the data
  pDataToWrite[*totalFileSize - CRC_SIZE] = header.crc;

  // Encrypt the prepared data
  encryptDecrypt(pDataToWrite, *totalFileSize);

  printf("Data prepared for writing.\n");
  return pDataToWrite;
}

void ProcessChunk(const char *chunkData, size_t chunkSize, void *context)
{
  Accumulator *acc = (Accumulator *)context;
  // Ensure there is enough space in the accumulator
  if (acc->size + chunkSize > acc->capacity)
  {
    size_t newCapacity = acc->capacity == 0 ? CHUNK_SIZE : acc->capacity * 2;
    while (newCapacity < acc->size + chunkSize)
      newCapacity *= 2;
    acc->data = (char *)realloc(acc->data, newCapacity);
    acc->capacity = newCapacity;
  }
  memcpy(acc->data + acc->size, chunkData, chunkSize);
  acc->size += chunkSize;
}

void WriteDataFile(const char *fileName, const char *pData,
                   const unsigned int nDataSize)
{
  FILE *file = fopen(fileName, "wb");
  if (file == NULL)
  {
    printf("Failed to open file: %s for writing.\n", fileName);
    return;
  }

  unsigned int remainingDataSize = nDataSize;
  const char *currentPtr = pData;

  while (remainingDataSize > 0)
  {
    unsigned int chunkSize = remainingDataSize > WRITE_CHUNK_SIZE
                                 ? WRITE_CHUNK_SIZE
                                 : remainingDataSize;
    size_t written = fwrite(currentPtr, 1, chunkSize, file);
    if (written != chunkSize)
    {
      printf("Failed to write data to file: %s\n", fileName);
      break; // Exit the loop on write error
    }
    remainingDataSize -= chunkSize;
    currentPtr += chunkSize;
  }

  fclose(file);
  printf("Data successfully written to file: %s\n", fileName);
}

// READ
HeaderInfo *ReadFromFile(const char *filename)
{
  unsigned int arrFileDataSize[MAXFILE_COUNT] = {0};
  char *arrFileData[MAXFILE_COUNT] = {NULL};

  printf("Reading data from file: %s\n", filename);

  readFileData(filename, arrFileData, arrFileDataSize);
  unsigned char validIndex = findValidIndex(arrFileData, arrFileDataSize);

  if (validIndex != 0xFF)
  {
    printf("Valid data found in the file.\n");

    HeaderInfo *headerInfo =
        processValidData(arrFileData[validIndex], arrFileDataSize[validIndex]);
    printf("headerInfo: %p\n", headerInfo);

    if (headerInfo)
    {
      printf("Header information processed successfully.\n");
      WriteToFile(filename, *headerInfo);
      printf("Header information written to file.\n");
    }
    else
    {
      printf("Failed to process header information.\n");
    }

    cleanupDataArrays(arrFileData);
    return headerInfo;
  }

  printf("No valid data found in the file.\n");
  cleanupDataArrays(arrFileData);
  return NULL;
}

void readFileData(const char *filename, char **arrFileData,
                  unsigned int *arrFileDataSize)
{
  // This loop remains the same, but ReadDataFromFile is adjusted to be more
  // efficient
  for (int i = 0; i < MAXFILE_COUNT; ++i)
  {
    const char *currentFilename =
        (i == 0) ? filename : GetBackupFilename(filename, i);
    arrFileData[i] = ReadDataFromFile(currentFilename, &arrFileDataSize[i]);
  }
}

unsigned char findValidIndex(char *arrFileData[],
                             unsigned int arrFileDataSize[])
{
  if (MAXFILE_COUNT <= 0)
  {
    return 0xFF; // Early exit if the array is empty or invalid size
  }

  for (unsigned int i = 0; i < MAXFILE_COUNT - 1; ++i)
  {
    // Skip if the current file data pointer is NULL
    if (!arrFileData[i])
      continue;

    for (unsigned int j = i + 1; j < MAXFILE_COUNT; ++j)
    {
      // Skip if the comparison file data pointer is NULL
      if (!arrFileData[j])
        continue;

      printf("Comparing data blocks %d and %d\n", i, j);
      printf("arrFileData[i]: %p\n", arrFileData[i]);

      DataBlock dataBlock1 = {arrFileData[i], arrFileDataSize[i]};
      DataBlock dataBlock2 = {arrFileData[j], arrFileDataSize[j]};

      bool isCompared = PerformDataComparison(&dataBlock1, &dataBlock2);
      if (isCompared)
      {
        printf("Data blocks %d and %d are the same.\n", i, j);
        return (unsigned char)i;
      }
    }
  }

  printf("No identical data blocks found.\n");
  return 0xFF; // Indicate no valid index found
}

HeaderInfo *processValidData(char *validData, unsigned int dataSize)
{
  // Allocate memory for HeaderInfo structure
  HeaderInfo *headerInfo = (HeaderInfo *)malloc(sizeof(HeaderInfo));
  if (!headerInfo)
  { // Check allocation success
    return NULL;
  }

  // Check if dataSize is large enough to hold the file ID, read count, and CRC
  if (dataSize < 3)
  {
    free(headerInfo);
    return NULL;
  }

  // Decrypt the data
  encryptDecrypt(validData, dataSize);

  // Calculate CRC on the decrypted data, excluding the last byte assumed to be
  // the CRC itself
  unsigned char nCRC = CalculateCRC(validData, dataSize - 1);

  // Early return if validation fails
  if ((unsigned char)validData[0] != FILEID ||
      (unsigned char)validData[dataSize - 1] != nCRC)
  {
    free(headerInfo);
    return NULL;
  }

  // Set header information based on validData content
  headerInfo->fileID = validData[0];
  headerInfo->readCount = validData[1] + 1;
  headerInfo->crc = nCRC;
  // Exclude fileID, readCount, CRC from payload size
  headerInfo->payloadSize = dataSize - 3;

  // Allocate memory for the payload
  headerInfo->payload = (unsigned char *)malloc(headerInfo->payloadSize);
  // Check payload allocation success
  if (!headerInfo->payload)
  {
    free(headerInfo);
    return NULL;
  }

  // Copy the payload data into the headerInfo structure
  memcpy(headerInfo->payload, validData + 2, headerInfo->payloadSize);

  return headerInfo;
}

void cleanupDataArrays(char *arrFileData[])
{
  for (int i = 0; i < MAXFILE_COUNT; ++i)
  {
    if (arrFileData[i])
      free(arrFileData[i]);
  }
}

char *ReadDataFromFile(const char *fileName, unsigned int *pFileSize)
{
  FILE *pFile = fopen(fileName, "rb");
  if (!pFile)
  {
    return NULL;
  }

  fseek(pFile, 0, SEEK_END);
  *pFileSize = ftell(pFile);
  fseek(pFile, 0, SEEK_SET);

  char *pData = (char *)malloc(*pFileSize);
  if (!pData)
  {
    fclose(pFile);
    return NULL;
  }

  if (fread(pData, 1, *pFileSize, pFile) != *pFileSize)
  {
    free(pData);
    fclose(pFile);
    return NULL;
  }

  fclose(pFile);
  return pData;
}

unsigned char CalculateCRC(const char *pData, const int nDataSize)
{
  // Early return if the data pointer is NULL to avoid dereferencing NULL
  if (pData == NULL)
  {
    return 0; // Return 0 or an appropriate error code
  }

  int nIndex;
  int nCRC = 0;

  // Perform CRC calculation only if pData is not NULL
  for (nIndex = 0; nIndex < nDataSize; nIndex++)
  {
    nCRC += pData[nIndex];
  }

  return (unsigned char)~nCRC; // Return the calculated CRC
}

char *GetBackupFilename(const char *pFilename, const int nBackupID)
{
  static char szFilename[100] = {0};
  if (pFilename == NULL)
    return 0;
  memset(szFilename, 0, 100);
  sprintf(szFilename, "%s_%d", pFilename, nBackupID);
  return szFilename;
}

void encryptDecrypt(char *pData, const int nDataSize)
{
  if (pData == NULL)
    return;
  for (int nIndex = 0; nIndex < nDataSize; nIndex++)
  {
    pData[nIndex] = ~pData[nIndex];
  }
}

bool PerformDataComparison(const DataBlock *dataBlock1,
                           const DataBlock *dataBlock2)
{
  // Check if either DataBlock has size less than 4 or their sizes are not equal
  if (dataBlock1->size < 4 || dataBlock2->size < 4 ||
      dataBlock1->size != dataBlock2->size)
    return false;

  // Compare the data contents of the DataBlocks
  return memcmp(dataBlock1->data, dataBlock2->data, dataBlock1->size) == 0;
}

// Function to collect user input
void getUserInput(char *inputBuffer, size_t bufferSize, const char *prompt)
{
  printf("%s", prompt);
  getchar(); // Clears any leftover newline character in the input buffer
  fgets(inputBuffer, bufferSize, stdin);
  // Remove newline character, if present
  size_t len = strlen(inputBuffer);
  if (len > 0 && inputBuffer[len - 1] == '\n')
  {
    inputBuffer[len - 1] = '\0';
  }
}

// Function to initialize header
HeaderInfo initializeHeader(const unsigned char *payload, size_t payloadSize)
{
  HeaderInfo header;
  header.fileID = FILEID;
  header.readCount = 0;
  header.crc = 0;

  // Allocate memory for payload and copy the data
  header.payload = (unsigned char *)malloc(payloadSize);
  if (header.payload == NULL)
  {
    // Handle memory allocation failure
    fprintf(stderr, "Error: Memory allocation failed for payload.\n");
    exit(EXIT_FAILURE);
  }
  memcpy(header.payload, payload, payloadSize);

  header.payloadSize = payloadSize;
  return header;
}

void printHeaderInfo(HeaderInfo *header)
{
  if (header == NULL)
  {
    printf("Header is NULL\n");
    return;
  }

  printf("File ID = %u\n", header->fileID);
  printf("Read Count = %u\n", header->readCount);
  printf("CRC = %u\n", header->crc);
  printf("Payload Size = %zu\n", header->payloadSize);
  printf("Payload Data:\n");
  for (size_t i = 0; i < header->payloadSize; ++i)
  {
    printf("%02X ", header->payload[i]);
  }
  printf(" = %s\n", header->payload);
  printf("\n");
}

void freeHeaderInfo(HeaderInfo *header)
{
  if (header != NULL)
  {
    free(header->payload);
    free(header);
  }
}

void createDirectory(const char *path)
{
  char command[256];
  sprintf(command, "mkdir -p %s", path);
  system(command);
}

char *constructFilePath(const char *filename)
{
  char *filepath = malloc(strlen(FILE_FOLDER) + strlen(filename) + 1);
  if (filepath == NULL)
  {
    perror("Memory allocation failed");
    exit(EXIT_FAILURE);
  }
  strcpy(filepath, FILE_FOLDER);
  strcat(filepath, filename);
  return filepath;
}
