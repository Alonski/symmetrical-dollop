#include <mpi.h>
#include <stdio.h>
#include <math.h>

#define HEAVY 100000
#define SHORT 1
#define LONG 10

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
    int x, y;
    int N = 20;
    double start_time, answer = 0;

    MPI_Init(&argc, &argv);

    start_time = MPI_Wtime();

    for (x = 0; x < N; x++)
        for (y = 0; y < N; y++)
            answer += heavy(x, y);

    printf("Sequential answer = %e\n", answer);
    printf("Time %lf\n", MPI_Wtime() - start_time);
    return 0;
}