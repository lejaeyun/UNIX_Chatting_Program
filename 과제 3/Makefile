CC=gcc
CFLAGES=-g -Wall


all :chat

chat: chat.o
	$(CC) $(CFLAGES) -o $@ $^ -lncurses -lpthread
	
chat.o: chat.c

clean:
	rm -f *.o
	rm -f chat

