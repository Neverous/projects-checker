FLAGS=${CXXFLAGS} --static

all: time.o test.o command.o judge.o main.o
	g++ ${FLAGS} -o judge time.o test.o command.o judge.o main.o

time.o: time.cpp time.hpp
	g++ ${FLAGS} -c -o time.o time.cpp

test.o: test.cpp test.hpp
	g++ ${FLAGS} -c -o test.o test.cpp

command.o: command.cpp command.hpp
	g++ ${FLAGS} -c -o command.o command.cpp

judge.o: judge.cpp judge.hpp
	g++ ${FLAGS} -c -o judge.o judge.cpp

main.o: main.cpp
	g++ ${FLAGS} -c -o main.o main.cpp

clean:
	rm *.o judge

.PHONY: all clean
