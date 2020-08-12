#include <stdio.h>
#include <math.h>
#include <mpi.h>

#define HEAVY 100000
#define SHORT 1
#define LONG 10

enum ranks{ROOT,N=20,WORK_TAG=0,END_TAG = 1};
void masterProcess(int num_procs);
void slaveProcess();
double heavy(int x, int y);
struct Point
    {
    // Struct used to hold both x,y
    int x, y;
    };



int main(int argc, char *argv[])
{
    int x, y;
    double answer = 0;
    int my_rank, num_procs,i,my_num,result;
    
    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    if(my_rank == ROOT)
    {
        //will call master upon first instance 
        masterProcess(num_procs);
    }
    else
        //all other tasks will happen in slave
        slaveProcess();

    MPI_Finalize();
    return 0;

}

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

void masterProcess(int num_procs)
    {
    // Array of tasks which hold all the x,y Point pairs.
    struct Point tasks[400];
        
        double start = MPI_Wtime();
        MPI_Status status;
        int worker_id,*arr;
        //define an array to hold all tasks where each task is a point to calculate, -1 is for removing the master instance as the master doesnt claculate
        int arr_size = (num_procs-1) * N * N;
        int jobs_sent=0,
        // jobs to do == arr_size as each machine will run a single task at a time 
        jobs_to_do = arr_size;
        int i=0,x,y;
        int my_sum=0,source,tag,all_sum;
        arr = (struct Point *)malloc(sizeof(struct Point) * arr_size);

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

        for(worker_id = 1;worker_id<num_procs;worker_id++)
            MPI_Send(arr+(worker_id-1),sizeof(struct Point),MPI_INT,worker_id,WORK_TAG,MPI_COMM_WORLD);
    
        for(jobs_sent = num_procs-1;jobs_sent<jobs_to_do;jobs_sent++)
        {
            if(jobs_to_do-jobs_sent > num_procs-1)
                tag = WORK_TAG;
            else
                tag = END_TAG;

            MPI_Recv(&my_sum,1,MPI_INT,MPI_ANY_SOURCE,WORK_TAG,MPI_COMM_WORLD,&status);
            source = status.MPI_SOURCE;
            MPI_Send(arr + jobs_sent,sizeof(struct Point),MPI_INT,source,tag,MPI_COMM_WORLD);
        }

        for(worker_id = 1;worker_id<num_procs;worker_id++)
        {
            MPI_Recv(&my_sum,1,MPI_INT,MPI_ANY_SOURCE,END_TAG,MPI_COMM_WORLD,&status);
            all_sum += my_sum;
        }
    
    printf("Total answer = %e\n", all_sum);
    printf("Time %lf\n", MPI_Wtime() - start);
    }
    
void slaveProcess()
    {
        int tag,my_sum,i;
        struct Point my_point;

        MPI_Status status;
    do
    {
        MPI_Recv(&my_point,sizeof(struct Point),MPI_INT,ROOT,MPI_ANY_TAG,MPI_COMM_WORLD,&status);

        tag = status.MPI_TAG;
     
        my_sum += heavy(my_point.x,my_point.y);
        
        MPI_Send(&my_sum,1,MPI_INT,ROOT,tag,MPI_COMM_WORLD);



    } while(tag != END_TAG);
    }
