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
    double start_time, my_sum, all_sum;

    struct Point *my_tasks;
    struct Point tasks[400];

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    if (my_rank == ROOT)
    {
        start_time = MPI_Wtime();
        i = 0;
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

    MPI_Bcast(tasks, N * N * 2, MPI_INT, ROOT, MPI_COMM_WORLD);

    my_size = N * N / num_procs;
    my_tasks = (struct Point *)malloc(sizeof(struct Point) * my_size);

    MPI_Scatter(tasks, my_size * 2, MPI_INT, my_tasks, my_size * 2, MPI_INT, ROOT, MPI_COMM_WORLD);

    for (i = 0; i < my_size; i++)
    {
        my_sum += heavy(my_tasks[i].x, my_tasks[i].y);
    }
    printf("answer for process %d = %e\n", my_rank, my_sum);

    MPI_Reduce(&my_sum, &all_sum, 1, MPI_DOUBLE, MPI_SUM, ROOT, MPI_COMM_WORLD);

    // TODO Compute tasks when tasks doesn't split up evenly between procs using mod

    if (my_rank == ROOT)
    {
        printf("total answer = %e\n", all_sum);
        printf("Time %lf\n", MPI_Wtime() - start_time);
    }

    MPI_Finalize();
    return 0;
}
