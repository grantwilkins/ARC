# NYX = 1, HACC = 2, Hurricane = 3
DATA_TO_USE = 1
eccMethod = "hamming"

datapath = "/proj/compressenergy-PG0/"

errorBounds = [1e-3, 1e-4, 1e-5, 1e-6]

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
	output_data_file += eccMethod + "_nyx.txt"
elif (DATA_TO_USE == 2):
	datasets = haccDatasets
	output_data_file += "_hacc.txt"
else:
	datasets = hurricaneDatasets
	output_data_file += "_hurricane.txt"

numCores = 32


for dataset in datasets:
	fullPath = datapath + dataset
	for errBound in errorBounds:
		for coreCount in range(1, numCores + 1):
			print("./arc_pressio_example " + fullPath 
				+ " " + str(errBound) + " " + str(coreCount) 
				+ " >> " + output_data_file)