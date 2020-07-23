CC=gcc
CFLAGES=-g -Wall


all :chat chatremove

chat: chat.o
	$(CC) $(CFLAGES) -o $@ $^
	
chatremove: chatremove.o
	$(CC) $(CFLAGES) -o $@ $^

chat.o: chatInfo.h chat.h chat.c
chatremove.o: chatInfo.h chat.h chat.c

clean:
	rm -f *.o
	rm -f chat
	rm -f chatremove

