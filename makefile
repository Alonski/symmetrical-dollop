build:
	mpicc -c $(FILE).c 
	mpicc -o exec $(FILE).o

clean:
	rm *.o exec

run:
	mpiexec -np $(N) ./exec 
