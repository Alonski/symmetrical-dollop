#include <stdio.h>
#include <string.h>
#include "mpi.h"

int main(int argc, char *argv[])
{
	int my_rank;	   /* rank of process */
	int p;			   /* number of processes */
	int source;		   /* rank of sender */
	int dest;		   /* rank of receiver */
	int tag = 0;	   /* tag for messages */
	char message[100]; /* storage for message */
	MPI_Status status; /* return status for receive */

	/* start up MPI */

	MPI_Init(&argc, &argv);

	/* find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	/* find out number of processes */
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	if (my_rank != 0)
	{
		/* create message */
		int len;
		sprintf(message, "Hello MPI World from process %d!", my_rank);
		dest = 0;
		/* use strlen+1 so that '\0' get transmitted */
		len = (int)strlen(message) + 1;
		MPI_Send(&len, 1, MPI_INT,
				 dest, tag, MPI_COMM_WORLD);

		MPI_Send(message, len, MPI_CHAR,
				 dest, tag, MPI_COMM_WORLD);
	}
	else
	{
		int len;
		printf("Hello MPI World From process 0: Num processes: %d\n", p);
		for (source = 1; source < p; source++)
		{
			MPI_Recv(&len, 1, MPI_INT, source, tag,
					 MPI_COMM_WORLD, &status);
			MPI_Recv(message, len, MPI_CHAR, source, tag,
					 MPI_COMM_WORLD, &status);
			printf("%s\n", message);
		}
	}
	/* shut down MPI */
	MPI_Finalize();

	return 0;
}