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

int main() {
    String* news = create_string();
    char in[] = "test";
    news = add_string(news, in);
    while (news->length < 50) {
        add_char(news, 'c');
    }
    while (news->length > 0) {
        pop_char(news);
    }
    delete_string(news);
}

