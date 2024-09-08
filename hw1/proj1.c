#include "proj1.h"

// **************************************************************************************************************** //
// STRING LIBRARY
// **************************************************************************************************************** //

// Intializes a string struct and creates space on the heap for a char array
String* create_string() {
    String* newstring = malloc(sizeof(String));
    newstring->length = 0;
    newstring->capacity = 2;
    newstring->instring = malloc(sizeof(char) * newstring->capacity);
    newstring->instring[0] = '\0';

    return newstring;
} 

// Adds a char to a given string and appends the null terminating character to the end
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

// Resets the string to empty string
void clear_string(String* str) {
    if (str == NULL) {
        return;
    }
    str->length = 0;    // Reset the length
    str->instring[0] = '\0';  // Set the first character to null terminator to empty the string
}

// Pops the last char from the string, gives error if you pop from an empty string
char pop_char(String* string) {
    char c;
    if (string->length > 0) {
        string->length -= 1;
        c = string->instring[string->length];
        string->instring[string->length] = '\0';
        if (string->length <= string->capacity / 2 && string->capacity / 2 > 1) {
            string->capacity /= 2;
            string->instring = realloc(string->instring, sizeof(char) * string->capacity);
        }
    }
    else {
        fprintf(stderr, "Attempting to pop from an empty string\n");
        return NULL;
    }
    return c;
}                                   

// Returns the last character of the string
char top_string(String* string) {
    return string->instring[string->length - 1];
}

// Frees the char* array and the string
void* delete_string(String* string) {
    free(string->instring);
    free(string);
}


// Adds a string to a string struct by repeatedly calling the addchar function
String* add_string(String* string, char* addstring) {
    int i = 0;
    int length = strlen(addstring);

    while (i < length) {
        add_char(string, addstring[i]);
        i++;
    }
    return string;
}

// Inserts a char* string into a string at a given index
String* insert_string(String* original, char* insert, int index, int insert_len) {
    size_t original_len = original->length;

    char* new_string = malloc(original_len + insert_len + 1);
    if (new_string == NULL) {
        return NULL;
    }
    strncpy(new_string, original->instring, index);
    new_string[index] = '\0'; 

    strncat(new_string, insert, insert_len);

    strncat(new_string, original->instring + index, original_len - index);

    free(original->instring);

    // Update the components of the string struct
    original->instring = new_string;
    original->length = original_len + insert_len; // Update the length of the original string
    original->capacity = original_len + insert_len + 1;

    return original;

}

// Frees an array of char** strings
void free_strings(char** strings, int num) {
    for (int i = 0; i < num; i++) {
        free(strings[i]);
    }
    free(strings);
}

// **************************************************************************************************************** //
// MACROS LIBRARY
// **************************************************************************************************************** //

// Create a macros struct to with arrays to hold names and values at the same indices
Macros* create_macros() {
    Macros* allmacros = malloc(sizeof(Macros));
    allmacros->length = 0;
    allmacros->capacity = 2;
    allmacros->names = malloc(sizeof(char *) * allmacros->capacity);
    allmacros->values = malloc(sizeof(char *) * allmacros->capacity);
}

// Add a macros to the macros struct by adding the name and value to the arrays
int add_macros(Macros* macros, char* name, char* value) {
    // Adds a macros into the the list of macros if it doesn't already exist
    for (int i = 0; i < macros->length; i++){
        if (macros->names[i] != NULL && strcmp(name, macros->names[i]) == 0) {
            return 1;
        }
    }

    if (macros->length + 1 > macros->capacity) {
        macros->capacity *= 2;
        macros->names = realloc(macros->names, sizeof(char *) * macros->capacity);
        macros->values = realloc(macros->values, sizeof(char *) * macros->capacity);
    }
    macros->names[macros->length] = strdup(name);
    macros->values[macros->length] = strdup(value);
    macros->length += 1;

    return 0;
}

