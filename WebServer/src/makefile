PROJ   = WebServer
OBJS   = main.o server.o signals.o socket.o client.o http.o resource.o
CC     = gcc
LINK   = gcc
RM     = rm -rf
CFLAGS = -c -Wall -I.
LIBS   = -lpthread

$(PROJ): $(OBJS)
	$(LINK) $^ $(LIBS) -o ../bin/$@

.c.o:
	$(CC) $(CFLAGS) $^

clean:
	$(RM) ../bin/$(PROJ) $(OBJS)
