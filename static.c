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
    int my_rank, num_procs, my_size;
    int x, y, i;
    int N = 20;
    int arr_size = N * N;
    double start_time, my_sum, all_sum;

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
        i = 0;
        // Create the tasks array which holds x,y Point pairs
        for (x = 0; x < N; x++)
        {
            for (y = 0; y < N; y++)
            {
                tasks[i].x = x;
                tasks[i].y = y;
                i++;
            }
        }
    }

    // Broadcast all tasks to all processes. We know that tasks is a Point made up of two ints.
    // So we pass it in with a size of arr_size * 2. This will broadcast all tasks correctly
    MPI_Bcast(tasks, arr_size * 2, MPI_INT, ROOT, MPI_COMM_WORLD);

    // Calculate the size of array that each process should receive
    my_size = arr_size / num_procs;
    my_tasks = (struct Point *)malloc(sizeof(struct Point) * my_size);

    // Scatter the tasks into my_tasks for each process. Again passing in the size * 2 to account for the Point struct
    MPI_Scatter(tasks, my_size * 2, MPI_INT, my_tasks, my_size * 2, MPI_INT, ROOT, MPI_COMM_WORLD);

    // printf("Process %d. Size: %d. Calculating from X: %d Y: %d\n", my_rank, my_size, my_tasks[0].x, my_tasks[0].y);
    for (i = 0; i < my_size; i++)
    {
        // Calculate the sum for the tasks for each process
        my_sum += heavy(my_tasks[i].x, my_tasks[i].y);
    }

    if (my_rank == ROOT)
    {
        // Calculate the remaining tasks that weren't handled by other processes in the case of a num_procs that isn't divisible by arr_size
        my_size = arr_size % num_procs;
        int start = arr_size - my_size;
        // printf("Extra start: %d - my_size: %d\n", start, my_size);
        // printf("Process %d. Size: %d. Calculating from X: %d Y: %d\n", my_rank, my_size, tasks[start].x, tasks[start].y);

        for (i = start; i < arr_size; i++)
        {
            // printf("X: %d. Y: %d\n", tasks[i].x, tasks[i].y);
            my_sum += heavy(tasks[i].x, tasks[i].y);
        }
    }

    // printf("Answer for process %d = %e\n", my_rank, my_sum);

    // Reduce the answers back to the root
    MPI_Reduce(&my_sum, &all_sum, 1, MPI_DOUBLE, MPI_SUM, ROOT, MPI_COMM_WORLD);

    if (my_rank == ROOT)
    {
        // Print the answers and the total time taken
        printf("Total answer = %e\n", all_sum);
        printf("Time %lf\n", MPI_Wtime() - start_time);
    }

    MPI_Finalize();
    return 0;
}
