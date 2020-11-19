.PHONY : archive clean open
CFLAGS  = -g
CC   		= gcc
TER  		= gnome-terminal

################################################################################

all: cache_reset Client Local_DNS Root_DNS TLD_DNS TLD_in Authoritative_DNS Authoritative_in

Client:
	${CC} $(CFLAGS) -o Client Client.c

Local_DNS:
	${CC} $(CFLAGS) -o Local_DNS Local_DNS.c

Root_DNS:
	${CC} $(CFLAGS) -o Root_DNS Root_DNS.c

TLD_DNS:
	${CC} $(CFLAGS) -o TLD_DNS TLD_DNS.c

TLD_in:
	${CC} $(CFLAGS) -o TLD_in TLD_in.c

Authoritative_DNS:
	${CC} $(CFLAGS) -o Authoritative_DNS Authoritative_DNS.c

Authoritative_in:
	${CC} $(CFLAGS) -o Authoritative_in Authoritative_in.c

################################################################################
open:
	${TER} -e ./Authoritative_in
	${TER} -e ./Authoritative_DNS
	${TER} -e ./TLD_in
	${TER} -e ./TLD_DNS
	${TER} -e ./Root_DNS
	${TER} -e ./Local_DNS
	${TER} -e ./Client

cache_reset :
	> Fichiers_txt/localcache.txt

clean:
	rm Client Local_DNS Root_DNS TLD_DNS TLD_in Authoritative_DNS Authoritative_in

archive :
	tar -cvzf benbassou.tar.gz *.c Fichiers_txt test Makefile portnumber_management *.pdf