// Undefines a macros by replacing the name and value with NULL
int undefine_macros(Macros* macros, char* name) {
    
    // "'Removes' a macros by replacing the name with 'NULL' and the value with 'NULL'"
    bool found = false;

    for (int i = 0; i < macros->length; i++) {
        if (macros->names[i] != NULL && strcmp(name, macros->names[i]) == 0) {
            free(macros->names[i]);
            free(macros->values[i]);
            macros->names[i] = NULL;
            macros->values[i] = NULL;
            return 0;
        }
    }

    if (!found) {
        return 1;
    }
}

// Macros are deleted by freeing each char* in the names and values array and then freeing the pointer to the char* arrays. Finally, the macros struct iteself is freed.
void* delete_macros(Macros* macros) {
    for (int i = 0; i < macros->length; i++){
        if (macros->names[i] != NULL) {
            free(macros->names[i]);
            free(macros->values[i]);
        }
    }
    free(macros->names);
    free(macros->values);
    free(macros);
}

// Searches for a macros by looking if a name in the names array matches the input name
void* search_macros(Macros* macros, char* name) {
    int i;
    i = 0;
    while (i < macros->length) {
        if (macros->names[i] != NULL && strcmp(macros->names[i], name) == 0) {
            return 0;
        }
        i += 1;
    }
    return 1;
}

// Function inserts the contents of a content file into the current string
String* extend_file(String* input, String* file, int index) {
    char* new_string = malloc(input->length + file->length + 1);
    if (new_string == NULL) {
        return NULL;
    }

    strncpy(new_string, input->instring, index);
    new_string[index] = '\0'; // Null-terminate after the copied part

    strncat(new_string, file->instring, file->length);

    strncat(new_string, input->instring + index, input->length - index);
    free(input->instring);

    input->instring = new_string;
    input->length = input->length + file->length;
    input->capacity = input->length + file->length + 1;

    return input;
}

char** parse_macros(char* key, char** c, int num_args, int* index) {
    
    char** output = malloc(sizeof(char*) * num_args);
    int cpy = num_args;
    bool inarg = false;

    while (cpy > 0) {
        int num_escapes = 0;
        int num_braces = 0;
        String* arg = create_string();

        while (1) {
            if (**c == '\\') {
                while (**c != '\0' && **c == '\\') {
                    add_char(arg, **c);
                    num_escapes++;
                    (*index)++;
                    (*c)++;
                }
            } 
            if (num_braces < 0 || **c == '\0') {
                free_strings(output, num_args - cpy);
                delete_string(arg);
                return NULL;
            }
            else if (cpy > 0 && num_braces == 0 && isblank(**c)){
                free_strings(output, num_args - cpy);
                delete_string(arg);
                return NULL;
            }
            else if (**c == '{') {
                if (num_escapes % 2 == 0) {
                    if (inarg) {
                        add_char(arg, **c);
                    }
                    num_braces++;
                    inarg = true;
                }
                else {
                    if (inarg) {
                        add_char(arg, **c);
                    }
                }
            } 
            else if (**c == '}') {
                if (num_escapes % 2 == 0) {
                    // printf("num_args = %d", num_braces);
                    if (inarg && num_braces - 1 != 0) {
                        add_char(arg, **c);
                    }
                    num_braces--;
                }
                else {
                    add_char(arg, **c);
                }
            }
            else {
                add_char(arg, **c);
            }
            num_escapes = 0;
            (*index)++;
            (*c)++;

            if (num_braces == 0) {
                inarg = false;
                break;
            }                   
        }
        output[num_args - cpy] = strdup(arg->instring);
        delete_string(arg);
        cpy -= 1;
    }
    return output;
}


