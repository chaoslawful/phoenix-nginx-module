all: crash libjit_dbg.so

crash: crash.c
	gcc -g3 -O0 $^ -o$@

libjit_dbg.so: jit_dbg.c
	gcc -fPIC -shared $^ -o $@

clean:
	rm -f crash libjit_dbg.so

.PHONY: all clean

