#include "proj1.h"

// **************************************************************************************************************** //
// STRING LIBRARY
// **************************************************************************************************************** //

String* create_string() {
    String* newstring = malloc(sizeof(String));
    newstring->length = 0;
    newstring->capacity = 2;
    newstring->instring = malloc(sizeof(char) * newstring->capacity);

    return newstring;
} 

String* add_char(String* string, char c) {
    if (string->length + 2 > string->capacity) {
        string->capacity *= 2;
        string->instring = realloc(string->instring, sizeof(char) * string->capacity);
    }
    string->instring[string->length] = c;
    string->instring[string->length + 1] = '\0';
    string->length += 1;

    return string;
}

String* pop_char(String* string) {
    if (string->length > 0) {
        string->length -= 1;
        string->instring[string->length] = '\0';
        if (string->length <= string->capacity / 2 && string->capacity / 2 > 0) {
            string->capacity /= 2;
            string->instring = realloc(string->instring, sizeof(char) * string->capacity);
        }
    }
    else {
        fprintf(stderr, "Attempting to pop from an empty string\n");
    }
    return string;
}                                   

void* delete_string(String* string) {
    free(string->instring);
    free(string);
}

String* add_string(String* string, char* addstring) {
    int i = 0;
    int length = strlen(addstring) + 1;

    while (i < length) {
        add_char(string, addstring[i]);
        i++;
    }
    return string;
}

// **************************************************************************************************************** //
// MACROS LIBRARY
// **************************************************************************************************************** //

Macros* create_macros() {
    Macros* allmacros = malloc(sizeof(Macros));
    allmacros->length = 0;
    allmacros->capacity = 2;
    allmacros->names = malloc(sizeof(char *) * allmacros->capacity);
    allmacros->values = malloc(sizeof(char *) * allmacros->capacity);
}

int add_macros(Macros* macros, char* name, char* value) {
    
    // Adds a macros into the the list of macros if it doesn't already exist
    for (int i = 0; i < macros->length; i++){
        if (strcmp(name, macros->names[i])) {
            return 1;
        }
    }

    if (macros->length + 1 > macros->capacity) {
        macros->capacity *= 2;
        macros->names = realloc(macros->names, sizeof(char *) * macros->capacity);
        macros->values = realloc(macros->values, sizeof(char *) * macros->capacity);
    }
    macros->names[macros->length] = name;
    macros->values[macros->length] = value;
    macros->length += 1;

    return 0;
}

int undefine_macros(Macros* macros, char* name) {
    
    // "'Removes' a macros by replacing the name with '$' and the value with ''"
    bool found = false;

    for (int i = 0; i < macros->length; i++) {
        if (strcmp(name, macros->names[i])) {
            macros->names[i] = NULL;
            macros->values[i] = NULL;
        }
    }

    if (!found) {
        return 1;
    }
}

void* delete_macros(Macros* macros) {
    for (int i = 0; i < macros->length; i++){
        free(macros->names[i]);
        free(macros->values[i]);
    }
    free(macros->names);
    free(macros->values);
    free(macros);
}

void* include_macros(Macros* macros, char* path, String* output) {

    // Opens the file at the path, processes the macros in the file, and adds it to output
    

}

char** parse_macros(char* key, char** c, int num_args) {
    
    char** output = malloc(sizeof(char*) * num_args);
    int cpy = num_args;

    while (cpy > 0) {
        int num_braces = 0;
        String* arg = create_string();

        while (1) {
            if (num_braces < 0 || **c == '\n' || **c == EOF) {
                return NULL;
            }
            else if (**c == '{') {
                num_braces++;
            } 
            else if (**c == '}') {
                num_braces--;
            }
            else {
                add_char(arg, **c);
            }
            (*c)++;

            if (num_braces == 0) {
                break;
            }
            if (num_braces < 0) {
                return NULL;
            }                      
        }
        output[num_args - cpy] = arg->instring;
        cpy -= 1;
    }
    return output;
}


