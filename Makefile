CC = g++
CFLAGS = -Wall
PROG = Assignment2_201102073

SRCS = Assignment2_201102073.cpp
LIBS = -lglut -lGL -lGLU -w

all: $(PROG)

$(PROG):	$(SRCS)
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS)

clean:
	rm -f $(PROG)
