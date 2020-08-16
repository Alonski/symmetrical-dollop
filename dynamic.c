#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define HEAVY 100000
#define SHORT 1
#define LONG 10

enum ranks
{
    ROOT,
    N = 20,
    WORK_TAG = 0,
    END_TAG = 1
};

struct Point
{
    // Struct used to hold both x,y
    int x, y;
};

void masterProcess(int num_procs);
void slaveProcess();
int generateNumbers(struct Point *arr, int size);
void sendInitialJobs(struct Point *tasks, int num_procs);
double receiveAndSendWorkJobs(struct Point *tasks, int num_procs, int jobs_to_do);
double receiveFinalEndJobs(struct Point *tasks, int num_procs);

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
    int my_rank, num_procs;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    if (my_rank == ROOT)
    {
        // The ROOT controls the other processes
        masterProcess(num_procs);
    }
    else
        // The actual calculations will happen for non ROOT processes.
        slaveProcess();

    MPI_Finalize();
    return 0;
}

void masterProcess(int num_procs)
{
    double all_sum = 0, start_time = MPI_Wtime();
    int arr_size = N * N;

    // Set jobs to do == arr_size as each process will run a single task at a time
    int jobs_to_do = arr_size;

    // Define an array to hold all tasks where each task is an X,Y Point to calculate
    struct Point *tasks = (struct Point *)malloc(sizeof(struct Point) * arr_size);
    generateNumbers(tasks, N);

    sendInitialJobs(tasks, num_procs);

    all_sum = receiveAndSendWorkJobs(tasks, num_procs, jobs_to_do) + receiveFinalEndJobs(tasks, num_procs);

    // Print the answers and the total time taken
    printf("Dynamic answer = %e - Time %lf\n", all_sum, MPI_Wtime() - start_time);
}

void slaveProcess()
{
    int tag;
    double my_sum = 0;
    struct Point my_task;

    MPI_Status status;
    do
    {
        // Receive a task from the ROOT process
        MPI_Recv(&my_task, sizeof(struct Point), MPI_INT, ROOT, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        tag = status.MPI_TAG;

        // Calculate the current task
        my_sum = heavy(my_task.x, my_task.y);

        // Return the result for this task to the ROOT process
        MPI_Send(&my_sum, 1, MPI_DOUBLE, ROOT, tag, MPI_COMM_WORLD);
    } while (tag != END_TAG);
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

void sendInitialJobs(struct Point *tasks, int num_procs)
{
    for (int worker_id = 1; worker_id < num_procs; worker_id++)
    {
        // Send the first task to each process
        MPI_Send(tasks + (worker_id - 1), sizeof(struct Point), MPI_INT, worker_id, WORK_TAG, MPI_COMM_WORLD);
    }
}

double receiveAndSendWorkJobs(struct Point *tasks, int num_procs, int jobs_to_do)
{
    MPI_Status status;
    double all_sum = 0, worker_sum = 0;
    int tag, source;
    for (int jobs_sent = num_procs - 1; jobs_sent < jobs_to_do; jobs_sent++)
    {
        if (jobs_to_do - jobs_sent > num_procs - 1)
            tag = WORK_TAG;
        else
            tag = END_TAG;

        // Receive a sum for a task from a process that isn't finished
        MPI_Recv(&worker_sum, 1, MPI_DOUBLE, MPI_ANY_SOURCE, WORK_TAG, MPI_COMM_WORLD, &status);

        source = status.MPI_SOURCE;

        // Send another task to the received process
        MPI_Send(tasks + jobs_sent, sizeof(struct Point), MPI_INT, source, tag, MPI_COMM_WORLD);

        all_sum += worker_sum;
    }
    return all_sum;
}

double receiveFinalEndJobs(struct Point *tasks, int num_procs)
{
    MPI_Status status;
    double all_sum = 0, worker_sum = 0;
    for (int worker_id = 1; worker_id < num_procs; worker_id++)
    {
        // Receive a sum for a task from a process that is finished
        MPI_Recv(&worker_sum, 1, MPI_DOUBLE, MPI_ANY_SOURCE, END_TAG, MPI_COMM_WORLD, &status);

        all_sum += worker_sum;
    }
    return all_sum;
}