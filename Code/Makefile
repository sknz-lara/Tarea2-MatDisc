CC      = gcc
CFLAGS  = -std=c11 -Wall -Wextra -O2

TARGET  = castillo
OBJS    = main.o castle.o draft.o

.PHONY: all run clean re

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

main.o: main.c castle.h draft.h
	$(CC) $(CFLAGS) -c main.c

castle.o: castle.c castle.h
	$(CC) $(CFLAGS) -c castle.c

draft.o: draft.c draft.h castle.h
	$(CC) $(CFLAGS) -c draft.c

run: $(TARGET)
	@./$(TARGET) 3 8 6
	@rm -f $(OBJS)

clean:
	rm -f $(OBJS) $(TARGET)

re: clean run
