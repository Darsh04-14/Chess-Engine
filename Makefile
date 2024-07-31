CXX = g++
CXXFLAGS = -std=c++17 -Wall -MMD
EXEC = chess
TEST_EXEC = chesstest
OBJECTS = window.o chess.o main.o movegen.o move.o human.o engine1.o engine2.o engine3.o engine4.o
TEST_OBJECTS = test.o chess.o movegen.o move.o
DEPENDS = ${OBJECTS:.o=.d} ${TEST_OBJECTS:.o=.d}

.PHONY: all clean test

all: ${EXEC}

${EXEC}: ${OBJECTS}
	${CXX} ${CXXFLAGS} ${OBJECTS} -o ${EXEC} -I/opt/X11/include -L/opt/X11/lib -lX11

${TEST_EXEC}: ${TEST_OBJECTS}
	${CXX} ${CXXFLAGS} ${TEST_OBJECTS} -o ${TEST_EXEC}

-include ${DEPENDS}

%.o: %.cc
	${CXX} ${CXXFLAGS} -I/opt/X11/include -c $< -o $@

clean:
	rm -f ${OBJECTS} ${TEST_OBJECTS} ${DEPENDS} ${EXEC} ${TEST_EXEC}

test: ${TEST_EXEC}
