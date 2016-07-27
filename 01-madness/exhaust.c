#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/mman.h>

// fills the address space with 32mb allocations to observe the behavior of mmap,
// especially the ordering of the returned addresses.

int main(int argc, const char* argv[], const char* envp[]) {
    malloc(1);
    uintptr_t ret = 0;
    uintptr_t last = 0;
    bool downwards = true;
    size_t alloc_size = 32*1024*1024;

    last = (uintptr_t) mmap(0, alloc_size, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
    printf("first mapping: %p\n", (void*)last);

    for (size_t i = 0; ; i++) {
        ret = (uintptr_t) mmap(0, alloc_size, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
        if (ret == MAP_FAILED)
            break;

        if ((ret + (downwards ? alloc_size : -alloc_size)) != last) {
            printf("non-contiguous mmap: %p after %p\n", (void *)ret, (void *)last);
        }

        if (ret > last && downwards) {
            downwards = false;
            printf("direction changed to upwards: %p after %p\n", (void *)ret, (void *)last);
        } else if (ret < last && !downwards) {
            downwards = true;
            printf("direction changed to downwards: %p after %p\n", (void *)ret, (void *)last);
        }

        last = ret;
    }

    printf("last address returned: %p\n", (void*)last);
    return 0;
}
