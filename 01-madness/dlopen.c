#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <sys/mman.h>

void *dlo(const char* name) 
{
    void *handle = dlopen(name, RTLD_NOW);
    if (!handle) {
        fprintf(stderr, "%s\n", dlerror());
        exit(EXIT_FAILURE);
    }

    return handle;
}

int main(int argc, const char *argv[]) {
    const size_t size = 8*1024*1024;
    void *mm = 0;
    void *mmm = 0;
    void **handles = 0;

    assert(argc > 1 && argc < 10);
    handles = (void**) malloc(argc * sizeof(void*));
    
    if ((mm = mmap(0, size, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0)) == MAP_FAILED) {
        perror("mmap failed\n");
        exit(1);
    }

    if ((mmm = mmap(0, size, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0)) == MAP_FAILED) {
        perror("mmap failed\n");
        exit(1);
    }


    printf("mapped mm: %p-%p\n", mm, (char*)mm+size);
    printf("emulating the arbitrary unmap primitive by unmapping mm at %p\n", mm);  
    if (munmap(mm, size) != 0) {
        perror("munmap failed");
        exit(1);
    }

    
    printf("loading %x dynamic libraries\n", argc-1);
    for (int i = 1; i < argc; i++) {
        handles[i-1] = dlo(argv[i]);        
        // peek into the "opaque" return value of dlopen to get the loaded lib base
        // hackish and non-portable for sure?
        printf("\tloaded %s at %p\n", argv[i], (void*)*(uintptr_t *)handles[i-1]);
    }

    return 0;
}
