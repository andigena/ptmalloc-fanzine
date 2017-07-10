#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


size_t *chunksizep(void *mem) {
    return (size_t *)(((char *)mem) - sizeof(size_t));
}


int main(int argc, const char* argv[]) {
    printf("This example showcases the possiblity to create overlapping chunks \
            via the tcaching code in _int_free\n");
    void *mem = malloc(0x48);
    void *sentry = malloc(0x18);
    printf("Allocated victim chunk with requested size 0x48 at %p\n", mem);
    printf("Allocated sentry element after victim (not strictly necessary): %p\n", 
            sentry);

    printf("Emulating corruption of the victim's size to 0x110\n");
    *chunksizep(mem) = 0x110;
    free(mem);
    printf("Freed victim chunk to put it in a different tcache bin\n");

    void *mem2 = malloc(0x100);
    printf("Requested a chunk of 0x100 bytes, it is at: %p\n", mem2);

    return 0;
}

