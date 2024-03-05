#include <stdio.h>
#include <string.h>

#define MAX_BUFFER 50

// remove in "str" all occurrences of "sub"
// returns a reference to the final string
// all the strings terminate with /0 
char* remove_substring(char * str, const char * sub) {
    while (1) {
        // printf("Current: %s", str);
        char * index = strstr(str, sub);
        if (index == NULL) {
            return str;
        }
        int index_i = index - str;
        int stop = index_i + strlen(sub);
        for (int i = stop; i < strlen(str) + 1; i ++) {
            str[i - strlen(sub) - 1] = str[i];
        }
    }
}

int main(int argc, char* argv[]) {
    char str[MAX_BUFFER];
    char sub[MAX_BUFFER];
    fgets(str, MAX_BUFFER, stdin);
    printf("Word to parse: %s\n", str);
    fgets(sub, MAX_BUFFER, stdin);
    printf("Word to remove: %s\n", sub);

    printf("Final result: %s\n", remove_substring(str, sub));

    return 0;
}