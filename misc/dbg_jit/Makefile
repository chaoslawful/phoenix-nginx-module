version=0.0.1
package=phoenix-utils-$(version)

all: crash libjit_dbg.so

crash: crash.c
	gcc -g3 -O0 $^ -o$@

libjit_dbg.so: jit_dbg.c
	gcc -g3 -O0 -fPIC -shared $^ -o $@

test: libjit_dbg.so crash
	LD_PRELOAD=./libjit_dbg.so ./crash

clean:
	rm -f crash libjit_dbg.so *.log

dist:
	rm -rf $(package)
	mkdir $(package)
	cp Makefile *.c $(package)/
	tar -cvf $(package).tar $(package)
	gzip -f --best $(package).tar

.PHONY: all clean

