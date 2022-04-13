## ************************************************************************
##  Makefile for arc_sz_example that demonstrates the use case of arc in 
##  conjunction with libPressio. 

##  PLEASE UPDATE THESE VARIABLES BEFORE COMPILING

##  COMPILER 
CC	= gcc
SPACK_PATH = /users/gfwilki/spack/opt/spack/linux-ubuntu20.04-zen2/gcc-9.4.0

## Libpressio flags
LIBPRESSIO_INCLUDE = $(SPACK_PATH)/libpressio-0.83.4-myllqo33wyo2slaqw7kdyk5spzlklvbh
LIBPRESSIO_SO_PATH = /users/gfwilki/spack/opt/spack/linux-ubuntu20.04-zen2/gcc-9.4.0/libpressio-0.83.4-myllqo33wyo2slaqw7kdyk5spzlklvbh/lib
## SZ flags
SZ_INCLUDE = $(SPACK_PATH)/sz-2.1.12.2-a5es2iuio2elattsmlttnwr4px5aoayt
SZ_SO_PATH = $(SZ_INCLUDE)/lib
## ZFP flags
ZFP_INCLUDE = $(SPACK_PATH)/zfp-0.5.5-5g3xvdcfctqxgmzfhmwflat46le2fvcp
ZFP_SO_PATH = $(ZFP_INCLUDE)/lib
## FPZIP flags
FPZIP_INCLUDE = $(SPACK_PATH)/fpzip-1.3.0-xkizpunmxhjqkmbthre627nzoyuszlhu
FPZIP_SO_PATH = $(FPZIP_INCLUDE)/lib
## ARC flags
ARC_INCLUDE = /users/gfwilki/ARC/include
ARC_SO_PATH = /users/gfwilki/ARC/lib64

## Compilation Includes
FLAGS = -I $(LIBPRESSIO_INCLUDE)/include/libpressio -I $(SZ_INCLUDE)/include/sz -I $(ZFP_INCLUDE)/include -I $(FPZIP_INCLUDE)/include -I $(ARC_INCLUDE) -L $(LIBPRESSIO_SO_PATH) -L $(SZ_SO_PATH) -L $(ZFP_SO_PATH) -L $(FPZIP_SO_PATH) -L $(ARC_SO_PATH) -llibpressio -lSZ -lzfp -lfpzip -larc -lm

## TARGETS
all: arc_pressio_example

arc_pressio_example:	arc_pressio_example.c 
	$(CC) -Wall -g -fopenmp -rdynamic -o arc_pressio_example arc_pressio_example.c $(FLAGS)
clean:
	rm arc_pressio_example

