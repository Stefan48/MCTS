CC = mpic++
CFLAGS = -Wall -Wextra -O0

.PHONY: build
build: main

main: main.o position.o board.o node.o mcts.o
	$(CC) $(CFLAGS) $^ -o $@
	
main.o: main.cpp
	$(CC) $(CFLAGS) -c $< -o $@
	
position.o: position.cpp
	$(CC) $(CFLAGS) -c $< -o $@
	
board.o: board.cpp
	$(CC) $(CFLAGS) -c $< -o $@
	
node.o: node.cpp
	$(CC) $(CFLAGS) -c $< -o $@
	
mcts.o: mcts.cpp
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: run	
run:
	mpirun -np 4 main

.PHONY: clean

clean:
	rm -f *.o main
