CC = g++
CFLAGS = -Wall -g

all: ./calculator.exe
calculator: ./calculator.exe

./calculator.exe: ./src/calculator.o
	$(CC) $(CFLAGS) -o calculator.exe src/calculator.o

./src/%.o: ./src/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f ./src/*.o ./calculator.exe
