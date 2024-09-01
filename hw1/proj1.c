#include "proj1.h"

String* create_string() {
    String* newstring = malloc(sizeof(String));
    newstring->length = 0;
    newstring->capacity = 2;
    newstring->instring = malloc(sizeof(char) * newstring->capacity);

    return newstring;
} 

String* add_char(String* string, char c) {
    if (string->length + 1 > string->capacity) {
        string->capacity *= 2;
        string->instring = realloc(string->instring, sizeof(char) * string->capacity);
    }
    string->instring[string->length] = c;
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
                // bool seen_newline = false;
                if (c == EOF || c == '\n') {
                    seen_newline = true;
                    curr_state = STATE_START;
                }
                break;
        }
    }
    return string;
}



String* parse_string(String* string) {
    char* strcln = string->instring;
    Pstate curr_state = STATE_BEGIN;
    char c = *strcln;
    String* output = create_string();

    while (c != '\0') {
        switch (curr_state) {

            case STATE_BEGIN:
                if (c != '\\') {
                    curr_state = STATE_TEXT;
                }
                else {
                    curr_state = STATE_ESC;
                }

            case STATE_ESC:
                if (c == '\\' || c == '#' || c == '%' || c == '{' || c == '}') {
                    strcln++;
                    curr_state = STATE_TEXT;
                }
                else if (isalnum(c)){
                    curr_state = STATE_MACROS;
                }
                else {
                    curr_state = STATE_TEXT;
                }

            case STATE_TEXT:
                add_char(output, c);
                strcln++;
        }
    }
}


int main(int argc, char* argv[]) {
    
    String* cleaned_string;
    String* output;
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
    // output = parse_string(cleaned_string);
    printf("%s\n", cleaned_string->instring);
    // printf("%s\n", output->instring);
}

