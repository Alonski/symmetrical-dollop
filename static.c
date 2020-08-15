#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define HEAVY 100000
#define SHORT 1
#define LONG 10

#define ROOT 0

struct Point
{
    // Struct used to hold both x,y
    int x, y;
};

int generateNumbers(struct Point *arr, int size);

// This function performs heavy computations,
// its run time depends on x and y values
double heavy(int x, int y)
{
    int i, loop = SHORT;
    double sum = 0;
    // Super heavy tasks
    if (x < 3 || y < 3)
        loop = LONG;
    // Heavy calculations
    for (i = 0; i < loop * HEAVY; i++)
        sum += cos(exp(sin((double)i / HEAVY)));
    return sum;
}

int main(int argc, char *argv[])
{
    int my_rank, num_procs, my_size, x, y, i, N = 20, arr_size = N * N;
    double start_time, my_sum, all_sum, calc;

    // Array of tasks scattered into each process
    struct Point *my_tasks;
    // Array of tasks which hold all the x,y Point pairs
    struct Point tasks[400];

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    if (my_rank == ROOT)
    {
        start_time = MPI_Wtime();
        generateNumbers(tasks, N);
    }

    // Broadcast all tasks to all processes. We know that tasks is a Point made up of two ints.
    // So we pass it in with a size of arr_size * 2. This will broadcast all tasks correctly
    MPI_Bcast(tasks, arr_size * 2, MPI_INT, ROOT, MPI_COMM_WORLD);

    // Calculate the size of array that each process should receive
    my_size = arr_size / num_procs;
    my_tasks = (struct Point *)malloc(sizeof(struct Point) * my_size);

    // Scatter the tasks into my_tasks for each process. Again passing in the size * 2 to account for the Point struct
    MPI_Scatter(tasks, my_size * 2, MPI_INT, my_tasks, my_size * 2, MPI_INT, ROOT, MPI_COMM_WORLD);

    for (i = 0; i < my_size; i++)
    {
        calc = heavy(my_tasks[i].x, my_tasks[i].y);
        // Calculate the sum for the tasks for each process
        my_sum += calc;
    }

    if (my_rank == ROOT)
    {
        // Calculate the remaining tasks that weren't handled by other processes in the case of a num_procs that isn't divisible by arr_size
        my_size = arr_size % num_procs;
        int start = arr_size - my_size;

        for (i = start; i < arr_size; i++)
        {
            calc = heavy(my_tasks[i].x, my_tasks[i].y);
            // printf("Received point: %d - %d. The sum is: %f\n", my_tasks[i].x, my_tasks[i].y, calc);
            my_sum += calc;
        }
    }

    // Reduce the answers back to the root
    MPI_Reduce(&my_sum, &all_sum, 1, MPI_DOUBLE, MPI_SUM, ROOT, MPI_COMM_WORLD);

    if (my_rank == ROOT)
    {
        // Print the answers and the total time taken
        printf("Static answer = %e - Time %lf\n", all_sum, MPI_Wtime() - start_time);
    }

    MPI_Finalize();
    return 0;
}

int generateNumbers(struct Point *arr, int size)
{
    int i = 0, x, y;

    for (x = 0; x < size; x++)
    {
        for (y = 0; y < size; y++)
        {
            arr[i].x = x;
            arr[i].y = y;
            i++;
        }
    }
}
