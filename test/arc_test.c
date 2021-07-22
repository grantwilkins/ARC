#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <signal.h>
#include <math.h>
#include <float.h>
#include <sys/time.h>
#include <execinfo.h>
#include <getopt.h>
#include <inttypes.h>
#include "arc.h"
#include <omp.h>      //OpenMP

int main(int argc, char *argv[]){
    int i, j, k;
    double i_lf;

    // Initialize ARC
    uint32_t max_threads = omp_get_max_threads();
    printf("Threads %" PRIu32 "\n", max_threads);
    arc_init(max_threads);

    // Setup simulated compressed data stream
    uint32_t data_size = 500000;
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

    // Set up test variables
    int test_passes = 0;
    int total_tests = 0;
    // Set up ARC interface variables
    int err;
    uint32_t encoding_parameter_a;
    uint32_t encoding_parameter_b;
    uint32_t num_threads;



    // *********************************
    // TEST 1: Parity Functionality
    // *********************************
    printf("Testing Parity Functionality\n");
    for (i = 1; i <= 10; i++){
        // Vary parity block size
        encoding_parameter_a = i;

        // Vary number of OpenMP threads 
        for (j = 1; j <= max_threads; j++){
            num_threads = j;

            // Encode with Parity
            uint32_t encoded_parity_size;
            uint8_t* encoded_parity;
            err = arc_parity_encode(data, data_size, encoding_parameter_a, num_threads, &encoded_parity, &encoded_parity_size);

            // Decode with Parity
            uint32_t decoded_parity_size; 
            uint8_t* decoded_parity;
            err = arc_parity_decode(encoded_parity, encoded_parity_size, &decoded_parity, &decoded_parity_size);

            // Check for correctness
            int parity_pass = 1;
            if (decoded_parity_size == data_size){
                for (k = 0; k < data_size; k++){
                    // Compare all elements
                    if (decoded_parity[k] != data[k]){
                        // Set failed test if difference is found
                        parity_pass = 0;
                    }
                }
            } else {
                parity_pass = 0;
            }

            total_tests++;
            if (parity_pass == 1){
                test_passes++;
            }

            // Free temp variables
            free(encoded_parity);
            free(decoded_parity);
        }
    }
    if (test_passes == total_tests){
        printf("\nParity Tests Passed!\n\n");
    } else {
        printf("\nERROR: Parity Test Failed!\n\n");
    }
    // Reset Test Counter if Previous Tests Failed
    if (test_passes != total_tests){
        test_passes = 0;
        total_tests = 0;
    } 



    // *********************************
    // TEST 2: Hamming Functionality
    // *********************************
    printf("Testing Hamming Functionality\n");
    // Test Hamming over 1 block
    encoding_parameter_a = 1;
    for (j = 1; j <= max_threads; j++){
        num_threads = j;

        // Encode with Hamming
        uint32_t encoded_hamming_1_size;
        uint8_t * encoded_hamming_1;
        err = arc_hamming_encode(data, data_size, encoding_parameter_a, num_threads, &encoded_hamming_1, &encoded_hamming_1_size);

        // Decode with Hamming
        uint32_t decoded_hamming_1_size; 
        uint8_t * decoded_hamming_1;
        err = arc_hamming_decode(encoded_hamming_1, encoded_hamming_1_size, &decoded_hamming_1, &decoded_hamming_1_size);

        // Check for correctness
        int hamming_1_pass = 1;
        if (decoded_hamming_1_size == data_size){
            for (k = 0; k < data_size; k++){
                // Compare all elements
                if (decoded_hamming_1[k] != data[k]){
                    // Set failed test if difference is found
                    hamming_1_pass = 0;
                }
            }
        } else {
            hamming_1_pass = 0;
        }

        total_tests++;
        if (hamming_1_pass == 1){
            test_passes++;
        }

        // Free temp variables
        free(encoded_hamming_1);
        free(decoded_hamming_1);
    }
    // Test Hamming over 8 blocks
    encoding_parameter_a = 8;
    for (j = 1; j <= max_threads; j++){
        num_threads = j;

        // Encode with Hamming
        uint32_t encoded_hamming_8_size;
        uint8_t * encoded_hamming_8;
        err = arc_hamming_encode(data, data_size, encoding_parameter_a, num_threads, &encoded_hamming_8, &encoded_hamming_8_size);

        // Decode with Hamming
        uint32_t decoded_hamming_8_size; 
        uint8_t * decoded_hamming_8;
        err = arc_hamming_decode(encoded_hamming_8, encoded_hamming_8_size, &decoded_hamming_8, &decoded_hamming_8_size);

        // Check for correctness
        int hamming_8_pass = 1;
        if (decoded_hamming_8_size == data_size){
            for (k = 0; k < data_size; k++){
                // Compare all elements
                if (decoded_hamming_8[k] != data[k]){
                    // Set failed test if difference is found
                    hamming_8_pass = 0;
                }
            }
        } else {
            hamming_8_pass = 0;
        }

        total_tests++;
        if (hamming_8_pass == 1){
            test_passes++;
        }

        // Free temp variables
        free(encoded_hamming_8);
        free(decoded_hamming_8);
    }
    if (test_passes == total_tests){
        printf("\nHamming Tests Passed!\n\n");
    } else {
        printf("\nERROR: Hamming Test Failed!\n\n");
    }
    // Reset Test Counter if Previous Tests Failed
    if (test_passes != total_tests){
        test_passes = 0;
        total_tests = 0;
    } 



    // *********************************
    // TEST 3: SECDED Functionality
    // *********************************
    printf("Testing SECDED Functionality\n");
    // Test SECDED over 1 block
    encoding_parameter_a = 1;
    for (j = 1; j <= max_threads; j++){
        num_threads = j;

        // Encode with SECDED
        uint32_t encoded_secded_1_size;
        uint8_t * encoded_secded_1;
        err = arc_secded_encode(data, data_size, encoding_parameter_a, num_threads, &encoded_secded_1, &encoded_secded_1_size);

        // Decode with SECDED
        uint32_t decoded_secded_1_size; 
        uint8_t * decoded_secded_1;
        err = arc_secded_decode(encoded_secded_1, encoded_secded_1_size, &decoded_secded_1, &decoded_secded_1_size);

        // Check for correctness
        int secded_1_pass = 1;
        if (decoded_secded_1_size == data_size){
            for (k = 0; k < data_size; k++){
                // Compare all elements
                if (decoded_secded_1[k] != data[k]){
                    // Set failed test if difference is found
                    secded_1_pass = 0;
                }
            }
        } else {
            secded_1_pass = 0;
        }

        total_tests++;
        if (secded_1_pass == 1){
            test_passes++;
        }

        // Free temp variables
        free(encoded_secded_1);
        free(decoded_secded_1);
    }
    // Test secded over 8 blocks
    encoding_parameter_a = 8;
    for (j = 1; j <= max_threads; j++){
        num_threads = j;

        // Encode with SECDED
        uint32_t encoded_secded_8_size;
        uint8_t * encoded_secded_8;
        err = arc_secded_encode(data, data_size, encoding_parameter_a, num_threads, &encoded_secded_8, &encoded_secded_8_size);

        // Decode with SECDED
        uint32_t decoded_secded_8_size; 
        uint8_t * decoded_secded_8;
        err = arc_secded_decode(encoded_secded_8, encoded_secded_8_size, &decoded_secded_8, &decoded_secded_8_size);

        // Check for correctness
        int secded_8_pass = 1;
        if (decoded_secded_8_size == data_size){
            for (k = 0; k < data_size; k++){
                // Compare all elements
                if (decoded_secded_8[k] != data[k]){
                    // Set failed test if difference is found
                    secded_8_pass = 0;
                }
            }
        } else {
            secded_8_pass = 0;
        }

        total_tests++;
        if (secded_8_pass == 1){
            test_passes++;
        }

        // Free temp variables
        free(encoded_secded_8);
        free(decoded_secded_8);
    }
    if (test_passes == total_tests){
        printf("\nSECDED Tests Passed!\n\n");
    } else {
        printf("\nERROR: SECDED Test Failed!\n\n");
    }
    // Reset Test Counter if Previous Tests Failed
    if (test_passes != total_tests){
        test_passes = 0;
        total_tests = 0;
    } 



    // *********************************
    // TEST 4: Reed-Solomon Functionality
    // *********************************
    /**
    printf("Testing Reed-Solomon Functionality\n");
    encoding_parameter_a = 128; // Data Devices
    encoding_parameter_b = 128; // Code Devices
    for (i = 0; i < 5; i++){
        uint32_t cur_encoding_parameter_a = encoding_parameter_a + i*20;
        uint32_t cur_encoding_parameter_b = encoding_parameter_b - i*20;

        // Vary number of OpenMP threads 
        for (j = 1; j <= max_threads; j++){
            num_threads = j;

            // Encode with Reed Solomon
            uint32_t encoded_rs_size;
            uint8_t * encoded_rs;
            err = arc_reed_solomon_encode(data, data_size, cur_encoding_parameter_a, cur_encoding_parameter_b, num_threads, &encoded_rs, &encoded_rs_size);

            // Decode with Reed Solomon
            uint32_t decoded_rs_size; 
            uint8_t * decoded_rs;
            err = arc_reed_solomon_decode(encoded_rs, encoded_rs_size, &decoded_rs, &decoded_rs_size);

            // Check for correctness
            int rs_pass = 1;
            if (decoded_rs_size == data_size){
                for (k = 0; k < data_size; k++){
                    // Compare all elements
                    if (decoded_rs[k] != data[k]){
                        // Set failed test if difference is found
                        rs_pass = 0;
                    }
                }
            } else {
                rs_pass = 0;
            }

            total_tests++;
            if (rs_pass == 1){
                test_passes++;
            }

            // Free temp variables
            free(encoded_rs);
            free(decoded_rs);
        }
    }
    if (test_passes == total_tests){
        printf("\nReed-Solomon Tests Passed!\n\n");
    } else {
        printf("\nERROR: Reed-Solomon Test Failed!\n\n");
    }
    // Reset Test Counter if Previous Tests Failed
    if (test_passes != total_tests){
        test_passes = 0;
        total_tests = 0;
    } 
    **/



    // *********************************
    // TEST 5: ARC Memory Constraint Functionality
    // *********************************
    /**
    printf("Testing ARC's Memory Constraint Functionality\n");

    printf("Parity Memory Performance Evaluation\n");
    for (i_lf = 0.00001; i_lf < 1.25; i_lf+=0.05){
        double memory_constraint = i_lf;

        // Encode with ARC
        uint32_t arc_mem_encoded_size;
        uint8_t* arc_mem_encoded;
        int resiliency_constraint[] = {ARC_PARITY};
        err = arc_encode(data, data_size, memory_constraint, ARC_ANY_BW, resiliency_constraint, 1, &arc_mem_encoded, &arc_mem_encoded_size);

        // Decode with ARC
        uint32_t arc_mem_decoded_size;
        uint8_t* arc_mem_decoded;
        err = arc_decode(arc_mem_encoded, arc_mem_encoded_size, &arc_mem_decoded, &arc_mem_decoded_size);

        // Determine Memory Overhead
        double memory_overhead = ((double)arc_mem_encoded_size / (double)data_size) - 1;

        printf("Parity: Target %lf Actual: %lf\n", memory_constraint, memory_overhead);
    }
    printf("Hamming Memory Performance Evaluation\n");
    for (i_lf = 0.00001; i_lf < 1.25; i_lf+=0.05){
        double memory_constraint = i_lf;

        // Encode with ARC
        uint32_t arc_mem_encoded_size;
        uint8_t* arc_mem_encoded;
        int resiliency_constraint[] = {ARC_HAMMING};
        err = arc_encode(data, data_size, memory_constraint, ARC_ANY_BW, resiliency_constraint, 1, &arc_mem_encoded, &arc_mem_encoded_size);

        // Decode with ARC
        uint32_t arc_mem_decoded_size;
        uint8_t* arc_mem_decoded;
        err = arc_decode(arc_mem_encoded, arc_mem_encoded_size, &arc_mem_decoded, &arc_mem_decoded_size);

        // Determine Memory Overhead
        double memory_overhead = ((double)arc_mem_encoded_size / (double)data_size) - 1;

        printf("Hamming: Target %lf Actual: %lf\n", memory_constraint, memory_overhead);
    }
    printf("SECDED Memory Performance Evaluation\n");
    for (i_lf = 0.00001; i_lf < 1.25; i_lf+=0.05){
        double memory_constraint = i_lf;

        // Encode with ARC
        uint32_t arc_mem_encoded_size;
        uint8_t* arc_mem_encoded;
        int resiliency_constraint[] = {ARC_SECDED};
        err = arc_encode(data, data_size, memory_constraint, ARC_ANY_BW, resiliency_constraint, 1, &arc_mem_encoded, &arc_mem_encoded_size);

        // Decode with ARC
        uint32_t arc_mem_decoded_size;
        uint8_t* arc_mem_decoded;
        err = arc_decode(arc_mem_encoded, arc_mem_encoded_size, &arc_mem_decoded, &arc_mem_decoded_size);

        // Determine Memory Overhead
        double memory_overhead = ((double)arc_mem_encoded_size / (double)data_size) - 1;

        printf("SECDED: Target %lf Actual: %lf\n", memory_constraint, memory_overhead);
    }
    printf("RS Memory Performance Evaluation\n");
    for (i_lf = 0.00001; i_lf < 1.25; i_lf+=0.05){
        double memory_constraint = i_lf;

        // Encode with ARC
        uint32_t arc_mem_encoded_size;
        uint8_t* arc_mem_encoded;
        int resiliency_constraint[] = {ARC_RS};
        err = arc_encode(data, data_size, memory_constraint, ARC_ANY_BW, resiliency_constraint, 1, &arc_mem_encoded, &arc_mem_encoded_size);

        // Decode with ARC
        uint32_t arc_mem_decoded_size;
        uint8_t* arc_mem_decoded;
        err = arc_decode(arc_mem_encoded, arc_mem_encoded_size, &arc_mem_decoded, &arc_mem_decoded_size);

        // Determine Memory Overhead
        double memory_overhead = ((double)arc_mem_encoded_size / (double)data_size) - 1;

        printf("RS: Target %lf Actual: %lf\n", memory_constraint, memory_overhead);
    }
    **/

    /**
    for (i_lf = 0.1; i_lf < 2; i_lf+=0.1){
        double memory_constraint = i_lf;

        // Encode with ARC
        uint32_t arc_mem_encoded_size;
        uint8_t* arc_mem_encoded;
        int resiliency_constraint[] = {ARC_HAMMING, ARC_SECDED, ARC_RS};
        err = arc_encode(data, data_size, memory_constraint, ARC_ANY_BW, resiliency_constraint, 3, &arc_mem_encoded, &arc_mem_encoded_size);

        // Decode with ARC
        uint32_t arc_mem_decoded_size;
        uint8_t* arc_mem_decoded;
        err = arc_decode(arc_mem_encoded, arc_mem_encoded_size, &arc_mem_decoded, &arc_mem_decoded_size);

        // Check for correctness
        int arc_mem_pass = 1;
        if (arc_mem_decoded_size == data_size){
            for (k = 0; k < data_size; k++){
                // Compare all elements
                if (arc_mem_decoded[k] != data[k]){
                    // Set failed test if difference is found
                    arc_mem_pass = 0;
                }
            }
        } else {
            arc_mem_pass = 0;
        }

        // Check to ensure memory constraint was followed
        //double memory_overhead = ((double)arc_mem_encoded_size / (double)data_size) - 1;
        //if (memory_overhead > memory_constraint){
        //    arc_mem_pass = 0;
        //}

        total_tests++;
        if (arc_mem_pass == 1){
            test_passes++;
        }

        // Free temp variables
        free(arc_mem_encoded);
        free(arc_mem_decoded);
    }
    if (test_passes == total_tests){
        printf("\nARC Memory Constatint Test Passed!\n\n");
    } else {
        printf("\nERROR: ARC Memory Constraint Test Failed!\n\n");
    }
    // Reset Test Counter if Previous Tests Failed
    if (test_passes != total_tests){
        test_passes = 0;
        total_tests = 0;
    } 
    **/



    // *********************************
    // TEST 6: ARC Throughput Constraint Functionality
    // *********************************
    printf("Testing ARC's Throughput Constraint Functionality\n");
    struct timeval start, stop;
    double encode_time_taken;

    printf("Parity Throughput Performance Evaluation\n");
    for (i_lf = 0.00001; i_lf < 550; i_lf+=25){
        double throughput_constraint = i_lf;

        // Encode with ARC
        uint32_t arc_bw_encoded_size;
        uint8_t* arc_bw_encoded;
        int resiliency_constraint[] = {ARC_PARITY};
        gettimeofday(&start, NULL);
        err = arc_encode(data, data_size, ARC_ANY_SIZE, throughput_constraint, resiliency_constraint, 1, &arc_bw_encoded, &arc_bw_encoded_size);
        gettimeofday(&stop, NULL);
        encode_time_taken = (double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec);

        // Decode with ARC
        uint32_t arc_bw_decoded_size;
        uint8_t* arc_bw_decoded;
        err = arc_decode(arc_bw_encoded, arc_bw_encoded_size, &arc_bw_decoded, &arc_bw_decoded_size);

        // Determine Memory Overhead
        double throughput_overhead = (data_size / encode_time_taken)/1E6; // MB/s

        printf("Parity: Target %lf Actual: %lf\n", throughput_constraint, throughput_overhead);
    }
    printf("Hamming Throughput Performance Evaluation\n");
    for (i_lf = 0.00001; i_lf < 550; i_lf+=25){
        double throughput_constraint = i_lf;

        // Encode with ARC
        uint32_t arc_bw_encoded_size;
        uint8_t* arc_bw_encoded;
        int resiliency_constraint[] = {ARC_HAMMING};
        gettimeofday(&start, NULL);
        err = arc_encode(data, data_size, ARC_ANY_SIZE, throughput_constraint, resiliency_constraint, 1, &arc_bw_encoded, &arc_bw_encoded_size);
        gettimeofday(&stop, NULL);
        encode_time_taken = (double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec);

        // Decode with ARC
        uint32_t arc_bw_decoded_size;
        uint8_t* arc_bw_decoded;
        err = arc_decode(arc_bw_encoded, arc_bw_encoded_size, &arc_bw_decoded, &arc_bw_decoded_size);

        // Determine Memory Overhead
        double throughput_overhead = (data_size / encode_time_taken)/1E6; // MB/s

        printf("Hamming: Target %lf Actual: %lf\n", throughput_constraint, throughput_overhead);
    }
    printf("SECDED Throughput Performance Evaluation\n");
    for (i_lf = 0.00001; i_lf < 550; i_lf+=25){
        double throughput_constraint = i_lf;

        // Encode with ARC
        uint32_t arc_bw_encoded_size;
        uint8_t* arc_bw_encoded;
        int resiliency_constraint[] = {ARC_SECDED};
        gettimeofday(&start, NULL);
        err = arc_encode(data, data_size, ARC_ANY_SIZE, throughput_constraint, resiliency_constraint, 1, &arc_bw_encoded, &arc_bw_encoded_size);
        gettimeofday(&stop, NULL);
        encode_time_taken = (double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec);

        // Decode with ARC
        uint32_t arc_bw_decoded_size;
        uint8_t* arc_bw_decoded;
        err = arc_decode(arc_bw_encoded, arc_bw_encoded_size, &arc_bw_decoded, &arc_bw_decoded_size);

        // Determine Memory Overhead
        double throughput_overhead = (data_size / encode_time_taken)/1E6; // MB/s

        printf("SECDED: Target %lf Actual: %lf\n", throughput_constraint, throughput_overhead);
    }
    printf("RS Throughput Performance Evaluation\n");
    for (i_lf = 0.00001; i_lf < 550; i_lf+=25){
        double throughput_constraint = i_lf;

        // Encode with ARC
        uint32_t arc_bw_encoded_size;
        uint8_t* arc_bw_encoded;
        int resiliency_constraint[] = {ARC_RS};
        gettimeofday(&start, NULL);
        err = arc_encode(data, data_size, ARC_ANY_SIZE, throughput_constraint, resiliency_constraint, 1, &arc_bw_encoded, &arc_bw_encoded_size);
        gettimeofday(&stop, NULL);
        encode_time_taken = (double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec);

        // Decode with ARC
        uint32_t arc_bw_decoded_size;
        uint8_t* arc_bw_decoded;
        err = arc_decode(arc_bw_encoded, arc_bw_encoded_size, &arc_bw_decoded, &arc_bw_decoded_size);

        // Determine Memory Overhead
        double throughput_overhead = (data_size / encode_time_taken)/1E6; // MB/s

        printf("RS: Target %lf Actual: %lf\n", throughput_constraint, throughput_overhead);
    }




    /**
    for (i_lf = 0.000001; i_lf < 100; i_lf+=5){
        double throughput_constraint = i_lf;

        // Encode with ARC
        uint32_t arc_bw_encoded_size;
        uint8_t* arc_bw_encoded;
        int resiliency_constraint[] = {ARC_HAMMING, ARC_SECDED};
        gettimeofday(&start, NULL);
        err = arc_encode(data, data_size, ARC_ANY_SIZE, throughput_constraint, resiliency_constraint, 2, &arc_bw_encoded, &arc_bw_encoded_size);
        gettimeofday(&stop, NULL);
        encode_time_taken = (double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec);

        // Decode with ARC
        uint32_t arc_bw_decoded_size;
        uint8_t* arc_bw_decoded;
        err = arc_decode(arc_bw_encoded, arc_bw_encoded_size, &arc_bw_decoded, &arc_bw_decoded_size);

        // Check for correctness
        int arc_bw_pass = 1;
        if (arc_bw_decoded_size == data_size){
            for (k = 0; k < data_size; k++){
                // Compare all elements
                if (arc_bw_decoded[k] != data[k]){
                    // Set failed test if difference is found
                    arc_bw_pass = 0;
                }
            }
        } else {
            arc_bw_pass = 0;
        }

        // Check to ensure throughput constraint was followed
        //double throughput_overhead = (data_size / encode_time_taken)/1E6; // MB/s
        //if (throughput_overhead < throughput_constraint){
        //    arc_bw_pass = 0;
        //}

        total_tests++;
        if (arc_bw_pass == 1){
            test_passes++;
        }

        // Free temp variables
        free(arc_bw_encoded);
        free(arc_bw_decoded);
    }
    if (test_passes == total_tests){
        printf("\nARC Throughput Constatint Test Passed!\n\n");
    } else {
        printf("\nERROR: ARC Throughput Constraint Test Failed!\n\n");
    }
    // Reset Test Counter if Previous Tests Failed
    if (test_passes != total_tests){
        test_passes = 0;
        total_tests = 0;
    } 
    **/


    // *********************************
    // TEST 7: ARC Resiliency Constraint Functionality
    // *********************************
    printf("Testing ARC's Resiliency Constraint Functionality\n");
    for (i = 1; i < 20; i++){
        double memory_constraint = (double)i / 10;
        double throughput_constraint = 5 * (double)i;

        // Encode with ARC
        uint32_t arc_joint_encoded_size;
        uint8_t* arc_joint_encoded;
        int resiliency_constraint[] = {ARC_HAMMING};
        err = arc_encode(data, data_size, memory_constraint, throughput_constraint, resiliency_constraint, 1, &arc_joint_encoded, &arc_joint_encoded_size);

        // Decode with ARC
        uint32_t arc_joint_decoded_size;
        uint8_t* arc_joint_decoded;
        err = arc_decode(arc_joint_encoded, arc_joint_encoded_size, &arc_joint_decoded, &arc_joint_decoded_size);

        // Check for correctness
        int arc_joint_pass = 1;
        if (arc_joint_decoded_size == data_size){
            for (k = 0; k < data_size; k++){
                // Compare all elements
                if (arc_joint_decoded[k] != data[k]){
                    // Set failed test if difference is found
                    arc_joint_pass = 0;
                }
            }
        } else {
            arc_joint_pass = 0;
        }

        total_tests++;
        if (arc_joint_pass == 1){
            test_passes++;
        }

        // Free temp variables
        free(arc_joint_encoded);
        free(arc_joint_decoded);
    }
    if (test_passes == total_tests){
        printf("\nARC Resiliency Constatint Test Passed!\n\n");
    } else {
        printf("\nERROR: ARC Resiliency Constraint Test Failed!\n\n");
    }




    // *********************************
    // TEST 8: ARC Joint Constraint Functionality
    // *********************************
    printf("Testing ARC's Joint Constraint Functionality\n");
    for (i = 1; i < 20; i++){
        double memory_constraint = (double)i / 10;
        double throughput_constraint = 5 * (double)i;

        // Encode with ARC
        uint32_t arc_joint_encoded_size;
        uint8_t* arc_joint_encoded;
        int resiliency_constraint[] = {ARC_ANY_ECC};
        err = arc_encode(data, data_size, memory_constraint, throughput_constraint, resiliency_constraint, 1, &arc_joint_encoded, &arc_joint_encoded_size);

        // Decode with ARC
        uint32_t arc_joint_decoded_size;
        uint8_t* arc_joint_decoded;
        err = arc_decode(arc_joint_encoded, arc_joint_encoded_size, &arc_joint_decoded, &arc_joint_decoded_size);

        // Check for correctness
        int arc_joint_pass = 1;
        if (arc_joint_decoded_size == data_size){
            for (k = 0; k < data_size; k++){
                // Compare all elements
                if (arc_joint_decoded[k] != data[k]){
                    // Set failed test if difference is found
                    arc_joint_pass = 0;
                }
            }
        } else {
            arc_joint_pass = 0;
        }

        total_tests++;
        if (arc_joint_pass == 1){
            test_passes++;
        }

        // Free temp variables
        free(arc_joint_encoded);
        free(arc_joint_decoded);
    }
    if (test_passes == total_tests){
        printf("\nARC Joint Constatint Test Passed!\n\n");
    } else {
        printf("\nERROR: ARC Joint Constraint Test Failed!\n\n");
    }

    printf("\nALL TESTS HAVE FINISHED!\n");
    arc_save();
    arc_close();
    return 0;
}