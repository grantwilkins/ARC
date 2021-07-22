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
    int i, j;
    // Check Arguements
    if (argc != 5){
        printf("Error: Incorrect Number of Arguements\n");
    }
    // Read in arguements
    uint32_t max_threads = atoi(argv[1]);
    char *methods = argv[2];
    double memory_constraint = atof(argv[3]);
    double throughput_constraint = atof(argv[4]);

    // Create ecc methods list from input
    int ecc_choices_temp[5] = {0};
    char *pt;
    int num_choices = 0;
    pt = strtok(methods, " ");
    while (pt != NULL) {
        ecc_choices_temp[num_choices] = atoi(pt);
        num_choices++;
        pt = strtok (NULL, " ");
    }
    // Pull into true list
    printf("ECC Choices: ");
    int *ecc_choices = malloc(num_choices * sizeof(int));
    for (i = 0; i < num_choices; i++){
		ecc_choices[i] = ecc_choices_temp[i];
        printf("%i ", ecc_choices[i]);
	}
    printf("\n");


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


    // Encode data
    printf("Encoding Data\n");
    uint32_t arc_encoded_size;
    uint8_t* arc_encoded;
    struct timeval start, stop;
    double encode_time_taken;
    double decode_time_taken;
    int err;
    gettimeofday(&start, NULL);
    err = arc_encode(data, data_size, memory_constraint, throughput_constraint, ecc_choices, num_choices, &arc_encoded, &arc_encoded_size);
    gettimeofday(&stop, NULL);
    encode_time_taken = (double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec);
    printf("Encoding on %d threads Completed in %lf Seconds!\n", i, encode_time_taken);

    // Inject Fault 
    //arc_encoded[data_size/2] = arc_encoded[data_size/2] ^ 0b00000001;
    //for (j = 18; j < 100000; j=j+2){
    //    arc_encoded[j] = arc_encoded[j] ^ 0b00000001;
    //}
    //for (j = 0; j < 50; j=j+2){
    //    int index = 29 + ((256)*8 + (256))*j;
    //    arc_encoded[index] = arc_encoded[index] ^ 0b00000001;
    //}

    // Decode data
    printf("Decoding Data\n");
    uint32_t arc_decoded_size;
    uint8_t* arc_decoded;
    gettimeofday(&start, NULL);
    err = arc_decode(arc_encoded, arc_encoded_size, &arc_decoded, &arc_decoded_size);
    gettimeofday(&stop, NULL);
    decode_time_taken = (double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec);
    printf("Decoding on %d threads Completed in %lf Seconds!\n", i, decode_time_taken);

    // Free 
    free(arc_encoded);
    free(arc_decoded);

    // Close ARC Correctly
    arc_save();
    arc_close();
}