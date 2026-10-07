/**
 * @file main.c
 * @brief Application entry point.
 */

#include <stdio.h>
#include "module.h"

int main(int argc, char *argv[]) {
    // Suppress unused parameter warnings if not used immediately
    (void)argc;
    (void)argv;

    printf("Starting application...\n");

    ModuleConfig_t config = {
        .id = 101,
        .value = 4.2f
    };

    if (Module_Init(&config) != 0) {
        fprintf(stderr, "Failed to initialize module.\n");
        return 1;
    }

    Module_Process();

    return 0;
}
