#	Makefile for Phigs examples

APPS = tank
OBJ = $(APPS).o
SRC = $(APPS).c

CFLAGS = $(C_OPTS) -I/usr/include
LIBS = -L/usr/X11R6/lib -lX11 -lXi -lglut -lGL -lGLU -lm -lpthread
  
application:$(APPS)

clean:
	rm -f $(APPS) *.raw *.o core a.out

realclean:	clean
	rm -f *~ *.bak *.BAK

.SUFFIXES: c o
.c.o:
	$(CC) -c $(CFLAGS) $<

$(APPS): $(OBJ) 
	$(CC) -o $(APPS) $(CFLAGS) $(OBJ) $(LIBS)

depend:
	makedepend -- $(CFLAGS) $(SRC)

