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
    printf("\n\n******************* My Shell ******************\n\n");
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
