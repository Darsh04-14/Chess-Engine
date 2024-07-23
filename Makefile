CXX = g++
CXXFLAGS = -std=c++14 -Wall -MMD
EXEC = chess
OBJECTS = chess.o main.o movegen.o move.o
DEPENDS = ${OBJECTS:.o=.d}

${EXEC}: ${OBJECTS}
	${CXX} ${CXXFLAGS} ${OBJECTS} -o ${EXEC}

-include ${DEPENDS}
