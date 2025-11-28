#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>

void test_segfault(void) {
    *(int*)NULL = 5;
}

void test_abort(void) {
    abort();
}

void test_timeout(void) {
    while(1);
}

void test_exit_bad(void) {
    exit(42);
}

void test_nice(void) {
    printf("Hello\n");
}

void test_div_zero(void) {
    int x = 5 / 0;
    (void)x;
}

int main(void) {
    printf("Test 1: "); sandbox(test_nice, 5, true);
    printf("Test 2: "); sandbox(test_segfault, 5, true);
    printf("Test 3: "); sandbox(test_abort, 5, true);
    printf("Test 4: "); sandbox(test_timeout, 2, true);
    printf("Test 5: "); sandbox(test_exit_bad, 5, true);
    printf("Test 6: "); sandbox(test_div_zero, 5, true);
    return 0;
}