String** parse_macros_S(char* key, char** c, int num_args, int* index) {
    String** output = malloc(sizeof(String*) * num_args);
    int cpy = num_args;
    bool inarg = false;

    while (cpy > 0) {
        int num_escapes = 0;
        int num_braces = 0;
        String* arg = create_string();

        while (1) {
            if (**c == '\\') {
                while (**c != '\0' && **c == '\\') {
                    add_char(arg, **c);
                    num_escapes++;
                    (*index)++;
                    (*c)++;
                }
            } 
            if (num_braces < 0 || **c == '\0') {
                delete_string(arg);
                free_strings(output, num_args - cpy);
                return NULL;
            }
            else if (cpy > 0 && num_braces == 0 && isblank(**c)){
                delete_string(arg);
                free_strings(output, num_args - cpy);
                return NULL;
            }
            else if (**c == '{') {
                if (num_escapes % 2 == 0) {
                    if (inarg) {
                        add_char(arg, **c);
                    }
                    num_braces++;
                    inarg = true;
                }
                else {
                    if (inarg) {
                        add_char(arg, **c);
                    }
                }
            } 
            else if (**c == '}') {
                if (num_escapes % 2 == 0) {
                    // printf("num_args = %d", num_braces);
                    if (inarg && num_braces - 1 != 0) {
                        add_char(arg, **c);
                    }
                    num_braces--;
                }
                else {
                    add_char(arg, **c);
                }
            }
            else {
                add_char(arg, **c);
            }
            num_escapes = 0;
            (*index)++;
            (*c)++;

            if (num_braces == 0) {
                inarg = false;
                break;
            }                   
        }
        output[num_args - cpy] = arg;
        cpy -= 1;
    }
    return output;
    // String** output = malloc(sizeof(String*) * num_args);
    // int cpy = num_args;
    // bool inarg = false;
    // bool escaped = false;

    // while (cpy > 0) {
    //     int num_escapes = 0;
    //     int num_braces = 0;
    //     String* arg = create_string();

    //     while (1) {
    //         if (**c == '\\') {
    //             while (**c != '\0' && **c == '\\') {
    //                 add_char(output, *c);
    //                 num_escapes++;
    //                 (*index)++;
    //                 (*c)++;
    //             }
    //             // if (num_escapes % 2 == 0) {
    //             //     num_escapes = num_escapes / 2;
    //             //     while (num_escapes > 0) {
    //             //         add_char(arg, '\\');
    //             //         num_escapes -= 1;
    //             //     }
    //             // } 
    //             // else {
    //             //     num_escapes = (num_escapes - 1) / 2 + 1;
    //             //     while (num_escapes > 0) {
    //             //         add_char(arg, '\\');
    //             //         num_escapes -= 1;
    //             //     }
    //             //     if (**c == '\\' || **c == '#' || **c == '%' || **c == '{' || **c == '}') {
    //             //         add_char(arg, **c);
    //             //     }
    //             // }
    //         }
    //         if (num_braces < 0 || **c == '\n' || **c == '\0') {
    //             delete_string(arg);
    //             free_strings(output, num_args - cpy);
    //             return NULL;
    //         }
    //         else if (cpy > 0 && num_braces == 0 && isblank(**c)){
    //             delete_string(arg);
    //             free_strings(output, num_args - cpy);
    //             return NULL;
    //         }
    //         else if (**c == '{') {
    //             if (num_escapes % 2 == 0) {
    //                 if (inarg) {
    //                     add_char(arg, **c);
    //                 }
    //                 num_braces++;
    //                 inarg = true;
    //             }
    //         } 
    //         else if (**c == '}') {
    //             if (num_escapes % 2 == 0) {
    //                 if (inarg && num_braces - 1 != 0) {
    //                     add_char(arg, **c);
    //                 }
    //                 num_braces--;
    //             }
    //             else {
    //                 if (inarg && num_braces - 1 != 0) {
    //                     add_char(arg, **c);
    //                 }
    //             }
    //         }
    //         else {
    //             add_char(arg, **c);
    //         }
    //         num_escapes = 0;
    //         (*index)++;
    //         (*c)++;

    //         if (num_braces == 0) {
    //             inarg = false;
    //             break;
    //         }                   
    //     }
    //     output[num_args - cpy] = arg;
    //     cpy -= 1;
    // }
    // return output;
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



void* expand_macros(Macros* macros, String* output, char* name, char* replacement) {
    int i = 0;

    while (macros->names[i] == NULL || strcmp(macros->names[i], name) != 0) {
        i++;
    }
    int j = 0;
    Pstate curr_state = STATE_BEGIN;
    char c = macros->values[i][j];
    while (c != '\0') {
        if (c == '\\') {
            int num_escapes = 0;
            while (c != '\0' && c == '\\') {
                num_escapes++;
                j++;
                c = macros->values[i][j];
            }
                
            if (num_escapes % 2 == 0) {
                num_escapes = num_escapes / 2;
                while (num_escapes > 0) {
                    add_char(output, '\\');
                    num_escapes -= 1;
                }
            } 
            else {
                num_escapes = (num_escapes - 1) / 2 + 1;
                while (num_escapes > 0) {
                    add_char(output, '\\');
                    num_escapes -= 1;
                }
                if (c == '\\' || c == '#' || c == '%' || c == '{' || c == '}') {
                    pop_char(output);
                }
                add_char(output, c);
                j++;
                c = macros->values[i][j];
            }
        }    
        if (c == '#') {
            add_string(output, replacement);
        } else {
            add_char(output, c);
        }
        if (c != '\0') {
            j++;
            c = macros->values[i][j];
        }
    }
    // while (c != '\0') {
    //     switch(curr_state) {
    //         case STATE_BEGIN:
    //             if (c != '\\') {
    //                 curr_state = STATE_TEXT;
    //             }
    //             else {
    //                 add_char(output, c);
    //                 j++;
    //                 c = macros->values[i][j];
    //                 curr_state = STATE_ESC;
    //             }
    //             break;
            
    //         case STATE_ESC:
    //             if (c == '\\' || c == '#' || c == '%' || c == '{' || c == '}') { // if the current character is a special character, pop the last '\' and add the special character
    //                 pop_char(output);
    //                 add_char(output, c);
    //                 j++;
    //                 c = macros->values[i][j];
    //                 curr_state = STATE_TEXT;
    //             }
    //             // else if (isalnum(c)){ // if the character is alphanumeric it is a macros
    //             //     pop_char(output);

    //             //     curr_state = STATE_MACROS;
    //             // }
    //             else { // if the character is none, then add the current character, also keep the '/'
    //                 add_char(output, c);
    //                 j++;
    //                 c = macros->values[i][j];
    //                 curr_state = STATE_TEXT;
    //             }
    //             break;

    //         case STATE_TEXT:
    //             if (c == '\\') { // if the character is a '\', then add it and change to the escape state
    //                 curr_state = STATE_ESC;
    //             }
    //             else if (c == '#') {
    //                 add_string(output, replacement);
    //             }
    //             else {
    //                 add_char(output, c);
    //             }
    //             j++;
    //             c = macros->values[i][j];
    //             break;
    //     }
    // }
}

global_state = STATE_BEGIN;
String *global_key;
String *temp;

int parse_string(String* string, String* output, Macros* macros) {
    char* c = string->instring;
    int index = 0;
    while (*c != '\0') {
        switch (global_state) {

            case STATE_BEGIN:
                if (*c != '\\') {
                    global_state = STATE_TEXT;
                } else {
                    add_char(output, *c);
                    index++;
                    c++;
                    global_state = STATE_ESC;
                }
                break;

            case STATE_ESC:
                if (*c == '\\' || *c == '#' || *c == '%' || *c == '{' || *c == '}') { 
                    pop_char(output);  // Remove the escape character
                    add_char(output, *c);
                    index++;
                    c++;
                    global_state = STATE_TEXT;
                } else if (isalnum(*c)) {
                    pop_char(output);  // Remove the escape character
                    global_state = STATE_MACROS;
                } else {
                    add_char(output, *c);
                    index++;
                    c++;
                    global_state = STATE_TEXT;
                }
                break;

            case STATE_TEXT:
                if (*c == '\\') {
                    global_state = STATE_ESC;
                }
                add_char(output, *c);
                index++;
                c++;
                break;

            case STATE_MACROS: {
                String* key = create_string();

                parse_macro_key:
                while (*c != '{') {
                    if (*c == '\0') {
                        global_key = create_string();
                        add_string(global_key, key->instring);
                        if (search_macros(macros, key->instring) != 0) {
                            add_char(output, '\\');
                            add_string(output, key->instring);
                            global_state = STATE_BEGIN;
                        }
                        delete_string(key);
                        return;
                    }
                    if (!isalnum(*c)) {
                        delete_string(key);
                        fprintf(stderr, "Definition error: macros names must be alphanumeric\n");
                        return 1;
                    }
                    add_char(key, *c);
                    index++;
                    c++;
                }

                if (strcmp(key->instring, "def") == 0) {
                    char** args = parse_macros(key, &c, 2, &index);
                    if (args == NULL) {
                        delete_string(key);
                        fprintf(stderr, "Definition error: The braces are not balanced for a macro definition.\n");
                        return 1;
                    }
                    // printf("FIRST ARGUMENT = %s ", args[0]);
                    // printf("SECOND ARGUMENT = %s \n", args[1]);
                    int res = add_macros(macros, args[0], args[1]);
                    if (res == 1) {
                        free_strings(args, 2);
                        delete_string(key);
                        fprintf(stderr, "Definition error: Invalid macro name definition, trying to redefine macros.\n");
                        return 1;
                    }
                    free_strings(args, 2);

                } else if (strcmp(key->instring, "if") == 0) {
                    char** args = parse_macros(key, &c, 3, &index);
                    if (args == NULL) {
                        delete_string(key);
                        fprintf(stderr, "Definition error: The braces are not balanced for a macro definition.\n");
                        return 1;
                    }

                    char* logic_str;
                    if (strcmp(args[0], "") != 0) {
                        logic_str = args[1];
                    }
                    else {
                        logic_str = args[2];
                    }
                    // printf("logic_str = %s\n", logic_str);
                    int i = 0;
                    while (logic_str[i] != '\0') {
                        // add_char(output, logic_str[i]);
                        i += 1;
                    }
                    insert_string(string, logic_str, index, i + 1);
                    c = string->instring + index;


                } else if (strcmp(key->instring, "ifdef") == 0) {
                    char** args = parse_macros(key, &c, 3, &index);
                    if (args == NULL) {
                        delete_string(key);
                        fprintf(stderr, "Definition error: The braces are not balanced for a macro definition.\n");
                        return 1;
                    }

                    char* logic_str;
                    if (search_macros(macros, args[0]) == 0) {
                        logic_str = args[1];
                    }
                    else {
                        logic_str = args[2];
                    }
                    int i = 0;
                    while (logic_str[i] != '\0') {
                        i += 1;
                    }
                    insert_string(string, logic_str, index, i + 1);
                    // printf("string after inserted = %s\n", string->instring);
                    c = string->instring + index;


                } else if (strcmp(key->instring, "undef") == 0) {
                    char** args = parse_macros(key, &c, 1, &index);
                    if (args == NULL) {
                        delete_string(key);
                        fprintf(stderr, "Definition error: The braces are not balanced for a macro definition.\n");
                        return 1;
                    }
                    // printf("arg to undef = %s\n", args[0]);
                    // printf("should not be empty here %s", output->instring);
                    int val = undefine_macros(macros, args[0]);
                    if (val == 1) {
                        delete_string(key);
                        free_strings(args, 1);
                        fprintf(stderr, "Cannot undefine a macros that has not been defined.\n");
                        return 1;
                    }
                    free_strings(args, 1);


                } else if (strcmp(key->instring, "include") == 0) {
                    char** args = parse_macros(key, &c, 1, &index);
                    if (args == NULL) {
                        delete_string(key);
                        fprintf(stderr, "Definition error: The braces are not balanced for a macro definition.\n");
                        return 1;
                    }

                    FILE* file;
                    // String* cleaned_name = create_string();
                    // parse_string(args[0], cleaned_name, macros);
                    file = fopen(args[0], "r");
                    if (file == NULL) {
                        free_strings(args, 1);
                        delete_string(key);
                        fprintf(stderr, "File can't be opened.");
                        return 1;
                    }

                    String* cleaned = remove_comments(file);
                    string = extend_file(string, cleaned, index);
                    delete_string(cleaned);
                    c = string->instring + index;
                    free_strings(args, 1);
                    fclose(file);


                } else if (strcmp(key->instring, "expandafter") == 0) {
                    // Expand macro, process recursively
                    String** args = parse_macros_S(key, &c, 2, &index);

                    if (args == NULL) {
                        delete_string(key);
                        fprintf(stderr, "Definition error: The braces are not balanced for a macro definition.\n");
                        return 1;
                    }
                    // printf("args[0] = %s ", args[0]->instring);                    
                    // printf("args[1] = %s ", args[1]->instring);

                    temp = create_string();
                    global_state = STATE_BEGIN;
                    // printf("Before parsing: temp = %s\n", temp ? temp->instring : "NULL");

                    parse_string(args[1], temp, macros);
                    if (global_state == STATE_MACROS){
                        delete_string(key);
                        free_strings(args, 1);
                        key = global_key;
                        goto parse_macro_key; 
                    }
                    // printf("temp = %s", temp->instring);
                    
                    // printf("inserted = %s", args[0]->instring);
                    // printf("string = %s", string->instring);
                    insert_string(string, args[0]->instring, index, args[0]->length);
                    // printf("inserted = %s", string->instring);
                    insert_string(string, temp->instring, index + args[0]->length, temp->length);
                    // insert_string(string, temp->instring, index + args[0]->length - 1, temp->length);
                    c = string->instring + index;
                    free(temp->instring);
                    delete_string(args[0]);
                    delete_string(args[1]);
                    free(args);
                } else if (search_macros(macros, key->instring) == 0) {
                    // Expand macro, process recursively
                    char** args = parse_macros(key, &c, 1, &index);
                    if (args == NULL) {
                        delete_string(key);
                        fprintf(stderr, "Definition error: The braces are not balanced for a macro definition.\n");
                        return 1;
                    }
                    String* expanded_macro = create_string();
                    // printf("arg0 = %s", args[0]);
                    expand_macros(macros, expanded_macro, key->instring, args[0]);
                    // printf("expanded macros = %s \n", expanded_macro->instring);
                    // Call parse_string recursively on the expanded macro output
                    // String* temp_output = create_string();
                    // add_string(temp_output, output->instring);

                    global_state = STATE_BEGIN;
                    parse_string(expanded_macro, output, macros);
                    if (global_state == STATE_MACROS){
                        delete_string(expanded_macro);
                        delete_string(key);
                        free_strings(args, 1);
                        key = global_key;
                        goto parse_macro_key; 
                    }
                    delete_string(expanded_macro);
                    free_strings(args, 1);
                }
                else {
                    delete_string(key);
                    fprintf(stderr, "This is an undefined macros\n");
                    return 1;
                }
                global_state = STATE_BEGIN;
                delete_string(key);

                break;
            }

            default:
                fprintf(stderr, "Unknown state\n");
                return 1;
        }
    }

    return 0;  // Return success
}


// /c/cs323/proj1/proj1 test.txt
// ./proj1 test.txt
int main(int argc, char* argv[]) {
    
    String* total = create_string();
    String* output = create_string();
    Macros* macros = create_macros();
    String* cleaned_string;
    temp = NULL;
    global_key = NULL;
    // temp = create_string();
    // Check if there are any input files
    if (argc > 1) {

        // Go through each file and remove the comments
        for (int i = 1; i < argc; i++) {
            FILE* file;
            file = fopen(argv[i], "r");
            if (file) {
                cleaned_string = remove_comments(file);
                add_string(total, cleaned_string->instring);
                delete_string(cleaned_string);
                fclose(file);
            }

            // If any of the files are cannot be opened, return
            else {
                delete_string(output);
                delete_string(total);
                fprintf(stderr, "Invalid input file: File cannot be opened");
                return 1;
            }
        }
    }

    // If there are not input files, read from stdin
    else {
        String* cleaned = remove_comments(stdin);
        add_string(total, cleaned->instring);
        delete_string(cleaned);
    }
    // printf("\n cleaned before state = %s\n", cleaned_string->instring);
    int correct = parse_string(total, output, macros);
    // printf("%d\n", cleaned_string->length);
    if (correct != 1) {
        printf("%s", output->instring);
    }
    delete_string(output);
    delete_string(total);
    delete_macros(macros);
    // if (global_key != NULL) {
    //     free(global_key->instring);
    // }
    // free(global_key);
    // if (global_key != NULL) {
    //     delete_string(global_key);
    // }
    if (temp != NULL){
        free(temp);
    }
    
}


