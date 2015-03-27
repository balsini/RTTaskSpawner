CC = gcc
LIBS = -lpthread
CFLAGS = -Wall -O0
DEPS = task.hpp sched_new.h periodicity.h
OBJ = main.o task.o periodicity.o sched_new.o
TARGET = spawner

all: $(TARGET)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	rm -f $(OBJ)
	rm -f $(TARGET)
