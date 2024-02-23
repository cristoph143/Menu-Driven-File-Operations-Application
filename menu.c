#include "menu.h"
#include "file_operations.h"
#include <stdio.h>
#include <stdlib.h>

#define _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE
void printMenu(const MenuItem *menuItems, int numItems)
{
  printf("------------------------------------------------------------\n");
  printf("Menu\n");
  printf("------------------------------------------------------------\n");

  for (int i = 0; i < numItems; i++)
  {
    printf("%d. %s\n", i + 1, menuItems[i].description);
  }

  printf("0. Exit\n");
  printf("------------------------------------------------------------\n");
  printf("Enter Choice: ");
}

// Implementation of the menu function
void menu(int choice, const MenuItem *menuItems, int numMenuItems)
{
  // Check if the choice is within the range of menu items
  if (choice >= 0 && choice < numMenuItems)
  {
    // Execute the function associated with the chosen menu item
    menuItems[choice].function();
    printf("Press any key to continue...");
    getchar(); // Wait for user input
    // Clear the newline character from the input buffer
    while (getchar() != '\n')
      ;
  }
  else
  {
    // Print an error message for an invalid choice
    printf("Invalid choice. Please enter a valid option.\n");
  }
}

void exitFunction() { printf("Exiting...\n"); }

/**
 * @brief Gets a valid choice from the user.
 *
 * This function prompts the user for input until a valid choice is entered.
 *
 * @param numOptions The number of options available.
 * @return The valid choice entered by the user.
 */
int getValidChoice(int numOptions)
{
  int choice;
  // printf("Enter Choice: ");
  while (scanf("%d", &choice) != 1 || choice < 0 || choice > numOptions)
  {
    // Clear input buffer
    while (getchar() != '\n')
      ;
    printf("Invalid choice. Please enter a valid option: \n");
  }
  return choice;
}

void runMenu(const MenuItem *menuItems, int numMenuItems)
{
  int choice = -1;

  while (choice != 0)
  {
    // Clear the screen
    system("cls || clear");
    printMenu(menuItems, numMenuItems);
    choice = getValidChoice(numMenuItems);
    clearInputBuffer();
    if (choice != 0)
    {
      menu(choice - 1, menuItems, numMenuItems);
    }
  }
}

void goodbye() { printf("Goodbye\n"); }

// Function to clear the input buffer
void clearInputBuffer()
{
  int c;
  while ((c = getchar()) != '\n' && c != EOF)
  {
  }
}

// Step 1: Create and Save File
// Refactored function to create and save file
void createAndSaveFile()
{
  char *userInput;
  char fileName[100] = "test.txt"; // Keeping file name handling simple for now

  printf("Enter text to be saved in the file. End with EOF (Ctrl+D/Ctrl+Z): ");
  userInput = readUntilEOF();

  if (userInput)
  {
    HeaderInfo header =
        initializeHeader((unsigned char *)userInput, strlen(userInput));
    printHeaderInfo(&header);
    WriteToFile(fileName, header);
    free(userInput); // Free dynamically allocated memory
  }
  else
  {
    printf("No input read.\n");
  }
}

char *readUntilEOF()
{
  char tempBuffer[1024];    // Temporary buffer for each line of input
  char *inputBuffer = NULL; // Dynamic buffer for the total input
  size_t totalSize = 0;     // Total size of the inputBuffer

  printf("Enter text (EOF to stop): \n");
  while (fgets(tempBuffer, sizeof(tempBuffer), stdin))
  {
    size_t lineSize = strlen(tempBuffer);
    char *newBuffer = realloc(inputBuffer, totalSize + lineSize + 1);
    if (!newBuffer)
    {
      perror("Failed to allocate memory");
      free(inputBuffer);
      exit(EXIT_FAILURE);
    }
    inputBuffer = newBuffer;
    memcpy(inputBuffer + totalSize, tempBuffer, lineSize);
    totalSize += lineSize;
    inputBuffer[totalSize] = '\0'; // Ensure null-terminated string
  }

  return inputBuffer; // Caller is responsible for freeing this
}

// Step 2: Read File
void readFiles()
{
  char fileName[100] = "test.txt";
  // char fileName[100];
  // getUserInput(fileName, sizeof(fileName), "Enter the filename: ");

  HeaderInfo *header = ReadFromFile(fileName); // Use the user-provided filename
  if (header != NULL)
  {
    printHeaderInfo(header);
    freeHeaderInfo(header); // Use the new function to free resources
  }
  else
  {
    printf("File Error\n");
  }
}

void inputString(const char *prompt, char *str, size_t size)
{
  printf("%s", prompt);
  scanf("%s", str);
}
