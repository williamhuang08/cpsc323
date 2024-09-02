// proj1.h                                          Stan Eisenstat (09/17/15)
//
// System header files and macros for proj1

#define _GNU_SOURCE
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct _string{
    char* instring;
    size_t length;
    size_t capacity;
} String;

typedef struct _macros{
    char** names;
    char** values;
    size_t length;
    size_t capacity;
} Macros;

typedef enum {
    STATE_START,
    STATE_ESCAPE,
    STATE_COMMENT
} Cstate;

typedef enum {
    STATE_BEGIN,
    STATE_TEXT,
    STATE_MACROS,
    STATE_SPECIAL,
    STATE_ESC,
    STATE_END
} Pstate;

typedef enum {
    SUBSTATE_NAME,
    SUBSTATE_ARG
} MSubstate;


// Write message to stderr using format FORMAT
#define WARN(format,...) fprintf (stderr, "proj1: " format "\n", __VA_ARGS__)

// Write message to stderr using format FORMAT and exit.
#define DIE(format,...)  WARN(format,__VA_ARGS__), exit (EXIT_FAILURE)

// Double the size of an allocated block PTR with NMEMB members and update
// NMEMB accordingly.  (NMEMB is only the size in bytes if PTR is a char *.)
#define DOUBLE(ptr,nmemb) realloc (ptr, (nmemb *= 2) * sizeof(*ptr))
