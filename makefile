CC=gcc
CFLAGS=-W -Wall -std=c99 -g
LDFLAGS=
EXEC=client server

all: $(EXEC)

server: server.o commande.o jvie.o jvie_serv.o
	$(CC) -o $@ $^ $(LDFLAGS)

client: client.o commande.o jvie.o
	$(CC) -o $@ $^ $(LDFLAGS)

client.o: client.c commande.h jvie.h
	$(CC) -o $@ -c $< $(CFLAGS)

commande.o: commande.c
	$(CC) -o $@ -c $< $(CFLAGS)

jvie.o: jvie.c
	$(CC) -o $@ -c $< $(CFLAGS)

jvie_serv.o: jvie_serv.c
	$(CC) -o $@ -c $< $(CFLAGS)

.PHONY: clean mrproper

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)
