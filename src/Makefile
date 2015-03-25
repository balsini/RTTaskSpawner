CC = g++
LIBS = -lpthread

DEPS = task.hpp sched_new.h
OBJ = main.o task.o
TARGET = spawner

all: $(TARGET)

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	rm -f $(OBJ)
	rm -f $(TARGET)
