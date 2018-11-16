seam_carver:  main.o seam_carving_functions.o 
	gcc -o seam_carver main.o seam_carving_functions.o -lm -O3
seam_carving_functions.o: seam_carving_functions.c seam_carving_functions.h
	gcc -c seam_carving_functions.c -O3
main.o: main.c
	gcc -c main.c -O3
clean:
	rm -f *.o


