## ************************************************************************
##  Makefile for the ARC: Automatic Reliability for Compression library.

## COMPILER 
CC	= gcc
CFLAGS = -O2

## TARGETS
all: arc_lib.o

arc.h: include/arc.h
	rm -f src/arc.h ; cp include/arc.h src/. ; chmod 0444 src/arc.h

galois.h: include/galois.h
	rm -f src/galois.h ; cp include/galois.h src/. ; chmod 0444 src/galois.h

jerasure.h: include/jerasure.h
	rm -f src/jerasure.h ; cp include/jerasure.h src/. ; chmod 0444 src/jerasure.h

reed_sol.h: include/reed_sol.h
	rm -f src/reed_sol.h ; cp include/reed_sol.h src/. ; chmod 0444 src/reed_sol.h

galois.o: galois.h 
	$(CC) -c src/galois.c 

jerasure.o: jerasure.h galois.h
	$(CC) -c src/jerasure.c 

reed_sol.o: reed_sol.h jerasure.h galois.h
	$(CC) -c src/reed_sol.c

arc.o: arc.h jerasure.h reed_sol.h galois.h
	$(CC) -c src/arc.c -fopenmp

arc_lib.o: arc.o galois.o jerasure.o reed_sol.o
	ar -rc lib64/libarc.a arc.o galois.o jerasure.o reed_sol.o
	rm arc.o galois.o jerasure.o reed_sol.o

clean:
	rm -f src/arc.h src/galois.h src/jerasure.h src/reed_sol.h
	rm lib64/libarc.a