<p align="center">
  <a href="" rel="noopener">
    <img width="200px" height="200px" src="https://i.imgur.com/6wj0hh6.jpg" alt="Project logo" />
  </a>
</p>
<MDXTag name="p" components={components}>
  <h3 align="center">
    { `Menu-Driven File Operations Application` }
  </h3>
</MDXTag>

<div align="center">

[![Status](https://img.shields.io/badge/status-active-success.svg)]()
[![GitHub Issues](https://img.shields.io/github/issues/your-repo-name/your-project-name.svg)](https://github.com/your-repo-name/your-project-name/issues)
[![GitHub Pull Requests](https://img.shields.io/github/issues-pr/your-repo-name/your-project-name.svg)](https://github.com/your-repo-name/your-project-name/pulls)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](/LICENSE)

</div>

---

<p align="center">

# Application Overview

The application facilitates various file operations through a user-friendly menu interface. Users can create and save files, read files, and exit the application smoothly.

</p>

## 📝 Table of Contents

- [Application Overview](#application-overview)
  - [📝 Table of Contents](#-table-of-contents)
  - [🧐 About ](#-about-)
  - [🏁 Getting Started ](#-getting-started-)
  - [🚀 Prerequisites ](#-prerequisites-)
  - [🚀 Compilation ](#-compilation-)
  - [🚀 Running the Application ](#-running-the-application-)
  - [🎈 Usage ](#-usage-)
  - [Menu](#menu)
  - [🎈 Flow Details ](#-flow-details-)
  - [🎈 Key Operation Details ](#-key-operation-details-)
  - [🎈 System Process Details ](#-system-process-details-)
    - [A. Create and Save File Process](#a-create-and-save-file-process)
    - [B. Read File Process](#b-read-file-process)
    - [Additional Key Functions](#additional-key-functions)
  - [✍️ Authors ](#️-authors-)
  - [🎉 Acknowledgements ](#-acknowledgements-)

## 🧐 About <a name = "about"></a>

Write about 1-2 paragraphs describing the purpose of your project.

## 🏁 Getting Started <a name = "getting_started"></a>

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. See [deployment](#deployment) for notes on how to deploy the project on a live system.

## 🚀 Prerequisites <a name = "Prerequisites"></a>

- A C compiler, such as GCC, for compiling the application.
- Basic understanding of terminal or command prompt usage.

## 🚀 Compilation <a name = "compile"></a>

- To compile the application, navigate to the project directory and run the following command:

```
gcc -o main  main.c menu.c file_operations.c
```

This command compiles the menu.c and file_operations.c files and generates an executable named menu_app.

## 🚀 Running the Application <a name = "run"></a>

After compilation, you can run the application by executing the following command in your terminal or command prompt:

```
./main
```

## 🎈 Usage <a name="usage"></a>

---

## Menu

1. Create and save file
2. Read file
3. Exit

---

Enter Choice:

- To create and save a file: Enter 1, then follow the prompts to input text. End the input with EOF (Ctrl+D on Linux/macOS, Ctrl+Z on Windows) and the file will be saved.
- To read a file: Enter 2, and the application will read and display the contents of the previously saved file.
  To exit the application: Enter 0.

Sample Dummy Data: https://www.lipsum.com/

## 🎈 Flow Details <a name="flow-details"></a>

1. Program Initialization

   - The program initializes and sets up any necessary configurations or variables.
   - An array of MenuItem structures is defined, associating menu options with their corresponding functions.

2. Display Menu
   - Upon starting, the application clears the screen for a clean interface.
   - The runMenu function is called, displaying the menu options via printMenu.
3. User Input
   - Users are prompted to select an option from the menu.
   - The getValidChoice function validates the user's input, ensuring it's within the range of available options.
4. Process Choice
   - Depending on the user's choice, the application executes the associated function from the MenuItem array:
     - "Create and save file" executes createAndSaveFile.
     - "Read file" executes readFiles.
   - Each selected operation has its specific flow, detailed in subsequent steps.
5. Execute Selected Operation
   - The application performs the selected operation, providing feedback or results to the user.
   - For file creation, users input text which is then saved to a file.
   - For file reading, the content of a specified file is displayed.
6. Loop or Exit
   - After completing an operation, the application may return to the main menu for further actions or exit upon user request.
7. Cleanup and Exit
   - On choosing to exit, the application performs any necessary cleanup and terminates gracefully.

## 🎈 Key Operation Details <a name="key"></a>

A. Create and Save File

1.  Prompt for text input.
2.  Capture input until EOF is detected.
3.  Initialize HeaderInfo with input data.
4.  Call WriteToFile to save data.
5.  Confirm successful operation to the user.

B. Read File

1.  Optionally prompt for a filename.
2.  Use ReadFromFile with the filename to read data.
3.  Display file contents or an error message as appropriate.

## 🎈 System Process Details <a name="depth"></a>

### A. Create and Save File Process

The process for creating and saving a file involves several key functions and steps:

1. **User Input Collection**

   - **Function**: `getUserInput`
   - **Description**: Prompts for text input, collecting it into a buffer until EOF is detected.

2. **Prepare Data for Writing**

   - **Function**: `prepareDataForWrite`
   - **Description**: Initializes `HeaderInfo` with input data, calculates total file size, allocates memory for data, and prepares the data, including CRC calculation and payload setup.

3. **Encrypt Data**

   - **Function**: `encryptDecrypt`
   - **Description**: Encrypts the data in preparation for secure writing to the file.

4. **Write Data to File**

   - **Function**: `WriteToFile`
   - **Description**: Manages the actual file writing, including opening the file, writing the encrypted data, handling backup creation, and closing the file.

5. **Free Allocated Memory**
   - **Action**: Frees the memory allocated for the data after writing to prevent memory leaks.

### B. Read File Process

This process involves reading and processing data from a file:

1. **Optional Filename Prompt**

   - **Function**: `getUserInput` (optional)
   - **Description**: Optionally prompts the user for the filename to read.

2. **Read Data from File**

   - **Function**: `ReadFromFile`
   - **Description**: Handles the reading of file data into memory, including file opening, memory allocation, data reading, and file closing.

3. **Validate and Process Data**

   - **Function**: `processValidData`
   - **Description**: Validates the CRC of the read data, decrypts it, and extracts `HeaderInfo` including file ID, read count, and payload.

4. **Display File Contents**

   - **Function**: `printHeaderInfo`
   - **Description**: Outputs the `HeaderInfo` contents, including payload, to the user.

5. **Clean Up**
   - **Function**: `cleanupDataArrays`
   - **Description**: Frees allocated memory used in the reading process to avoid memory leaks.

### Additional Key Functions

- **`CalculateCRC`**: Ensures data integrity by calculating the CRC of data.
- **`encryptDecrypt`**: Secures file contents through encryption before writing and decryption after reading.
- **`GetBackupFilename`**: Generates filenames for backup copies of data.
- **`WriteDataFile`**: Manages low-level file writing operations, including chunked data writing.
- **`ReadDataFromFile`**: Performs low-level file reading operations, loading data into memory.

Each function plays a critical role in ensuring data integrity, security, and effective user interaction within the application's file operation processes.

<p align="center">
  <a href="" rel="noopener">
    <img src="/flow.png" alt="Project logo" />
  </a>
</p>

## ✍️ Authors <a name = "authors"></a>

- [@cristoph143](https://github.com/cristoph143) - author

## 🎉 Acknowledgements <a name = "acknowledgement"></a>

- Hat tip to anyone whose code was used
- Inspiration
- References
