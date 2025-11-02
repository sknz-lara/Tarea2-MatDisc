CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -O2

OBJS = main.o castle.o
TARGET = castillo

run: $(TARGET)
	@./$(TARGET) 3 8 6
	@rm -f $(OBJS)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

main.o: main.c castle.h
	$(CC) $(CFLAGS) -c main.c

castle.o: castle.c castle.h
	$(CC) $(CFLAGS) -c castle.c

clean:
	rm -f $(OBJS) $(TARGET)

re: clean run
