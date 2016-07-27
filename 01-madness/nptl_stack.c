#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>

pthread_t thrd;

// simply return an address from the thread stack
void *thrd_f(void *p) {
    size_t local = 1;
    pthread_exit(&local);
}


int main() {
    const size_t size = 8*1024*1024;
    void *mm = NULL;
    void *thrd_local = NULL;

    if ((mm = mmap(0, size, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0)) == MAP_FAILED) {
        perror("mmap failed\n");
        exit(1);
    }

    printf("mapped mm: %p-%p\n", mm, (char*)mm+size);
    printf("unmapping mm then starting thread\n");
    if (munmap(mm, size) != 0) {
        perror("munmap failed");
        exit(1);
    }

    if (pthread_create(&thrd, NULL, thrd_f, 0)) {
        perror("Error creating thread\n");
        exit(1);
    }
    
    pthread_join(thrd, &thrd_local);
    printf("local variable at %p, %s\n", thrd_local, 
            ((char*)thrd_local < (char*)mm + size && 
             (char*)thrd_local >= (char*)mm) ? 
            "inside target region" : "outside target region");
    return 0;
}
