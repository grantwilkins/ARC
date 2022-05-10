import math
# NYX = 1, HACC = 2, Hurricane = 3
DATA_TO_USE = 1

datapath = "/zfs/fthpc/common/sdrbench/"

print("export LD_LIBRARY_PATH=/home/gfwilki/software/spackages/linux-centos8-k10/gcc-9.3.0/libpressio-0.78.0-3pnimzoovw5f6yzd4kx44bzeuk4fwjvv/lib64/:/home/gfwilki/software/spackages/linux-centos8-k10/gcc-9.3.0/sz-2.1.12-rfqqyoc35hoy67jczspwyaj7tppshnvl/lib64:/home/gfwilki/software/spackages/linux-centos8-k10/gcc-9.3.0/zfp-0.5.5-hmy3isnhwljhzaxzmaxbdagowsgnh3gu/lib64")

errorBounds = [1e-3, 1e-4, 1e-5, 1e-6]

cores_to_test = [1,2,4,8,16,32,40]

nyxDatasets = ["nyx/baryon_density.dat", 
	"nyx/temperature.dat",  
	"nyx/velocity_y.dat", 
	"nyx/dark_matter_density.dat", 
	"nyx/velocity_x.dat", 
	"nyx/velocity_z.dat"]

haccDatasets = ["hacc/vx.f32",
  	"hacc/vy.f32",
  	"hacc/vz.f32",
  	"hacc/xx.f32",
  	"hacc/yy.f32",
  	"hacc/zz.f32"]

hurricaneDatasets = ["hurricane/CLOUDf48.bin",
	"hurricane/PRECIPf48.bin",
	"hurricane/QCLOUDf48.bin",
	"hurricane/TCf48.bin",
	"hurricane/Uf48.bin",
	"hurricane/Wf48.bin"]

datasets = []
output_data_file = "results"

if (DATA_TO_USE == 1):
	datasets = nyxDatasets
	output_data_file += "_nyx_arc_bw.txt"
elif (DATA_TO_USE == 2):
	datasets = haccDatasets
	output_data_file += "_hacc_arc_bw.txt"
else:
	datasets = hurricaneDatasets
	output_data_file += "_hurricane_arc_bw.txt"



for dataset in datasets:
	fullPath = datapath + dataset
	for errBound in errorBounds:
		for coreCount in cores_to_test:
			for ecc in range(1, 5):
				print("./arc_pressio_example " + fullPath 
					+ " " + str(errBound) + " " + str(coreCount) 
					+ " " + str(ecc) + " 0 0"
					+ " >> " + output_data_file)
