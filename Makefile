vite : vite.o line.o singlechar.o
	gcc -o vite vite.o line.o singlechar.o

vite.o : vite.c
	gcc -c vite.c -Wall

line.o : line.c
	gcc -c line.c -Wall

singlechar.o : singlechar.c
	gcc -c singlechar.c -Wall
