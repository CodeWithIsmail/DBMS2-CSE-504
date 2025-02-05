#include <stdio.h>

#include <stdlib.h>

#include <omp.h>

#include <string.h>

#define DATA_SIZE_1 100000

#define DATA_SIZE_2 10000

#define NUM_CORES 4

typedef struct
{

    int number;

    char text1[20];

    char text2[20]; // Text from the second array

} DataRow;

void generate_data(DataRow *data, int size, const char *prefix)
{

    for (int i = 0; i < size; i++)
    {

        data[i].number = rand() % 10000; // Random number

        snprintf(data[i].text1, 20, "%s_%d", prefix, i); // Random text for first array

        snprintf(data[i].text2, 20, "%s_%d", prefix, i); // Random text for second array
    }
}

// Comparator for sorting

int compare(const void *a, const void *b)
{

    return ((DataRow *)a)->number - ((DataRow *)b)->number;
}

int main()
{

    DataRow *data1 = (DataRow *)malloc(DATA_SIZE_1 * sizeof(DataRow));

    DataRow *data2 = (DataRow *)malloc(DATA_SIZE_2 * sizeof(DataRow));

    DataRow *union_table = (DataRow *)malloc((DATA_SIZE_1 + DATA_SIZE_2) * sizeof(DataRow));

    if (!data1 || !data2 || !union_table)
    {

        printf("Memory allocation failed!\n");

        return EXIT_FAILURE;
    }

    // Generate data for both arrays

    generate_data(data1, DATA_SIZE_1, "A");

    generate_data(data2, DATA_SIZE_2, "B");

    // Merge two tables

    int total_size = DATA_SIZE_1 + DATA_SIZE_2;

    memcpy(union_table, data1, DATA_SIZE_1 * sizeof(DataRow));

    memcpy(union_table + DATA_SIZE_1, data2, DATA_SIZE_2 * sizeof(DataRow));

    // Sort by number column to make union easier

    qsort(union_table, total_size, sizeof(DataRow), compare);

    // Open file in append mode to ensure all writes happen

    FILE *output_file = fopen("partitioning_results.txt", "w");

    if (output_file == NULL)
    {

        perror("Error opening file");

        return EXIT_FAILURE;
    }

    int partition_size = total_size / NUM_CORES;

    // Parallel execution

#pragma omp parallel num_threads(NUM_CORES)

    {

        int core_id = omp_get_thread_num();

        int start = core_id * partition_size;

        int end = (core_id == NUM_CORES - 1) ? total_size : (core_id + 1) * partition_size;

        // Write to file directly inside the parallel region

#pragma omp critical

        {

            fprintf(output_file, "Core %d processing rows %d to %d\n", core_id + 1, start, end);

            for (int i = start; i < end; i++)
            {

                fprintf(output_file, "Core %d processed [%d] = (%d, %s, %s)\n",

                        core_id + 1, i, union_table[i].number, union_table[i].text1, union_table[i].text2);
            }
        }
    }

    fclose(output_file);

    printf("Processing complete. Results written to 'partitioning_results.txt'.\n");

    // Free memory

    free(data1);

    free(data2);

    free(union_table);

    return 0;
}