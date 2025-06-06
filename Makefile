CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGET = sched
SRC = MP2.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
