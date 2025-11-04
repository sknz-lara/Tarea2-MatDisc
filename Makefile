CC      = gcc
CFLAGS  = -std=c11 -Wall -Wextra -O2

TARGET  = castillo
OBJS    = main.o castle.o draft.o reader.o bfs.o

.PHONY: all run clean re

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

main.o: main.c castle.h draft.h reader.h bfs.h
	$(CC) $(CFLAGS) -c main.c

castle.o: castle.c castle.h
	$(CC) $(CFLAGS) -c castle.c

draft.o: draft.c draft.h castle.h
	$(CC) $(CFLAGS) -c draft.c

reader.o: reader.c reader.h
	$(CC) $(CFLAGS) -c reader.c

bfs.o: bfs.c bfs.h castle.h
	$(CC) $(CFLAGS) -c bfs.c

run: $(TARGET)
	@./$(TARGET)
	@rm -f $(OBJS)

clean:
	rm -f $(OBJS) $(TARGET)

re: clean all
