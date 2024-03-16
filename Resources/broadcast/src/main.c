#include <stdio.h>
#include <stdarg.h>

void log(int id, char * format, ...) {
    va_list args;
    va_start(args, format);
    printf("%d: ", id);
    printf(format, args);
    va_end(args);
}

int main(int argc, char *argv[])
{
    setvbuf(stdout, NULL, _IONBF, 0);

    if (argc < 3) {
        perror("No neighbor for this node");
        return 1;
    }

    int id = atoi(argv[1]);

    int * neighbors = calloc(sizeof(int), argc - 2);
    for (int i = 0; i < argc - 2; i ++) {
        neighbors[i] = atoi(argv[i + 2]);
    }

    log(id, "begin");

    // Implementation

    free(neighbors);

}