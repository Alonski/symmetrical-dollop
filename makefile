build:
		mpicc -c sequential.c
		mpicc -o sequential sequential.o -lm
		mpicc -c static.c 
		mpicc -o static static.o -lm
		mpicc -c dynamic.c 
		mpicc -o dynamic dynamic.o -lm

clean:
	rm *.o sequential
	rm *.o static
	rm *.o dynamic

run:
	mpiexec -np $(N) ./$(FILE) 