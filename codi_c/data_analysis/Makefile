C=main.c dr_reader.c data_dimensions.c llistad.c \
data_velos.c data_rug.c data_n.c llistai.c data_stress_molecular.c \
data_energy.c dr_writer.c
O=main.o dr_reader.o data_dimensions.o llistad.o \
data_velos.o data_rug.o data_n.o llistai.o data_stress_molecular.o \
data_energy.o dr_writer.o # ojo en posar sources aquí!

CC=gcc
CFLAGS=-I/usr/local/include/ -I. -ffast-math -Wall
LFLAGS=-L/usr/local/lib/ -lgsl -lgslcblas -lm
DEBUG=#-ggdb3

EXECUTABLE=data_analysis

$(EXECUTABLE) : $(O) triangle.o
	$(CC) $(DEBUG) $(LFLAGS) -o $(EXECUTABLE) $O triangle.o

# $(EXECUTABLE) : $O 
# 	$(CC) $(DEBUG) $(LFLAGS) -o $(EXECUTABLE) $O

main.o : main.c
	$(CC) $(CFLAGS) $(DEBUG) -c main.c

triangle.o : triangle.c
	$(CC) $(DEBUG) $(CFLAGS) -DTRILIBRARY -c -o triangle.o triangle.c

.PHONY : clean
clean:
	-rm $(O) $(EXECUTABLE) triangle.o
