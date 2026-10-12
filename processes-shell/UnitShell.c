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

// Function to print banner
void printBanner() {
    printf("\n**********************************************\n");
    printf("           ********** My Shell ************\n");
    printf("**********************************************\n\n");
}

// main loop that execpts argurment
int main(int argc, char *argv[]) {

    printBanner();    // Printing banner to separate thingy

    // never ending while for shell to keep going unless you type EXIT
    while(1){
        
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
