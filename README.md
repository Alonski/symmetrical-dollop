# Parallel Programming Heavy Computation

Created by Alon Bukai and Oleg Danilok

## Two Parallelization Implementations

### Static Task Pools

### Dynamic Task Pools

## Instructions to Build

To build this project a *Makefile* is included.

Just run `make` in your terminal.

## Instructions to Run

This project must be run using `make run` and supplied the file(FILE) to run and the number(N) of processes to create in parallel.

An example to build and run any program:
```sh
mpicc -c static.c && mpicc -o exec static.o -lm && mpiexec -np 4 ./exec 
```

An example to run the *Sequential* program:
```sh
make run FILE=sequential N=1
```
The above should print: `answer = 8.354289e+06`.

An example to run the *Static* program:
```sh
make run FILE=static N=5
```
The above should print: `answer = 8.354289e+06`.

An example to run the *Dynamic* program:
```sh
make run FILE=dynamic N=5
```
The above should print: `answer = 8.354289e+06`.
