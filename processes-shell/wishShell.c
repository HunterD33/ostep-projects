// copy code into file in linux then run in terminal also look on blackboard week 6 to look at examples and practices
// coding in c 
// so when done coding copy it to here to save and share

// main code
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_INPUT 1024   // Max length of user input
#define MAX_ARGS 64      // Max number of arguments

// Function to print banner to separate my shell from the stuff before
void printBanner() {
    printf("\n\n******************* Wish Shell ******************\n\n");
}

// Printing directory before input is taken, if its too long then exit
void printDirectory(){
    char dir[1024];    // max length is 1024

    if(getcwd(dir, sizeof(dir)) != NULL){
        printf("\nDir: %s", dir);    // printing directory
    }else{
        printf("\ngetcwd failed");    // telling you that dir is null or size is too long
        return;        // exiting function saftly
    }
}

// Function to take input from the user
void takeInput(char *input) {
    printf("\n>>> ");  // Prompt

    // fgets = file get string
    if (fgets(input, MAX_INPUT, stdin) == NULL) {
        // If input fails exit shell
        printf("\nExiting...\n");
        exit(0);
    }
    
    // Remove trailing newline character
    input[strcspn(input, "\n")] = 0;
    // measures the length of the initial segment of a string that 
    //     does not contain any characters from a specified rejection set 
}

// Function to split input into command + arguments
void parseInput(char *input, char **args) {
    int i = 0;

    // Tokenize input based on space
    args[i] = strtok(input, " ");    // it is used to split a larger string into smaller pieces called "tokens"

    // Continue splitting until NULL or max args reached
    while (args[i] != NULL && i < MAX_ARGS - 1) {
        i++;
        args[i] = strtok(NULL, " ");
    }
}

// Function to handle built-in shell commands
int handleBuiltIn(char **args) {
    if (args[0] == NULL) return 1;  // Empty command

    // Exit command
    if (strcmp(args[0], "exit") == 0) {
        printf("Exiting shell...\n");
        exit(0);
    }

    // Change directory command
    if (strcmp(args[0], "cd") == 0) {
        if (args[1] == NULL) {
            printf("Expected argument to \"cd\"\n");
        } else {
            // Change directory and handle error
            if (chdir(args[1]) != 0) {
                perror("cd failed");
            }
        }
        return 1;
    }

    // Help command
    if (strcmp(args[0], "help") == 0) {
        printf("\nSimple Shell Help\n");
        printf("Built-in commands:\n");
        printf("  cd <dir>\n  exit\n  help\n");
        printf("Supports pipes: ls | grep txt\n");    // '|' is pipe
        return 1;
    }

    return 0; // Not a built-in command
}

// Function to execute a normal (non-piped) command
void executeCommand(char **args) {
    pid_t pid = fork();  // Create a child process

    if (pid == 0) {
        // Child process executes command
        if (execvp(args[0], args) < 0) {
            perror("Command failed"); // If execution fails prints a descriptive error message
        }
        exit(1);
    } else {
        // Parent process waits for child to finish
        wait(NULL);
    }
}

// Function to execute two commands connected by a pipe
void executePiped(char **args1, char **args2) {
    int pipefd[2];
    pipe(pipefd);  // Create pipe '|' this is its symbol
    // It takes an array of two integers and fills them with file descriptors
    // pipefd[0]: The read end of the pipe (used to read data out).
    // pipefd[1]: The write end of the pipe (used to write data in)

    pid_t p1 = fork();

    if (p1 == 0) {
        // First child process (left command)
        dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to pipe write end
        close(pipefd[0]); // Close unused read end
        close(pipefd[1]);

        execvp(args1[0], args1); // Execute first command
        perror("Pipe cmd1 failed");
        exit(1);
    }

    pid_t p2 = fork();

    if (p2 == 0) {
        // Second child process (right command)
        dup2(pipefd[0], STDIN_FILENO); // Redirect stdin to pipe read end
        close(pipefd[1]); // Close unused write end
        close(pipefd[0]);

        execvp(args2[0], args2); // Execute second command
        perror("Pipe cmd2 failed");
        exit(1);
    }

    // Parent process closes both ends of pipe
    close(pipefd[0]);
    close(pipefd[1]);

    // Wait for both child processes to complete
    wait(NULL);
    wait(NULL);
}

// Function to detect and split input around a pipe '|'
int parsePipe(char *input, char **left, char **right) {
    char *pipePos = strchr(input, '|'); // Find pipe symbol
    // strchr finds the first occurance of a character insided a null-termination string

    if (!pipePos) return 0; // No pipe found

    *pipePos = '\0'; // Split string at pipe
    pipePos++;       // Move to right side

    // Skip leading spaces in right command
    while (*pipePos == ' ') pipePos++;

    *left = input;    // Left command
    *right = pipePos; // Right command

    return 1; // Pipe exists
}

// main loop that execpts argurment
int main(]) {
    // defining inputs and their max length
    char input[MAX_INPUT];
    char *args1[MAX_ARGS], *args2[MAX_ARGS];
    
    printBanner();    // Printing banner to separate thingy
    
    // never ending while for shell to keep going unless you type EXIT
    while(1){
        printDirectory();    // printing directory before input
        takeInput(input);    // taking user input

        if (strlen(input) == 0) continue; // Ignore empty input

        char *left, *right;

        // Check if input contains a pipe
        if (parsePipe(input, &left, &right)) {
            parseInput(left, args1);   // Parse left command
            parseInput(right, args2);  // Parse right command

            executePiped(args1, args2); // Execute piped commands
        } else {
            parseInput(input, args1); // Parse normal command

            // Handle built-in commands first
            if (handleBuiltIn(args1)) continue;

            executeCommand(args1); // Execute external command
        }
    }

    return 0;  
    }
}

/*
Examples and Practices not for a grade 
Copy the example files into a working folder and compile every program with warnings on:
mkdir -p ~/lab5 && cd ~/lab5
gcc -Wall -Wextra -g ex1.c -o ex1
./ex1
-Wall -Wextra //makes the compiler report many of the bugs in this lab. Read every warning before you run the program.
*/
