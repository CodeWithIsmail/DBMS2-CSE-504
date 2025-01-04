#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <time.h>

#define DATA_SIZE 100000
#define NUM_THREADS 4

int array1[DATA_SIZE];
int array2[DATA_SIZE];

typedef struct {
    int key;
    int value;
    int thread_id; 
} JoinResult;

JoinResult join_results[DATA_SIZE];
int result_count = 0;

void generate_random_data(int* array) {
    srand(time(NULL)); 
    for (int i = 0; i < DATA_SIZE; i++) {
        array[i] = rand() % DATA_SIZE*2 + 1; 
    }
}

void range_join(int start, int end, int thread_id) {
    for (int i = start; i < end; i++) {
        for (int j = 0; j < DATA_SIZE; j++) {
            if (array1[i] == array2[j]) {
                join_results[result_count].key = array1[i];
                join_results[result_count].value = array2[j];
                join_results[result_count].thread_id = thread_id;
                result_count++;
            }
        }
    }
}

void range_join_serial(int start, int end, int thread_id) {
    for (int i = 0; i < DATA_SIZE; i++) {
        for (int j = 0; j < DATA_SIZE; j++) {
            if (array1[i] == array2[j]) {
                join_results[result_count].key = array1[i];
                join_results[result_count].value = array2[j];
                join_results[result_count].thread_id = thread_id;
                result_count++;
            }
        }
    }
}

int main() {
    for (int i = 0; i < DATA_SIZE; i++) {
        array1[i] = i + 1;
    }

    generate_random_data(array2);

    clock_t start_time = clock();

    // #pragma omp parallel num_threads(NUM_THREADS)
    // {
    //     int thread_id = omp_get_thread_num();
    //     // printf("id - %d \n", thread_id);        
    //     int chunk_size = DATA_SIZE / NUM_THREADS;
    //     int start = thread_id * chunk_size;
    //     int end = (thread_id == NUM_THREADS - 1) ? DATA_SIZE : start + chunk_size; 

    //     range_join(start, end, thread_id);
        
    // }

   range_join_serial(0,DATA_SIZE,0);

    clock_t end_time = clock();
    double time_taken = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    FILE* output_file = fopen("join_results.txt", "w");
    if (!output_file) {
        perror("Error opening output file");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < result_count; i++) {
        fprintf(output_file, "%d %d (Processed by Core %d)\n",
                join_results[i].key, join_results[i].value, join_results[i].thread_id);
    }
    printf("Time taken for join operation: %f seconds\n", time_taken);

    fclose(output_file);
    printf("Join results written to join_results.txt\n");

    return 0;
}