PROGRAMS = realloc_noop realloc_noop_mremap_exact wild_memcpy int_realloc_grow_into_top int_realloc_encompass_valid_boundary
CFLAGS += -Wpedantic -std=gnu11 -g -fPIC -pie 

all: $(PROGRAMS)
clean:
	rm -f $(PROGRAMS)
