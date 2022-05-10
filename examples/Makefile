## ************************************************************************
##  Makefile for arc_sz_example that demonstrates the use case of arc in 
##  conjunction with libPressio. 

##  PLEASE UPDATE THESE VARIABLES BEFORE COMPILING

##  COMPILER 
CC	= gcc
SPACK_PATH = /home/gfwilki/software/spackages/linux-centos8-k10/gcc-9.3.0

## Libpressio flags
LIBPRESSIO_INCLUDE = $(SPACK_PATH)/libpressio-0.78.0-3pnimzoovw5f6yzd4kx44bzeuk4fwjvv
LIBPRESSIO_SO_PATH = $(LIBPRESSIO_INCLUDE)/lib64
## SZ flags
SZ_INCLUDE = $(SPACK_PATH)/sz-2.1.12-rfqqyoc35hoy67jczspwyaj7tppshnvl
SZ_SO_PATH = $(SZ_INCLUDE)/lib64
## ZFP flags
ZFP_INCLUDE = $(SPACK_PATH)/zfp-0.5.5-hmy3isnhwljhzaxzmaxbdagowsgnh3gu
ZFP_SO_PATH = $(ZFP_INCLUDE)/lib64
## ARC flags
ARC_INCLUDE = /home/gfwilki/ARC/include
ARC_SO_PATH = /home/gfwilki/ARC/lib64

## Compilation Includes
FLAGS = -I $(LIBPRESSIO_INCLUDE)/include/libpressio -I $(SZ_INCLUDE)/include/sz -I $(ZFP_INCLUDE)/include -I $(ARC_INCLUDE) -L $(LIBPRESSIO_SO_PATH) -L $(SZ_SO_PATH) -L $(ZFP_SO_PATH) -L $(ARC_SO_PATH) -llibpressio -lSZ -lzfp -larc -lm

## TARGETS
all: arc_pressio_example #libpressio_example_sz libpressio_example_zfp

arc_pressio_example:	arc_pressio_example.c 
	$(CC) -Wall -g -fopenmp -rdynamic -o arc_pressio_example arc_pressio_example.c $(FLAGS)

#libpressio_example_sz: libpressio_example_sz.c
#	$(CC) -Wall -g -fopenmp -rdynamic -o libpressio_example_sz libpressio_example_sz.c $(FLAGS)

#libpressio_example_zfp: libpressio_example_zfp.c
#	$(CC) -Wall -g -fopenmp -rdynamic -o libpressio_example_zfp libpressio_example_zfp.c $(FLAGS)

clean:
	rm arc_pressio_example

