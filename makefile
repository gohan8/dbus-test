CC=g++

SOURCEDIR=src
BINDIR=bin
OBJDIR=obj

LDFLAGS=-lglib-2.0 -lgio-2.0 -lgobject-2.0
CFLAGS=-c -I/usr/include/glib-2.0 -I/usr/lib/i386-linux-gnu/glib-2.0/include

src=$(SOURCEDIR)/*.c 

all: $(BINDIR)/a.out

$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) -o 

bin/a.out: obj/%.o
	$(CC) $(LSFLAGS) 