String* remove_comments(FILE* input_file) {
    String* string = create_string();
    Cstate curr_state = STATE_START;
    bool seen_newline = false;
    int c;

    while ((c = getc(input_file)) != EOF) {
        switch (curr_state) {

            case STATE_START:
                if (c == '\\') {
                    add_char(string, c);
                    curr_state = STATE_ESCAPE;
                }
                else if (c == '%') {
                    curr_state = STATE_COMMENT;     
                }
                else if (seen_newline) {
                    if (!isblank(c)){
                        add_char(string, c);
                        seen_newline = false;
                    }
                }
                else {
                    add_char(string, c);
                }
                break;

            case STATE_ESCAPE:
                add_char(string, c);
                curr_state = STATE_START;
                break;

            case STATE_COMMENT:
                if (c == EOF || c == '\n') {
                    seen_newline = true;
                    curr_state = STATE_START;
                }
                break;
        }
    }
    return string;
}



int parse_string(String* string, String* output) {
    char* c = string->instring;
    Pstate curr_state = STATE_BEGIN;
    Macros* macros = create_macros();

    while (*c != '\0') {
        switch (curr_state) {

            case STATE_BEGIN:
                if (*c != '\\') {
                    curr_state = STATE_TEXT;
                }
                else {
                    add_char(output, *c);
                    c++;
                    curr_state = STATE_ESC;
                }
                break;

            case STATE_ESC:
                if (*c == '\\' || *c == '#' || *c == '%' || *c == '{' || *c == '}') { // if the current character is a special character, pop the last '\' and add the special character
                    pop_char(output);
                    add_char(output, *c);
                    c++;
                    curr_state = STATE_TEXT;
                }
                else if (isalnum(*c)){ // if the character is alphanumeric it is a macros
                    pop_char(output);
                    curr_state = STATE_MACROS;
                }
                else { // if the character is none, then add the current character, also keep the '/'
                    add_char(output, *c);
                    c++;
                    curr_state = STATE_TEXT;
                }
                break;

            case STATE_TEXT:
                if (*c == '\\') { // if the character is a '\', then add it and change to the escape state
                    curr_state = STATE_ESC;
                }
                add_char(output, *c);
                c++;
                break;

            case STATE_MACROS:
                String* key = create_string();

                while (*c != '{') {
                    if (*c == '\0' || !isalnum(*c)) {
                        return 1;
                    }
                    add_char(key, *c);
                    c++;
                }
                if ((strcmp(key->instring, "def")) == 0) {
                    char** args = parse_macros(key, &c, 2);

                    if (args == NULL) {
                        // error handling for unbalanced braces
                        fprintf(stderr, "Definition error: The braces are not balanced for a macro definition.");
                        return 1;
                    }

                    int res = add_macros(macros, args[0], args[1]);
                    if (res == 1) {
                        // error handling for invalid macros definitions
                        return 1;
                    }
                }
                else if (strcmp(key->instring, "undef") == 0) {
                    
                    char** args = parse_macros(key, &c, 1);
                    if (args == NULL) {
                        // error handling for unbalanced braces
                        fprintf(stderr, "Definition error: The braces are not balanced for a macro definition.");
                        return 1;
                    }

                }
                curr_state = STATE_BEGIN;
                break;
        }
    }
    return 0;
}

// /c/cs323/proj1/proj1 test.txt
// ./proj1 test.txt
int main(int argc, char* argv[]) {
    
    String* cleaned_string;
    String* output = create_string();
    // Check if there are any input files
    if (argc > 1) {

        // Go through each file and remove the comments
        for (int i = 1; i < argc; i++) {
            FILE* file;
            file = fopen(argv[i], "r");
            if (file) {
                cleaned_string = remove_comments(file);
            }

            // If any of the files are cannot be opened, return
            else {
                fprintf(stderr, "Invalid input file: File cannot be opened");
                return 1;
            }
        }
    }

    // If there are not input files, read from stdin
    else {
        cleaned_string = remove_comments(stdin);
    }
    parse_string(cleaned_string, output);
    printf("%s\n", cleaned_string->instring);
    printf("%d\n", cleaned_string->length);
    printf("%s\n", output->instring);
}

