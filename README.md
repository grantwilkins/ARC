# ARC

ARC is an automatic resiliency library designed to provide security to lossy compressed data or other uint8_t data arrays. To accomplish this, ARC first requires a small training period be performed. This training period has a complexity of O(log(n)) and as more threads are added the overhead increases reduces logarithmically. For example, this training period over 16 threads takes roughly 10 minutes. 

Upon completing this training period ARC is ready to use and only requires minor amounts of user input. Specifically, this input includes the data to encode, the original data size, the amount of memory overhead that it should not exceed, and a minimum bandwidth (MB/s) it should maintain during the encoding and decoding processes. Using this information, ARC is able to determine the best error-correcting code approach to use on the data and applies this to the data before returning the encoded results to the user. 

## ARC Usage

Before using ARC, some modifications to the source must be made:
In arc.c, 
```c
line 52: char *resource_location = "/home/dakotaf/ARC/src/res/";
line 54: char *cache_resource_location = "/home/dakotaf/ARC/src/res/cache/";
```
Both of the above lines must be changed to the correct full path of ARC's resource folder and training cache folder. Upon making these simple changes, ARC is ready to be compiled and used.


The following example demonstrates how ARC can be used in C:
```c
// Load nessecary libraries
#include <omp.h>
#include "arc.h"

int main(){
  // Byte style array
  uint8_t *data;
  // Size of data array
  uint32_t data_size = 500000;
  
  // Initialize ARC with the maximum number of threads it should use
  uint32_t max_threads = omp_get_max_threads();
  arc_init(max_threads);
  
  // Specify maximum amount of memory overhead to introduce
  // In this case, Encoded Data <= 750000
  double memory_constraint = 0.5;
  // If memory overhead is not a concern
  //double memory_constraint = ARC_ANY_SIZE;
  
  // Specify minimum bandwidth to maintain during encoding and decoding process
  // In this case, Encoding process bandwith >= 100 MB/s
  double throughput_constraint = 100;
  // If throughput is not a concern
  double throughput_constraint = ARC_ANY_BW;
  
  // Specify level of resiliency 
  // In this case, only use ECC capable of detecting or correcting sparse errors
  int num_choices = 2;
  int ecc_choices = [ARC_DET_SPARSE, ARC_COR_SPARSE];
  // If resiliency is not a concern
  int num_choices = 1;
  int ecc_choices = [ARC_ANY_ECC];
  
  // Set encoding output variables
  int err;
  uint8_t* arc_encoded;
  uint32_t arc_encoded_size;
  // Encode data with ARC
  err = arc_encode(data, data_size, memory_constraint, throughput_constraint, ecc_choices, num_choices, &arc_encoded, &arc_encoded_size);
  
  // Set decoding output variables
  uint8_t* arc_decoded;
  uint32_t arc_decoded_size;
  
  // Decode data with ARC
  err = arc_decode(arc_encoded, arc_encoded_size, &arc_decoded, &arc_decoded_size);
  
  // Save optimization data aquired through standard ARC use
  arc_save();
  // Securely close ARC
  arc_close();
}
```
