#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <math.h>
#include <float.h>
#include <sys/time.h>
#include <execinfo.h>

#include<omp.h>

// Libpressio Libraries
#include "libpressio.h"
#include "sz.h"

// Initial Data Pointer
float *DATA;
// Faulted Decompressed Data Pointer
float *RET_DATA;

/*
ARGV[1] := path/to/data
ARGV[2] := error bound
ARGV[3] := number of cores
ARGV[4] := ECC
*/

int main(int argc, char* argv[]) {
    // Declare dataset file and dimensions to compress with libPressio
    char *data_path = argv[1];
    //size_t dims[] = {1073726487};
    //size_t dims[] = {512, 512, 512};
    size_t dims[] = {500, 500, 100};

    int ndims = sizeof(dims)/sizeof(dims[0]);
    struct timeval start, stop;
    // Read in data from file
    // printf("Reading Data From File\n");
    size_t data_size = sizeof(float);
    for(int k = 0; k < ndims; k++)
        data_size *= dims[k];
  
    FILE *fp;
    DATA= malloc(data_size);
  fp = fopen(data_path,"rb");
  if (fp == NULL){
    perror("ERROR: ");
    exit(-1);
  } else {
    fread(DATA, 1, data_size, fp);
    fclose(fp);
  }

    // Initialize libPressio
    // printf("Initializing Pressio\n");
    struct pressio* library = pressio_instance();
    struct pressio_compressor* compressor = pressio_get_compressor(library, "sz");

    // Set up libPressio metrics
    // printf("Setting Metrics\n");
    const char* metrics[] = { "size" };
    struct pressio_metrics* metrics_plugin = pressio_new_metrics(library, metrics, 1);
    pressio_compressor_set_metrics(compressor, metrics_plugin);

    // Configure compressor
    // printf("Setting SZ Parameters\n");
    struct pressio_options* sz_options = pressio_compressor_get_options(compressor);
    // Set error bounding mode
    char error_bound_mode[] = "ABS";
    pressio_options_set_integer(sz_options, "sz:error_bound_mode", ABS);
    // pressio_options_set_integer(sz_options, "sz:error_bound_mode", PW_REL);
    // pressio_options_set_integer(sz_options, "sz:error_bound_mode", PSNR);
    // Set error bound
    double error_bound = atof(argv[2]);
    pressio_options_set_double(sz_options, "sz:abs_err_bound", error_bound);

    // Check and set options
    if (pressio_compressor_check_options(compressor, sz_options)) {
        printf("%s\n", pressio_compressor_error_msg(compressor));
        exit(pressio_compressor_error_code(compressor));
    }
    if (pressio_compressor_set_options(compressor, sz_options)) {
        printf("%s\n", pressio_compressor_error_msg(compressor));
        exit(pressio_compressor_error_code(compressor));
    }

    // Create libPressio data structures
    // printf("Creating Data Structures\n");
    struct pressio_data* input_data = pressio_data_new_move(pressio_float_dtype, DATA, ndims, dims, pressio_data_libc_free_fn, NULL);
    // creates an output dataset pointer
    struct pressio_data* compressed_data = pressio_data_new_empty(pressio_byte_dtype, 0, NULL);
    // configure the decompressed output area
    struct pressio_data* decompressed_data = pressio_data_new_empty(pressio_float_dtype, ndims, dims);

    double compress_time_taken = 0.0; 
    // Compress data 
    // printf("Compressing Data\n");
    gettimeofday(&start, NULL);
    if (pressio_compressor_compress(compressor, input_data, compressed_data)) 
    {
        printf("%s\n", pressio_compressor_error_msg(compressor));
        exit(pressio_compressor_error_code(compressor));
    }
    gettimeofday(&stop, NULL);
    compress_time_taken = (double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec);
    

    // Get a pointer to uint8_t data from libPressio
    size_t compressed_size;
    uint8_t * data = (uint8_t *)pressio_data_ptr(compressed_data, &compressed_size);

    // printf("Memory Constraint: %lf\n", memory_constraint);
    // printf("Time Constraint: %lf\n", time_constraint);
    // printf("Original Data Size: %"PRIu32"\n", (uint32_t)compressed_size);
    // printf("Encoded Data Size: %"PRIu32"\n", arc_encoded_data_size);

    // Create a new pressio struct to manage decoded data 

    // Get dims information from original compressed data
    size_t * decoded_dims = calloc(pressio_data_num_dimensions(compressed_data), sizeof(size_t));
    size_t i;
    for (i = 0; i <= pressio_data_num_dimensions(compressed_data); ++i){
        decoded_dims[i] = pressio_data_get_dimension(compressed_data, i);
    }
    struct pressio_data* pressio_decoded_data = pressio_data_new_move(pressio_data_dtype(compressed_data), (void*)compressed_data, pressio_data_num_dimensions(compressed_data), decoded_dims, pressio_data_libc_free_fn, NULL);


    double decompress_time_taken = 0.0; 
    // Decompress decoded data 
    // printf("Decompressing Data\n");
    gettimeofday(&start, NULL);
    if (pressio_compressor_decompress(compressor, pressio_decoded_data, decompressed_data)) {
        printf("%s\n", pressio_compressor_error_msg(compressor));
        exit(pressio_compressor_error_code(compressor));
    }
    gettimeofday(&stop, NULL);
    decompress_time_taken = (double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec);
    
    // Copy decompressed data to result array
    size_t out_bytes;
    RET_DATA = (float *)pressio_data_copy(decompressed_data, &out_bytes);

    fp = fopen("decomp_data.bin","rb");

    fread(RET_DATA, 1, data_size, fp);
    fclose(fp);
    // Compare first elements of both original and returned data
    // printf("First Element of Original Data: %f\n", DATA[0]);
    // printf("First Element of Decompressed Data: %f\n", RET_DATA[0]);

    // Free data pointers
    // printf("Freeing Data Pointers\n");
    pressio_data_free(decompressed_data);
    pressio_data_free(compressed_data);
    pressio_data_free(input_data);
    pressio_data_free(pressio_decoded_data);

    // Free options and the library
    // printf("Freeing Options and Library\n");
    pressio_options_free(sz_options);
    pressio_compressor_release(compressor);
    pressio_release(library);

    // Free other data pointers
    // Note: we do not need to free the DATA pointer since pressio is handling that data
    // printf("Freeing Other Data\n");
    if (RET_DATA){
      free(RET_DATA);
    }

    return 0;
}
