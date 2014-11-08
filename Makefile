CC=gcc
CFLAGS=-I/usr/include/GL -D_GNU_SOURCE -DPTHREADS -Wall -Wpointer-arith -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wnested-externs -fno-strict-aliasing -Wbad-function-cast -Wold-style-definition -Wdeclaration-after-statement -O2 
LFLAGS=-lGL -lGLEW -lGLU -lGL -lm -lX11 -lXext

glxgears: glxgears.o
	$(CC) -o $@ $^ $(CFLAGS) $(LFLAGS)

glxgears.o: glxgears.c
	$(CC) -c -o $@ $< $(CFLAGS) -MT $< -MD -MP -MF glxgears.Tpo

clean:
	rm *.o
	rm glxgears
