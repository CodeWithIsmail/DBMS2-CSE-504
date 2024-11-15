#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N 100000

// Function to check if a number is in the array
int contains(int *arr, int size, int target) {
    for (int i = 0; i < size; i++) {
        if (arr[i] == target)
            return 1;
    }
    return 0;
}

int main() {
    // Initialize two arrays with 10^5 elements
    int *arr1 = malloc(N * sizeof(int));
    int *arr2 = malloc(N * sizeof(int));
    
    // Populate the arrays with some values (for example purposes)
    for (int i = 0; i < N; i++) {
        arr1[i] = i * 2;       // Even numbers
        arr2[i] = i * 3;       // Multiples of 3
    }

    // Each thread will store its results here temporarily
    int *common_elements[4];
    int common_counts[4] = {0, 0, 0, 0}; // Store count of common elements per thread

    for (int i = 0; i < 4; i++) {
        common_elements[i] = malloc((N / 4) * sizeof(int)); // Allocate space per thread
    }

    // Parallel region to process each part of arr1 in a separate thread
    #pragma omp parallel num_threads(4)
    {
        int thread_id = omp_get_thread_num();
        int start = thread_id * (N / 4);     // Start index for this part
        int end = (thread_id + 1) * (N / 4); // End index for this part

        printf("Core %d is processing elements from index %d to %d\n", thread_id + 1, start, end - 1);

        for (int i = start; i < end; i++) {
            if (contains(arr2, N, arr1[i])) {
                common_elements[thread_id][common_counts[thread_id]++] = arr1[i];
            }
        }
    }

    // Print results from each "core"
    for (int i = 0; i < 4; i++) {
        printf("Core %d - ", i + 1);
        for (int j = 0; j < common_counts[i]; j++) {
            printf("%d ", common_elements[i][j]);
        }
        printf("\n");
    }

    // Free allocated memory
    for (int i = 0; i < 4; i++) {
        free(common_elements[i]);
    }
    free(arr1);
    free(arr2);

    return 0;
}
