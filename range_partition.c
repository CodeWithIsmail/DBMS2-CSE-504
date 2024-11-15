#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define DATA_SIZE 100000
#define NUM_THREADS 5

typedef struct
{
    int key;
    int value;
    int thread_id;
} TableRow;

TableRow table1[DATA_SIZE];
TableRow table2[DATA_SIZE];
TableRow join_results[DATA_SIZE];
int result_count = 0;

omp_lock_t lock;

void generate_random_data(TableRow *table)
{
    for (int i = 0; i < DATA_SIZE; i++)
    {
        table[i].key = i + 1;
        table[i].value = rand() % DATA_SIZE + 1;
    }
}

void write_tables_to_file(const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (!file)
    {
        perror("Error opening file for writing");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < DATA_SIZE; i++)
    {
        fprintf(file, "%d %d\n", table1[i].key, table1[i].value);
    }
    fprintf(file, "===\n");
    for (int i = 0; i < DATA_SIZE; i++)
    {
        fprintf(file, "%d %d\n", table2[i].key, table2[i].value);
    }

    fclose(file);
}

void read_tables_from_file(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Error opening file for reading");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < DATA_SIZE; i++)
    {
        fscanf(file, "%d %d", &table1[i].key, &table1[i].value);
    }

    char separator[4];
    fscanf(file, "%s", separator);

    for (int i = 0; i < DATA_SIZE; i++)
    {
        fscanf(file, "%d %d", &table2[i].key, &table2[i].value);
    }

    fclose(file);
}

void range_join(int start, int end, int thread_id)
{
    for (int i = start; i < end; i++)
    {
        for (int j = 0; j < DATA_SIZE; j++)
        {
            if (table1[i].value == table2[j].value)
            {
                omp_set_lock(&lock);
                join_results[result_count].key = table1[i].value;
                join_results[result_count].value = table2[j].value;
                join_results[result_count].thread_id = thread_id;
                result_count++;
                omp_unset_lock(&lock);
            }
        }
    }
}
void write_results_to_file(const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (!file)
    {
        perror("Error opening file for writing results");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < result_count; i++)
    {
        fprintf(file, "%d %d (Processed by Core %d)\n",
                join_results[i].key, join_results[i].value, join_results[i].thread_id);
    }

    fclose(file);
}

int main()
{
    for (int i = 0; i < DATA_SIZE; i++)
    {
        table1[i].key = i + 1;
        table1[i].value = i + 1;
    }
    generate_random_data(table2);
    const char *input_file = "tables.txt";
    write_tables_to_file(input_file);
    read_tables_from_file(input_file);
    omp_init_lock(&lock);

#pragma omp parallel num_threads(NUM_THREADS)
    {
        int thread_id = omp_get_thread_num();
        int chunk_size = DATA_SIZE / NUM_THREADS;
        int start = thread_id * chunk_size;
        int end = (thread_id == NUM_THREADS - 1) ? DATA_SIZE : start + chunk_size;

        range_join(start, end, thread_id);
    }

    omp_destroy_lock(&lock);

    const char *output_file = "join_results.txt";
    write_results_to_file(output_file);
    printf("Join results written to %s\n", output_file);

    return 0;
}