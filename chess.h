#ifndef CHESS_H
#define CHESS_H

#include <vector>
#include <iostream>
using namespace std;

class Chess {
    const int Empty = 0; // Renamed from None to avoid conflict
    const int King = 75;
    const int Queen = 81;
    const int Knight = 78;
    const int Bishop = 66;
    const int Rook = 82;
    const int Pawn = 80;

    const int White = (1 << 7);
    const int Black = (1 << 8);

    struct Move {
        int start;
        int target;
        int isCapture;
    };

    int board[8][8];
    int colorToMove;

    vector<Move> validMoves;

    void kingMoves(int);
    void pawnMove(int);
    void knightMove(int);
    void castleMove();
    void pawnPromotions(int);
    bool isCheck(int);

    void diagonalMove(int);
    void horizontalVerticalMoves(int);
    void generateMoves();

    int moveIndex(string);

public:
    Chess();
    Chess(string);

    bool playMove(string, string);

    void print();

    // Accessor methods
    int getPiece(int row, int col) const;
    int getEmpty() const;
    int getBlack() const;
    int getWhite() const;
};

#endif
