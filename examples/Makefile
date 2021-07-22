## ************************************************************************
##  Makefile for arc_sz_example that demonstrates the use case of arc in 
##  conjunction with libPressio. 

##  PLEASE UPDATE THESE VARIABLES BEFORE COMPILING

##  COMPILER 
CC	= gcc

## Libpressio flags
LIBPRESSIO_INCLUDE_ol7_sandybridge = /zfs/ftdice/spack/opt/spack/linux-ol7-sandybridge/gcc-8.2.0/libpressio-0.38.1-6bphbkmsnbfp4zxvgxn3a6al6cmmk4jl
LIBPRESSIO_SO_PATH_ol7_sandybridge = /zfs/ftdice/spack/opt/spack/linux-ol7-sandybridge/gcc-8.2.0/libpressio-0.38.1-6bphbkmsnbfp4zxvgxn3a6al6cmmk4jl/lib64
## SZ flags
SZ_INCLUDE_ol7_sandybridge = /zfs/ftdice/spack/opt/spack/linux-ol7-sandybridge/gcc-8.2.0/sz-2.1.8.1-hnjxctv4winw52xm2eeyucatpizag4cl
SZ_SO_PATH_ol7_sandybridge = /zfs/ftdice/spack/opt/spack/linux-ol7-sandybridge/gcc-8.2.0/sz-2.1.8.1-hnjxctv4winw52xm2eeyucatpizag4cl/lib64 
## ZFP flags
ZFP_INCLUDE_ol7_sandybridge = /zfs/ftdice/spack/opt/spack/linux-ol7-sandybridge/gcc-8.2.0/zfp-0.5.5-dycqslvmlpomptwjg52nt4zeoiqtcmun
ZFP_SO_PATH_ol7_sandybridge = /zfs/ftdice/spack/opt/spack/linux-ol7-sandybridge/gcc-8.2.0/zfp-0.5.5-dycqslvmlpomptwjg52nt4zeoiqtcmun/lib64
## FPZIP flags
FPZIP_INCLUDE_ol7_sandybridge = /zfs/ftdice/spack/opt/spack/linux-ol7-sandybridge/gcc-8.2.0/fpzip-1.3.0-hzhhfannwjcokyjokrs4q7oojxphwya6
FPZIP_SO_PATH_ol7_sandybridge = /zfs/ftdice/spack/opt/spack/linux-ol7-sandybridge/gcc-8.2.0/fpzip-1.3.0-hzhhfannwjcokyjokrs4q7oojxphwya6/lib64
## ARC flags
ARC_INCLUDE = /home/dakotaf/ARC/include
ARC_SO_PATH = /home/dakotaf/ARC/lib64

## Compilation Includes
FLAGS_ol7_sandybridge = -I $(LIBPRESSIO_INCLUDE_ol7_sandybridge)/include/libpressio -I $(SZ_INCLUDE_ol7_sandybridge)/include/sz -I $(ZFP_INCLUDE_ol7_sandybridge)/include -I $(FPZIP_INCLUDE_ol7_sandybridge)/include -I $(ARC_INCLUDE) -L $(LIBPRESSIO_SO_PATH_ol7_sandybridge) -L $(SZ_SO_PATH_ol7_sandybridge) -L $(ZFP_SO_PATH_ol7_sandybridge) -L $(FPZIP_SO_PATH_ol7_sandybridge) -L $(ARC_SO_PATH) -llibpressio -lSZ -lzfp -lfpzip -larc -lm

## TARGETS
all: arc_pressio_example

arc_pressio_example:	arc_pressio_example.c 
	$(CC) -Wall -g -rdynamic -o arc_pressio_example arc_pressio_example.c $(FLAGS_ol7_sandybridge)
clean:
	rm arc_pressio_example

