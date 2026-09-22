/*
 * ============================================================
 *  OPERATING SYSTEMS AND SYSTEMS PROGRAMMING (25CS2104E)
 *  Project      : Argument Passing
 *  Section 4    : Team 18
 *  Members      : Shaik Juneeth   (2520030435)
 *                 V Ashish Reddy  (2520030415)
 *                 D Venkata Mahith(2520030497)
 * ------------------------------------------------------------
 *  MODE 1 (command line) :
 *      ./argument_passing hello 25 linux 10.5
 *      -> arguments come from the SHELL through argc / argv
 *
 *  MODE 2 (interactive) :
 *      ./argument_passing
 *      -> the program asks you to TYPE a line, then splits it
 *         into arguments itself, exactly the way the shell does
 *
 *  Build : gcc -Wall -o argument_passing argument_passing.c
 * ============================================================
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
 
#define MAX_ARGS 64
#define MAX_LINE 512
 
/* ---------- help screen ---------- */
static void print_help(const char *prog)
{
    printf("\nUSAGE:\n");
    printf("  %s <arg1> <arg2> ... <argN>   (arguments from command line)\n", prog);
    printf("  %s                            (program asks you to type them)\n\n", prog);
    printf("OPTIONS:\n");
    printf("  -h, --help      Show this help message\n");
    printf("  -v, --version   Show program version\n\n");
}
 
/* ---------- check whether a string is a number ---------- */
static int is_number(const char *s)
{
    int i = 0, digits = 0, dot = 0;
 
    if (s[0] == '+' || s[0] == '-')
        i = 1;
 
    for (; s[i] != '\0'; i++) {
        if (isdigit((unsigned char)s[i]))
            digits++;
        else if (s[i] == '.' && dot == 0)
            dot = 1;
        else
            return 0;
    }
    return digits > 0;
}
 
/* ============================================================
 *  process_arguments()
 *  Displays every argument with its index, length and type,
 *  then prints a summary. Used by BOTH modes.
 * ============================================================ */
static void process_arguments(int count, char *args[])
{
    int    i;
    int    numeric_count = 0;
    int    string_count  = 0;
    double sum           = 0.0;
 
    printf("\nTotal arguments (argc) : %d\n", count);
    printf("User arguments         : %d\n", count - 1);
 
    printf("\n--------------------------------------------------------\n");
    printf("%-9s %-22s %-8s %-10s\n", "INDEX", "VALUE", "LENGTH", "TYPE");
    printf("--------------------------------------------------------\n");
 
    for (i = 0; i < count; i++) {
        const char *type;
 
        if (i == 0) {
            type = "program";
        } else if (is_number(args[i])) {
            type = "number";
            numeric_count++;
            sum += strtod(args[i], NULL);
        } else {
            type = "string";
            string_count++;
        }
 
        char label[24];
        snprintf(label, sizeof(label), "argv[%d]", i);
 
        printf("%-9s %-22s %-8lu %-10s\n",
               label, args[i], (unsigned long)strlen(args[i]), type);
    }
    printf("--------------------------------------------------------\n");
 
    printf("\nSUMMARY\n");
    printf("  Numeric arguments : %d\n", numeric_count);
    printf("  String arguments  : %d\n", string_count);
 
    if (numeric_count > 0) {
        printf("  Sum of numbers    : %.2f\n", sum);
        printf("  Average of numbers: %.2f\n", sum / numeric_count);
    }
 
    printf("\n[OK] All arguments received and processed.\n\n");
}
 
/* ============================================================
 *  split_line()
 *  Breaks a typed line into separate arguments on spaces.
 *  Text inside "double quotes" is kept as ONE argument.
 *  This is the same job the Linux shell performs before it
 *  hands argc / argv to a new process.
 * ============================================================ */
static int split_line(char *line, char *args[], int max)
{
    int   count = 0;
    char *p     = line;
 
    while (*p != '\0' && count < max - 1) {
 
        while (*p == ' ' || *p == '\t')     /* skip leading spaces */
            p++;
        if (*p == '\0')
            break;
 
        if (*p == '"') {                    /* quoted argument */
            p++;
            args[count++] = p;
            while (*p != '\0' && *p != '"')
                p++;
        } else {                            /* normal argument */
            args[count++] = p;
            while (*p != '\0' && *p != ' ' && *p != '\t')
                p++;
        }
 
        if (*p != '\0')
            *p++ = '\0';                    /* terminate this argument */
    }
 
    args[count] = NULL;
    return count;
}
 
/* ============================================================
 *                          main()
 * ============================================================ */
int main(int argc, char *argv[])
{
    printf("\n========================================================\n");
    printf("      ARGUMENT PASSING  -  Team 18  (Section 4)\n");
    printf("========================================================\n");
    printf("Process ID (PID)         : %d\n", getpid());
    printf("Parent Process ID (PPID) : %d\n", getppid());
    printf("Program name (argv[0])   : %s\n", argv[0]);
 
    /* ---------- flags ---------- */
    if (argc > 1 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)) {
        print_help(argv[0]);
        return EXIT_SUCCESS;
    }
    if (argc > 1 && (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0)) {
        printf("\nArgument Passing - Version 2.0\n\n");
        return EXIT_SUCCESS;
    }
 
    /* ---------- MODE 1 : arguments came from the command line ---------- */
    if (argc > 1) {
        printf("\nMODE 1 : arguments supplied by the SHELL at process startup\n");
        process_arguments(argc, argv);
        return EXIT_SUCCESS;
    }
 
    /* ---------- MODE 2 : no arguments, so ask the user ---------- */
    {
        char  line[MAX_LINE];
        char  original[MAX_LINE];
        char *args[MAX_ARGS];
        int   count;
 
        printf("\nMODE 2 : no command-line arguments found (argc = 1)\n");
        printf("The program will now read your input and split it\n");
        printf("into arguments the same way the Linux shell does.\n");
        printf("\nUse \"double quotes\" to keep spaces inside one argument.\n");
        printf("\nEnter your arguments : ");
 
        if (fgets(line, sizeof(line), stdin) == NULL) {
            printf("\n[!] No input received.\n\n");
            return EXIT_FAILURE;
        }
 
        line[strcspn(line, "\n")] = '\0';   /* remove the newline */
        strcpy(original, line);             /* split_line() edits line */
 
        args[0] = argv[0];                  /* argv[0] = program name */
        count   = split_line(line, &args[1], MAX_ARGS - 1) + 1;
 
        if (count == 1) {
            printf("\n[!] You did not enter any arguments.\n");
            print_help(argv[0]);
            return EXIT_FAILURE;
        }
 
        printf("\nYou entered : %s\n", original);
        process_arguments(count, args);
    }
 
    return EXIT_SUCCESS;
}
