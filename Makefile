# Makefile for MyThreads project

CC = gcc
CFLAGS = -Wall -g
TARGET = mythreads
SRC = Mythreads.c

all: $(TARGET)

$(TARGET): $(SRC)
    $(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
    rm -f $(TARGET)