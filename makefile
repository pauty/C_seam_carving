FLAGS =-O2

seam_carver:  main.o seam_carving_functions.o 
	gcc -o seam_carver main.o seam_carving_functions.o -lm $(FLAGS)
seam_carving_functions.o: seam_carving_functions.c seam_carving_functions.h
	gcc -c seam_carving_functions.c $(FLAGS)
main.o: main.c
	gcc -c main.c $(FLAGS)
clean:
	rm -f *.o


