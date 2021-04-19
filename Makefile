SRCS = $(wildcard src/*.c)
OBJS = $(SRCS:c=o)
EXE = uart_ledctl.out
CC = gcc
CC := $(CROSS_COMPILE)$(CC)

CFLAGS = -g -Wall

.PHONY: all build run clean

all: build

build: $(EXE)

run: $(EXE)
	./$(EXE)

clean:
	rm -rf $(OBJS) $(EXE)

$(EXE): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@
