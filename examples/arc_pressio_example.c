#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <math.h>
#include <float.h>
#include <sys/time.h>
#include <execinfo.h>
// ARC Libraries
#include "arc.h"
#include<omp.h>

// Libpressio Libraries
#include "libpressio.h"
#include "sz.h"


// FLIP THIS TO DO ERROR INJECTION
int INJECT = 0;

// Initial Data Pointer
float *DATA;
// Faulted Decompressed Data Pointer
float *RET_DATA;

/*
ARGV[1] := path/to/data
ARGV[2] := error bound
ARGV[3] := number of cores
ARGV[4] := ECC
ARGV[5] := flip in bit location
ARGV[6] := byte to flip location

example run on palmetto:
./arc_pressio_example /zfs/fthpc/common/sdrbench/nyx/baryon_density.dat 0.001 24 2 1 1
-- This would run ARC encode/decode with 1e-3 error bound, on 24 threads, using hamming ECC
*/

int main(int argc, char* argv[]) {
    // Declare dataset file and dimensions to compress with libPressio
    char *data_path = argv[1];
    double error_bound = atof(argv[2]);
    int number_threads = atoi(argv[3]);
    int ecc_selection = atoi(argv[4]);
    int flip_loc = atoi(argv[5]);
    int char_loc = atoi(argv[6]);
    //size_t dims[] = {1073726487};
    size_t dims[] = {512, 512, 512};
    //size_t dims[] = {500, 500, 100};

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

    // Set ECC
    int resiliency_constraint[1];
    if(ecc_selection == 1)
    {
        resiliency_constraint[0] = -1;
        printf("Parity,");
    }
    else if(ecc_selection == 2)
    {
        resiliency_constraint[0] = -2;
        printf("Hamming,");
    }
    else if(ecc_selection == 3)
    {
        resiliency_constraint[0] = -3;
        printf("SEC-DED,");
    }
    else if(ecc_selection == 4)
    { 
        resiliency_constraint[0] = -4;
        printf("Reed-Solomon,");
    }
    else
    {
        printf("UNSUPPORTED ECC INPUT\n");
        exit(-1);
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
    
    // Utilize ARC library
    arc_init(number_threads);

    // Get a pointer to uint8_t data from libPressio
    size_t compressed_size;
	uint8_t * data = (uint8_t *)pressio_data_ptr(compressed_data, &compressed_size);

    // Encode data using ARC 
    int ret;
    double encode_time_taken;
    double decode_time_taken;
    double memory_constraint = ARC_ANY_SIZE;
    double time_constraint = ARC_ANY_BW;
    uint8_t * arc_encoded_data;
    uint32_t arc_encoded_data_size;
    gettimeofday(&start, NULL);
    ret = arc_encode(data, (uint32_t)compressed_size, memory_constraint, time_constraint, resiliency_constraint, 1, &arc_encoded_data, &arc_encoded_data_size);
    gettimeofday(&stop, NULL);
    encode_time_taken = (double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec);
    if (ret == 0){
        printf("Error Found In ARC...\nExiting...\n");
    }

    if(INJECT)
    {
        arc_encoded_data[char_loc] = arc_encoded_data[char_loc] ^ ((((uint8_t) 0x1 << flip_loc)));
    }

    // Decode data using ARC
    uint8_t * arc_decoded_data;
    uint32_t arc_decoded_data_size;
    gettimeofday(&start, NULL);
    ret = arc_decode(arc_encoded_data, arc_encoded_data_size, &arc_decoded_data, &arc_decoded_data_size);
    gettimeofday(&stop, NULL);
    decode_time_taken = (double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec);
    if (ret == 0){
        printf("Error Found In ARC...\nExiting...\n");
    }
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
    struct pressio_data* pressio_decoded_data = pressio_data_new_move(pressio_data_dtype(compressed_data), (void*)arc_decoded_data, pressio_data_num_dimensions(compressed_data), decoded_dims, pressio_data_libc_free_fn, NULL);


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

    // Get compression ratio through pressio metrics
    // printf("Get compression Ratio\n");
    struct pressio_options* metric_results = pressio_compressor_get_metrics_results(compressor);
    double compression_ratio = 0;
    if (pressio_options_get_double(metric_results, "size:compression_ratio", &compression_ratio)) {
        printf("failed to get compression ratio\n");
        exit(1);
    }
    double encode_bandwidth = data_size / (1e6*(encode_time_taken));
    double decode_bandwidth = data_size / (1e6*(decode_time_taken));

    int number_of_incorrect = 0;
    double max_diff = 0;
    double rmse_sum = 0;
    float max_val = -1;
    float min_val = -1;

    for (i = 0; i < data_size/sizeof(float); i++)
    {
        float a = DATA[i];
        float b = RET_DATA[i];

        // RMSE Work
        double rmse_diff = a - b;
        rmse_diff = rmse_diff * rmse_diff;
        rmse_sum = rmse_sum + rmse_diff;
        
        //PSNR Work
        if(a > max_val || max_val == -1){
            max_val = a;
        }
        if(a < min_val || min_val == -1){
            min_val = a;
        }

        double diff = fabs(a - b);
        if(diff > error_bound){
            number_of_incorrect++;
        }
        if(diff > max_diff){
            max_diff = diff;
        }
    }

    //Calculate Root Mean Square Error 
    double rmse = rmse_sum / ((data_size/sizeof(float)) - 1);
    rmse = sqrt(rmse);

    //Check RMSE for bad values
    if (fpclassify(rmse) == FP_INFINITE) {
        rmse = FLT_MAX;
    } else if (fpclassify(rmse) == FP_NAN) {
        rmse = FLT_MAX;
    }

    //Calculate PSNR
    double psnr_control_value = 10000;
    double psnr = 0;
    if (rmse == 0){
        psnr = psnr_control_value;
    } else {
        psnr = 20 * log10((max_val - min_val) / rmse);  
    }   

    //Check PSNR for bad values
    if (fpclassify(psnr) == FP_INFINITE){
        psnr = psnr_control_value * -1;
    } else if (fpclassify(psnr) == FP_NAN) {
        psnr = psnr_control_value * -1;
    }
    
    //Check Maximum Difference for bad values
    if (fpclassify(max_diff) == FP_INFINITE){
        max_diff = FLT_MAX;
    } else if (fpclassify(max_diff) == FP_NAN) {
        max_diff = FLT_MAX;
    }

    /*
    //Print Metrics
    printf("Number of Incorrect: %d\n", number_of_incorrect);
    printf("Maximum Absolute Difference: %lf\n", max_diff);
    printf("Root Mean Squared Error: %lf\n", rmse);
    printf("PSNR: %lf\n", psnr);
    
    printf("Error Bound: %lf\n", error_bound);
    printf("Number of Threads: %d\n", number_threads);
    printf("Encode Time Taken: %lf\n", encode_time_taken);
    printf("Decode Time Taken: %lf\n", decode_time_taken);
    printf("Compress Time Taken: %lf\n", compress_time_taken);
    printf("Decompress Time Taken: %lf\n", decompress_time_taken);
    printf("Compression ratio: %lf\n", compression_ratio);
    printf("Encode Bandwidth: %lf\n", encode_bandwidth);
    printf("Decode Bandwidth: %lf\n", decode_bandwidth);
    */
    printf("%s,%d,%s,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%d,%lf,%lf,%lf\n",
        data_path, number_threads,error_bound_mode, error_bound,encode_time_taken,
        decode_time_taken, compress_time_taken,
        decompress_time_taken,compression_ratio,
        encode_bandwidth, decode_bandwidth, number_of_incorrect, max_diff, rmse, psnr);

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
    pressio_options_free(metric_results);
    pressio_compressor_release(compressor);
    pressio_release(library);

    // Free other data pointers
    // Note: we do not need to free the DATA pointer since pressio is handling that data
    // printf("Freeing Other Data\n");
    if (RET_DATA){
    	free(RET_DATA);
    }
    if (arc_encoded_data){
        free(arc_encoded_data);
    }

    return 0;
}
