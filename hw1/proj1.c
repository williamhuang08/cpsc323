#include "proj1.h"

String* create_string() {
    String* newstring = malloc(sizeof(String));
    newstring->length = 0
    newstring->capacity = 20
    newstring->instring = malloc(sizeof(char) * newstring->capacity)
} 

String* add_char(String* string, char c) {
    if (string->length + 1 > capacity) {
        string->capacity *= 2
        string->instring = realloc(string->instring, sizeof(char) * string->capacity)
    }
    newstring->instring[string->length] = c
    string->length += 1
}