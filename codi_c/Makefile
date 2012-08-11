C=main.c globals.c sistema.c gsl_stuff.c \
ioutils.c cells.c evolve.c odedef.c odedef_random.c \
string_utils.c odedef_parets.c data.c \
boundaries.c  \
euler_simple.c energy.c \
memoria.c output_settings.c inicia_cristall.c \
dr_writer.c random_generator.c #data_rug.c
#Fitxers.c parets.c utils.c String.c 

O=main.o globals.o sistema.o gsl_stuff.o \
ioutils.o cells.o evolve.o odedef.o odedef_random.o \
string_utils.o odedef_parets.o data.o \
boundaries.o  \
euler_simple.o energy.o \
memoria.o output_settings.o inicia_cristall.o \
dr_writer.o random_generator.o #data_rug.o
#Fitxers.o parets.o utils.o String.o 

CC=gcc
CFLAGS=-I/usr/local/include/ -I. -ffast-math -Wall -Winline -O \
-finline-functions -funroll-all-loops -ftree-vectorize -Wall #-pg
#he canviat O a O3
LFLAGS=-L/usr/local/lib/ -lgsl -lgslcblas -lm -O -finline-functions #-pg
DEBUG=#-ggdb3
EXECUTABLE=executable

$(EXECUTABLE) : $(O)
	$(CC) $(DEBUG) $(LFLAGS) -o $(EXECUTABLE) $O

main.o : main.c
	$(CC) $(DEBUG) $(CFLAGS) -c main.c

.PHONY : clean
clean:
	-rm $(O) $(EXECUTABLE)

.PHONY : dclean
dclean:
	-rm $(0) debug

debug :	debug_ $(EXECUTABLE)

debug_: 
	DEBUG=-ggdb3
	EXECUTABLE=debug
