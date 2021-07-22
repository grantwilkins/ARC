## ************************************************************************
##  Makefile for arc_sz_example that demonstrates the use case of arc in 
##  conjunction with libPressio. 

##  PLEASE UPDATE THESE VARIABLES BEFORE COMPILING

##  COMPILER 
CC	= gcc

## ARC flags
ARC_INCLUDE = ../include
ARC_LIB_PATH = ../lib64

## TARGETS
all: arc_test interactive_arc_test interactive_arc_front_test #openmp_test

arc_test: arc_test.c
	$(CC) -Wall -g -o arc_test arc_test.c -I $(ARC_INCLUDE) -L $(ARC_LIB_PATH) -larc -lm -fopenmp

interactive_arc_test: interactive_arc_test.c
	$(CC) -Wall -g -o interactive_arc_test interactive_arc_test.c -I $(ARC_INCLUDE) -L $(ARC_LIB_PATH) -larc -lm -fopenmp

interactive_arc_front_test: interactive_arc_front_test.c
	$(CC) -Wall -g -o interactive_arc_front_test interactive_arc_front_test.c -I $(ARC_INCLUDE) -L $(ARC_LIB_PATH) -larc -lm -fopenmp

openmp_test: openmp_test.c
	$(CC) -Wall -g -o openmp_test openmp_test.c -I $(ARC_INCLUDE) -L $(ARC_LIB_PATH) -larc -lm -fopenmp

clean:
	rm arc_test
	rm interactive_arc_test
	rm interactive_arc_front_test
	#rm openmp_test

