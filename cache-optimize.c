#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define N 800

void generateNumbers(int *mat);

void multiply(int *mat1, int *mat2, int *res)
{
    int i, j, k;
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            res[i * N + j] = 0;
            for (k = 0; k < N; k++)
                res[i * N + j] += mat1[i * N + k] * mat2[k * N + j];
        }
    }
}

void transpose(int *A, int *B)
{
    int i, j;
    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++)
            B[i * N + j] = A[j * N + i];
}

void multiply_transpose(int *mat1, int *mat2, int *res)
{
    int i, j, k;
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            res[i * N + j] = 0;
            for (k = 0; k < N; k++)
                res[i * N + j] += mat1[i * N + k] * mat2[j * N + k];
        }
    }
}

int areSame(int *A, int *B)
{
    int i, j;
    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++)
            if (A[i * N + j] != B[i * N + j])
                return 0;
    return 1;
}

int main(int argc, char *argv[])
{

    MPI_Init(&argc, &argv);
    double time0;
    int *mat1 = NULL;
    int *mat2 = NULL;
    int *mat2_transpose = NULL;
    int *res1 = NULL;
    int *res2 = NULL;

    mat1 = (int *)malloc(N * N * sizeof(int));
    if (!mat1)
        goto exit;
    mat2 = (int *)malloc(N * N * sizeof(int));
    if (!mat2)
        goto exit;
    mat2_transpose = (int *)malloc(N * N * sizeof(int));
    if (!mat2_transpose)
        goto exit;
    res1 = (int *)malloc(N * N * sizeof(int));
    if (!res1)
        goto exit;
    res2 = (int *)malloc(N * N * sizeof(int));
    if (!res2)
        goto exit;

    generateNumbers(mat1);
    generateNumbers(mat2);

    time0 = MPI_Wtime();
    multiply(mat1, mat2, res1);
    printf("Normal time %lf\n", MPI_Wtime() - time0);

    transpose(mat2, mat2_transpose);
    time0 = MPI_Wtime();
    multiply_transpose(mat1, mat2_transpose, res2);
    printf("Optimized time %lf\n", MPI_Wtime() - time0);

    if (areSame(res1, res2))
        printf("Matrices are identical!\n");
    else
        printf("Matrices are not identical!\n");

exit:
    free(mat1);
    free(mat2);
    free(mat2_transpose);
    free(res1);
    free(res2);

    MPI_Finalize();
    return 0;
}

void generateNumbers(int *mat)
{
    int i, j;

    srand(time(NULL));
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            mat[i * N + j] = rand() % 101;
        }
    }
}
