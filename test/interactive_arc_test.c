#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <signal.h>
#include <math.h>
#include <unistd.h>
#include <float.h>
#include <sys/time.h>
#include <execinfo.h>
#include <getopt.h>
#include <inttypes.h>
#include "arc.h"
#include <omp.h>      //OpenMP


int main(int argc, char *argv[]){
    int i, j, k;
    // Check Arguements
    if (argc != 5){
        printf("Error: Incorrect Number of Arguements\n");
    }
    // Read in arguements
    uint32_t max_threads = atoi(argv[1]);
    int method = atoi(argv[2]);
    uint32_t block_size = atoi(argv[3]);
    int iterations = atoi(argv[4]);

    
    // Setup simulated compressed data stream
    uint32_t data_size = 5000000;
    uint8_t * data = malloc(sizeof(uint8_t)*data_size);
    // Create random data array to simulate compressed data stream values
    srand(0);
    for (i = 0; i < data_size; i++){
        data[i] = (uint8_t)(rand() % 255);
    }
    // Print out first 10 values
    printf("Initial Data\n");
    for (i = 0; i < 10; i++){
        printf("%d: %" PRIu8 "\n", i, data[i]);
    }

    // Initialize ARC
    arc_init(max_threads);

    // Create arrays to hold encoding and decoding results
    double *encode_times = malloc(sizeof(double) * iterations);
    double *decode_times = malloc(sizeof(double) * iterations);

    PRINT = 0;

    // Encode data using threads up to the amount 
    for(i = 1; i <= max_threads; i++){
        // Repeat process multiple times and gather standard deviation as well
        for (j = 0; j <= iterations; j++){
            // Zero out current times array location
            encode_times[j] = 0;
            decode_times[j] = 0;

            // Encode data
            printf("Encoding Data\n");
            uint32_t arc_encoded_size;
            uint8_t* arc_encoded;
            struct timeval start, stop;
            double encode_time_taken;
            double decode_time_taken;
            gettimeofday(&start, NULL);
            int err;
            if (method == 1){
                err = arc_parity_encode(data, data_size, block_size, i, &arc_encoded, &arc_encoded_size);
            } else if (method == 2){
                err = arc_hamming_encode(data, data_size, block_size, i, &arc_encoded, &arc_encoded_size);
            } else if (method == 3){
                err = arc_secded_encode(data, data_size, block_size, i, &arc_encoded, &arc_encoded_size);
            } else if (method == 4){
                err = arc_reed_solomon_encode(data, data_size, block_size, (256-block_size), i, &arc_encoded, &arc_encoded_size);
            }
            gettimeofday(&stop, NULL);
            encode_time_taken = (double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec);
            double encode_overhead = (data_size / encode_time_taken)/1E6; // MB/s
            encode_times[j] = encode_overhead;

            /**
            if (method == 1){
                printf("Parity Encoding on %d threads Completed in %lf Seconds!\n", i, encode_time_taken);
            } else if (method == 2){
                printf("Hamming Encoding on %d threads Completed in %lf Seconds!\n", i, encode_time_taken);
            } else if (method == 3){
                printf("SECDED Encoding on %d threads Completed in %lf Seconds!\n", i, encode_time_taken);
            } else if (method == 4){
                printf("RS Encoding on %d threads Completed in %lf Seconds!\n", i, encode_time_taken);
            }
            **/

            // Inject Fault 
            //arc_encoded[data_size/2] = arc_encoded[data_size/2] ^ 0b00000001;
            //for (k = 18; k < 100000; k=k+2){
            //    arc_encoded[k] = arc_encoded[k] ^ 0b00000001;
            //}
            for (k = 0; k < 50; k=k+2){
                int index = 26 + ((256)*8 + (256))*k;
                arc_encoded[index] = arc_encoded[index] ^ 0b00000001;
            }

            // Decode data
            printf("Decoding Data\n");
            uint32_t arc_decoded_size;
            uint8_t* arc_decoded;
            gettimeofday(&start, NULL);
            err = arc_decode(arc_encoded, arc_encoded_size, &arc_decoded, &arc_decoded_size);
            gettimeofday(&stop, NULL);
            decode_time_taken = (double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec);
            double decode_overhead = (data_size / decode_time_taken)/1E6; // MB/s
            decode_times[j] = decode_overhead;
            /**
            if (method == 1){
                printf("Parity Decoding on %d threads Completed in %lf Seconds!\n", i, decode_time_taken);
            } else if (method == 2){
                printf("Hamming Decoding on %d threads Completed in %lf Seconds!\n", i, decode_time_taken);
            } else if (method == 3){
                printf("SECDED Decoding on %d threads Completed in %lf Seconds!\n", i, decode_time_taken);
            } else if (method == 4){
                printf("RS Decoding on %d threads Completed in %lf Seconds!\n", i, decode_time_taken);
            }
            **/

            // Free 
            free(arc_encoded);
            free(arc_decoded);
        }
        // Calculate standard deviation of multiple iteration trials for this number of threads
        
        // Determine the average
        double encode_sum = 0;
        double decode_sum = 0;
        for (j = 0; j <= iterations; j++){
            encode_sum = encode_sum + encode_times[j];
            decode_sum = decode_sum + decode_times[j];
        }
        double encode_avg = encode_sum / iterations;
        double decode_avg = decode_sum / iterations;

        // Sum squared differences of all 
        double encode_difference_sum = 0;
        double decode_difference_sum = 0;
        for (j = 0; j <= iterations; j++){
            // Take difference
            double encode_difference = encode_avg - encode_times[j];
            double decode_difference = decode_avg - decode_times[j];
            // Square difference
            encode_difference = encode_difference * encode_difference;
            decode_difference = decode_difference * decode_difference;
            // Add to sum
            encode_difference_sum = encode_difference_sum + encode_difference;
            decode_difference_sum = decode_difference_sum + decode_difference;
        }
        
        // Get standard deviation
        double encode_standard_deviation = sqrt(encode_difference_sum / iterations);
        double decode_standard_deviation = sqrt(decode_difference_sum / iterations);

        printf("Encoding Average on %d threads: %lf\n", i, encode_avg);
        printf("Encoding Standard Deviation on %d threads: %lf\n", i, encode_standard_deviation);
        printf("Decoding Average on %d threads: %lf\n", i, decode_avg);
        printf("Decoding Standard Deviation on %d threads: %lf\n", i, decode_standard_deviation);

        sleep(12);
    }

    // Close ARC Correctly
    arc_save();
    arc_close();
}