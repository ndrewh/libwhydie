#include <stdio.h>
#include <stdlib.h>

__attribute__((noinline))
static void crash() {
    *((char*)1) = 1;
}
int main() {
    crash();
}
