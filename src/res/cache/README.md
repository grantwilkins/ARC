# ARC Training Cache

For ARC to be able to accurately determine which error-correcting code approach to use, ARC uses stored training data that is collected during the first run of the arc_init function. These results are stored within this folder using the following file format:
```
{#_of_threads}_information_cache.csv
```
The contents of these files are stuctured as follows:
```
ecc_algorithm,ecc_parameter_a,ecc_parameter_b,num_threads,memory_overhead,throughput_overhead
```
Such that,
```
ecc_algorithm       : 1 - 4 (1=Parity,2=Hamming,3=SECDED,4=RS)
ecc_parameter_a     : The first parameter for the desired ecc algorithm. Ususally in the form of block size
ecc_parameter_b     : The second parameter for the desired ecc algorithm. Used primarily with RS encoding and holds the number of code devices in this case.
num_threads         : number of threads used, same as #_of_threads in file name
memory_overhead     : The amount of memory overhead introduced from using this ECC configuration.
throughput_overhead : The average bandwidth achieved when using this ECC configuration.
```
By seperating the training data based on the number of threads used, ARC is able to reuse the data obtained when training on a lower number of threads when using a higher number of threads at a later instance. For example, if training was done initially with a maximum of 4 threads but later was changed to a maximum of 8 threads, the training results from threads 1-4 would be reused and only training on threads 5-8 would be done, therefore saving training time.

### Note:

In order to fully retrain on a system, all results within this folder must be deleted.
