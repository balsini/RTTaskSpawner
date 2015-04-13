CC = gcc
CFLAGS = -Wall -O0 $(shell pkg-config --cflags json-c)
LDFLAGS = -lpthread -ljson $(shell pkg-config --libs json-c)
DEPS = task.hpp sched_new.h periodicity.h json_inout.h
OBJ = main.o task.o periodicity.o sched_new.o json_inout.o
TARGET = spawner

all: $(TARGET)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(OBJ)
	rm -f $(TARGET)
