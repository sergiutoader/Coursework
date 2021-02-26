
default: build

build:
	mpicc main.c stringProcessing.c -o main 

run:
	mpirun  --oversubscribe -np 5 main $(INFILE)
	

clean:
	rm main
	rm -r *.dSYM