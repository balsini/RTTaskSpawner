CC = gcc
CFLAGS = -Wall -O0
LDFLAGS = -I../
DEPS = andom_task_generator.h ../periodicity.h
OBJ = main.o random_task_generator.o ../periodicity.o
TARGET = task_config_generator

all: $(TARGET)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(OBJ)
	rm -f $(TARGET)
