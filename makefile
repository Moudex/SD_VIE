CC=gcc
CFLAGS=-W -Wall -std=c99
LDFLAGS=
EXEC=client

all: $(EXEC)

client: client.o commande.o jvie.o
	$(CC) -o $@ $^ $(LDFLAGS)

client.o: client.c commande.h jvie.h
	$(CC) -o $@ -c $< $(CFLAGS)

commande.o: commande.c
	$(CC) -o $@ -c $< $(CFLAGS)

jvie.o: jvie.c
	$(CC) -o $@ -c $< $(CFLAGS)

.PHONY: clean mrproper

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)
