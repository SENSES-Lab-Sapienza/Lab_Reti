#include <stdio.h>
#include <string.h>

#define MAX_NAME_LENGTH 50

// Returns the n-th fibonacci number
// fibonacci(0) = fibonacci(1) = 1
int fibonacci( int n) {
    if (n < 0) {
        perror("Not negative number please!");
    }
    if (n < 2) {
        return 1;
    }
    return fibonacci(n-1) + fibonacci(n-2);
}

int fibonacci_optimized( int n) {
    if (n < 0) {
        perror("Not negative number please!");
    }
    if (n < 2) {
        return 1;
    }
    int v1, v2 = 1;
    for (int i = 2; i <= n; n++ ) {
        int last = v1 + v2;
        v1 = v2;
        v2 = last;
    }
    return v2;
}

// Compute the square of a number
int square(int n) {
    return n*n;
}

int mainMenu() {
    int choice;
    printf("What do you want to do today?\n");
    printf("0: fibonacci\n");
    printf("1: square of a number\n");
    printf("2: fibonacci optmized\n");
    scanf("%d", &choice);
    return choice;
}

int insertNumber() {
    char buffer[MAX_NAME_LENGTH];
    int n;
    fgets(buffer, MAX_NAME_LENGTH, stdin);
    sscanf(buffer, "%d", &n);
    return n;
}

int main() {
    int (*functions[])(int) = { fibonacci, square, fibonacci_optimized };
    int choice = mainMenu();
    int n = insertNumber();
    printf("Inserted number n: %d\n", n);
    int f = functions[choice](n);
    printf("Fibonacci of %d is %d\n", n, f);
    return 0;
}