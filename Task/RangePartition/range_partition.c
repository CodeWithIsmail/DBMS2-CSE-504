#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

typedef struct
{
    int val1;
    int val2;
    int threadId;
} JoinResult;

int countLines(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error opening file for counting!\n");
        exit(1);
    }

    int count = 0;
    char ch;

    while (!feof(file))
    {
        ch = fgetc(file);
        if (ch == '\n')
        {
            count++;
        }
    }

    fclose(file);
    return count;
}

void readArrays(const char *filename, int **arr1, int **arr2, int *size)
{
    *size = countLines(filename);

    *arr1 = (int *)malloc(*size * sizeof(int));
    *arr2 = (int *)malloc(*size * sizeof(int));

    if (*arr1 == NULL || *arr2 == NULL)
    {
        printf("Memory allocation failed!\n");
        exit(1);
    }

    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error opening file for reading!\n");
        exit(1);
    }

    int i = 0;
    while (i < *size && fscanf(file, "%d %d", &(*arr1)[i], &(*arr2)[i]) == 2)
    {
        i++;
    }

    fclose(file);
}

void findRangeBoundaries(int *arr, int size, int numPartitions, int *boundaries)
{
    int min = arr[0];
    int max = arr[0];

    for (int i = 1; i < size; i++)
    {
        if (arr[i] < min)
            min = arr[i];
        if (arr[i] > max)
            max = arr[i];
    }

    int range = (max - min + 1) / numPartitions;
    boundaries[0] = min;

    for (int i = 1; i < numPartitions; i++)
    {
        boundaries[i] = boundaries[i - 1] + range;
    }
    boundaries[numPartitions] = max + 1;

    printf("\nRange Assignments:\n");
    for (int i = 0; i < numPartitions; i++)
    {
        printf("Thread %d: Range [%d, %d)\n", i, boundaries[i], boundaries[i + 1]);
    }
    printf("\n");
}

JoinResult *rangePartitionJoin(int *arr1, int size1, int *arr2, int size2, int *totalJoins)
{
    int numThreads = omp_get_max_threads();
    int *boundaries = (int *)malloc((numThreads + 1) * sizeof(int));

    findRangeBoundaries(arr1, size1, numThreads, boundaries);

    JoinResult *results = (JoinResult *)malloc(size1 * size2 * sizeof(JoinResult));
    *totalJoins = 0;

    int *joinsPerThread = (int *)calloc(numThreads, sizeof(int));

#pragma omp parallel
    {
        int tid = omp_get_thread_num();
        int localJoins = 0;
        JoinResult *localResults = (JoinResult *)malloc(size1 * size2 * sizeof(JoinResult));

        for (int i = 0; i < size1; i++)
        {
            if (arr1[i] >= boundaries[tid] && arr1[i] < boundaries[tid + 1])
            {
                for (int j = 0; j < size2; j++)
                {
                    if (arr2[j] >= boundaries[tid] && arr2[j] < boundaries[tid + 1])
                    {
                        if (arr1[i] == arr2[j])
                        {
                            localResults[localJoins].val1 = arr1[i];
                            localResults[localJoins].val2 = arr2[j];
                            localResults[localJoins].threadId = tid;
                            localJoins++;
                        }
                    }
                }
            }
        }

#pragma omp critical
        {
            for (int i = 0; i < localJoins; i++)
            {
                results[*totalJoins + i] = localResults[i];
            }
            joinsPerThread[tid] = localJoins;
            *totalJoins += localJoins;
        }

        free(localResults);
    }

    free(boundaries);
    free(joinsPerThread);
    return results;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    int *arr1, *arr2;
    int size;
    double start_time, end_time;

    omp_set_num_threads(4);

    readArrays(argv[1], &arr1, &arr2, &size);

    start_time = omp_get_wtime();

    int totalJoins;
    JoinResult *results = rangePartitionJoin(arr1, size, arr2, size, &totalJoins);

    end_time = omp_get_wtime();

    printf("\nTime taken: %f seconds\n", end_time - start_time);

    printf("\nJoin Results:\n");
    for (int i = 0; i < totalJoins; i++)
    {
        printf("CPU %d: (%d, %d)\n",
               results[i].threadId,
               results[i].val1,
               results[i].val2);
    }

    free(arr1);
    free(arr2);
    free(results);
    return 0;
}