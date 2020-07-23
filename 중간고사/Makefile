#201624548_leejaeyoon
CC=gcc
CFLAGES=-g -Wall


all :201624548-Lee

201624548-Lee: 201624548-Lee.o
	$(CC) $(CFLAGES) -o $@ $^ -lncurses -lpthread
	
201624548-Lee.o: 201624548-Lee.c

clean:
	rm -f *.o
	rm -f chat

