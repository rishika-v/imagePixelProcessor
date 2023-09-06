# <Rishika Vadlamudi>
#<rvadlam2>

#Nandita Balaji
#nbalaji2

# Midterm Project - Makefile

CC=gcc
CFLAGS=-std=c99 -pedantic -Wall -Wextra
DFLAGS= -g -O0
FLAGS= $(CFLAGS) $(DFLAGS)

## TODO: Write your own Makefile commands to link and compile your project as well as the cleanup
project: project.o img_processing.o ppm_io.o
	$(CC) -o project project.o img_processing.o ppm_io.o

project.o: project.c img_processing.h ppm_io.h
	$(CC) -c project.c $(FLAGS)

img_processing.o: img_processing.c img_processing.h ppm_io.h
	$(CC) -c img_processing.c $(FLAGS)

## Below are commands to link and compile the checkerboard program
# Links together files needed to create the checkerboard executable
checkerboard : checkerboard.o ppm_io.o
	$(CC) -o $@ checkerboard.o ppm_io.o

# Compile the ppm i/o source code
ppm_io.o: ppm_io.c ppm_io.h
	$(CC) $(CFLAGS) -c ppm_io.c

# Removes all object files and the executable named project, so we can start fresh
clean:
	rm -f *.o checkerboard project *.ppm
