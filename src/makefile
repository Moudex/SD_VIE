CC=gcc
CFLAGS=-W -Wall -std=c99 -g
LDFLAGS=-lncurses
EXEC=client server

all: $(EXEC)

server: server.o commande.o jvie.o jvie_serv.o affichage.o
	$(CC) -o $@ $^ $(LDFLAGS)

client: client.o commande.o jvie.o affichage.o
	$(CC) -o $@ $^ $(LDFLAGS)

client.o: client.c client.h commande.h jvie.h
	$(CC) -o $@ -c $< $(CFLAGS)

commande.o: commande.c commande.h
	$(CC) -o $@ -c $< $(CFLAGS)

jvie.o: jvie.c jvie.h
	$(CC) -o $@ -c $< $(CFLAGS)

jvie_serv.o: jvie_serv.c jvie_serv.h
	$(CC) -o $@ -c $< $(CFLAGS)

affichage.o: affichage.c affichage.h jvie.h jvie_serv.h
	$(CC) -I/usr/include/ncurses -o $@ -c $< $(CFLAGS) -lncurses

.PHONY: clean mrproper

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)
