#define CHESS_H

#include <vector>
#include <iostream>
using namespace std;



class Chess {
    const int None = 0;
    const int King = 75;
    const int Queen = 81;
    const int Knight = 78;
    const int Bishop = 66;
    const int Rook = 82;
    const int Pawn = 80;

    const int White = (1 << 7);
    const int Black = (1 << 8);

    int board[8][8];
public:
    Chess();
    Chess(string inputed);
    void print();
};
