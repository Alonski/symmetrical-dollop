#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "mpi.h"

enum ranks
{
	ROOT,
	SLAVE,
	ARR_SIZE = 800000000000
};

int sumArr(int *arr, int size);

void generateNumbers(int *arr, int size);

int main(int argc, char *argv[])
{

	int my_rank, num_procs;
	MPI_Init(&argc, &argv);

	/* find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	/* find out number of processes */
	MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

	if (my_rank == ROOT)
	{
		int numbers[ARR_SIZE], otherProc = SLAVE, mySum, hisSum;
		long start;
		generateNumbers(numbers, ARR_SIZE);
		start = MPI_Wtime();
		MPI_Send(numbers + ARR_SIZE / 2, ARR_SIZE / 2, MPI_INT, otherProc, 0, MPI_COMM_WORLD);
		mySum = sumArr(numbers, ARR_SIZE / 2);
		MPI_Recv(&hisSum, 1, MPI_INT, otherProc, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		printf("---Parallel---\n");
		printf("Total time: %lf\n", MPI_Wtime() - start);
		printf("The sum is: %d\n", mySum + hisSum);

		//control
		start = MPI_Wtime();
		mySum = sumArr(numbers, ARR_SIZE);
		printf("---Serial---\n");
		printf("Total time: %lf\n", MPI_Wtime() - start);
		printf("The sum is: %d\n", mySum);
	}
	else
	{
		int numbers[ARR_SIZE / 2], sum;
		MPI_Recv(numbers, ARR_SIZE / 2, MPI_INT, ROOT, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		sum = sumArr(numbers, ARR_SIZE / 2);
		MPI_Send(&sum, 1, MPI_INT, ROOT, 0, MPI_COMM_WORLD);
	}

	MPI_Finalize();

	return 0;
}

void generateNumbers(int *arr, int size)
{
	int i;
	srand(time(NULL));
	for (i = 0; i < size; i++)
		arr[i] = rand() % 101;
}

int sumArr(int *arr, int size)
{
	int i, sum = 0;
	for (i = 0; i < size; i++)
		sum += arr[i];
	return sum;
}
