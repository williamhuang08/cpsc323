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

int parse_string(String* string, String* output, Macros* macros);

void* expand_macros(Macros* macros, String* output, char* name, char* replacement);

String* remove_comments(FILE* input_file);

String** parse_macros_S(char* key, char** c, int num_args, int* index);

char** parse_macros(char* key, char** c, int num_args, int* index);

String* extend_file(String* input, String* file, int index);

void* search_macros(Macros* macros, char* name);

void* delete_macros(Macros* macros);

int undefine_macros(Macros* macros, char* name);

int add_macros(Macros* macros, char* name, char* value);

Macros* create_macros();

void free_strings(char** strings, int num);

String* insert_string(String* original, char* insert, int index, int insert_len);

String* add_string(String* string, char* addstring);

void* delete_string(String* string);

char top_string(String* string);

char pop_char(String* string);

void clear_string(String* str);

String* add_char(String* string, char c);

String* create_string();
