CXX = g++
CXXFLAGS = -std=c++17 -Wall -MMD
EXEC = chess
OBJECTS = window.o chess.o main.o movegen.o move.o human.o engine1.o engine2.o engine3.o engine4.o
DEPENDS = ${OBJECTS:.o=.d}

${EXEC}: ${OBJECTS}
	${CXX} ${CXXFLAGS} ${OBJECTS} -o ${EXEC} -I/opt/X11/include -L/opt/X11/lib -lX11

-include ${DEPENDS}
%.o: %.cc
	${CXX} ${CXXFLAGS} -I/opt/X11/include -c $< -o $@

clean:
	rm -f ${EXEC} ${OBJECTS} ${DEPENDS}
