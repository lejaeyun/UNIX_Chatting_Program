CC=gcc
CFLAGES=-g -Wall
OBJS=HW1_main.o HW1_func.o
TARGET=a.out

all : $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ -lncurses -lpthread

HW1_main.o: HW1_header.h HW1_main.c
HW1_func.o: HW1_header.h HW1_func.c

clean:
	rm -f *.o
	rm -f $(TARGET)

