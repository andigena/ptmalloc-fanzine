nptl_stack: LDLIBS = -lpthread
dlopen: LDLIBS = -ldl
PROGRAMS = blind_chunk_unmapping dlopen exhaust nptl_stack main_stack
CFLAGS += -Wpedantic -std=gnu11 -g -fPIC -pie 

all: $(PROGRAMS)
clean:
	rm -f $(PROGRAMS)
