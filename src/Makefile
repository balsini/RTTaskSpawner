CC = gcc

CFLAGS = -g -Wall -O0 #$(shell pkg-config --cflags json-c)
LDFLAGS = -lpthread -ljson $(shell pkg-config --libs json-c)

DEPS = task.h sched_new.h periodicity.h json_inout.h
OBJ = task.o periodicity.o sched_new.o json_inout.o
TARGET_OBJ = main.o $(OBJ)
TEST_OBJ = main_test.o $(OBJ)

TARGET = spawner 
TEST = spawner_test 

all: $(TARGET)

test: $(TEST)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET): $(TARGET_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

$(TEST): $(TEST_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) -lcunit

clean:
	rm -f $(OBJ)
	rm -f $(TARGET)
	rm -f $(TEST)
