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

    struct Move {
        int start; // start square
        int target; // final square
        int isCapture; // see if opposite coloure peice available for capture
    };

    int board[8][8];
    int colorToMove;

    vector<Move> validMoves;// valid moves from start to end square 0 - 63

    void kingMoves(int);
    void pawnMove(int); // parameter is start of square where pawn is 
    void knightMove(int);
    void castleMove();

    void diagonalMove(int);
    void horizontalVerticalMoves(int);
    void generateMoves();

    int moveIndex(string);

public:
    Chess();
    Chess(string);

    bool playMove(string, string);// start and end for entering

    void print();
};
