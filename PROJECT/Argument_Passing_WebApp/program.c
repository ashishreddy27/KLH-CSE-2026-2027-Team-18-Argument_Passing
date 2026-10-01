/**
 * PROJECT: ARGUMENT PASSING
 * Systems Programming / Linux C Implementation
 * Demonstrates argc, argv, data type detection (Numbers vs Strings), 
 * pointer memory addresses, and POSIX NULL termination.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// Helper to check if an argument is numeric
int is_numeric(const char *str) {
    if (!str || !*str) return 0;
    if (*str == '-' || *str == '+') str++;
    if (!*str) return 0;
    while (*str) {
        if (!isdigit((unsigned char)*str)) return 0;
        str++;
    }
    return 1;
}

int main(int argc, char *argv[]) {
    printf("====================================================================\n");
    printf("        ARGUMENT PASSING - OS & SYSTEMS PROGRAMMING                 \n");
    printf("====================================================================\n");
    
    // Display total argument count
    printf("Argument Count (argc): %d\n", argc);
    printf("--------------------------------------------------------------------\n");
    printf("%-8s %-18s %-22s %-18s\n", "INDEX", "ARGUMENT VALUE", "DATA TYPE / FORMAT", "MEMORY POINTER");
    printf("--------------------------------------------------------------------\n");

    // Loop through each argument vector entry
    for (int i = 0; i < argc; i++) {
        char type_desc[32];
        if (i == 0) {
            snprintf(type_desc, sizeof(type_desc), "Executable (%lu chars)", strlen(argv[i]));
        } else if (is_numeric(argv[i])) {
            snprintf(type_desc, sizeof(type_desc), "Number (int: %d)", atoi(argv[i]));
        } else {
            snprintf(type_desc, sizeof(type_desc), "String (%lu chars)", strlen(argv[i]));
        }

        printf("argv[%-2d]  %-18s %-22s %p\n", 
               i, 
               argv[i], 
               type_desc, 
               (void *)argv[i]);
    }

    printf("--------------------------------------------------------------------\n");
    
    // POSIX Verification: argv[argc] must be NULL
    if (argv[argc] == NULL) {
        printf("POSIX Verification: argv[%d] is NULL (Standard Compliant)\n", argc);
    } else {
        printf("POSIX Verification Warning: argv[%d] is not NULL\n", argc);
    }

    if (argc == 1) {
        printf("\n[INFO] No additional arguments supplied (only argv[0]).\n");
    } else {
        printf("\n[SUCCESS] Successfully received %d user argument(s).\n", argc - 1);
    }
    printf("====================================================================\n");

    return 0;
}
