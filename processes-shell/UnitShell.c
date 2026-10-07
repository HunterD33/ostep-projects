/**
 * @file module.h
 * @brief Brief description of what this module does.
 * @author Your Name
 * @date 2026-10-07
 */

#ifndef MODULE_H
#define MODULE_H

/* --- Standard Library Includes --- */
#include <stdint.h>

/* --- Macro Definitions / Constants --- */
#define MAX_BUFFER_SIZE 1024

/* --- Custom Type Definitions (Enums, Structs) --- */
typedef struct {
    int32_t id;
    float value;
} ModuleConfig_t;

/* --- Public Function Prototypes (API) --- */

/**
 * @brief Initializes the module with the specified configuration.
 * @param config Pointer to the configuration structure.
 * @return 0 on success, non-zero error code on failure.
 */
int32_t Module_Init(const ModuleConfig_t *config);

/**
 * @brief Processes the module data.
 * @return Current status code.
 */
int32_t Module_Process(void);

#endif /* MODULE_H */
