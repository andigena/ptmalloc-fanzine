#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

int main() {
        void* p1 = malloc(0x40);
        free(p1);
        free(p1);
        printf("Next allocated memory will be same: %p %p\n", malloc(0x40), malloc(0x40));
}
