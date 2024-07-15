#define CHESS_H

#include <iostream>
#include <vector>
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

    const int Colour = (3 << 7);

    // Piece bits breakdown
    // [0:6] indicate piece type
    // [7:8] indicate White or Black
    // [9:32] random

    // Used to check for pins/checks
    unsigned long long diagonalMasks[16];
    unsigned long long rdiagonalMasks[16];
    unsigned long long horizontalMasks[8];
    unsigned long long verticalMasks[8];

    // Bitboards used to track pieces of a certain colour
    // Represent pieces in this order: King, Queen, Knight, Bishop, Rook, Pawn
    unsigned long long whiteBoards[6] = {0};
    unsigned long long blackBoards[6] = {0};
    int bitboardIndex[6] = {King, Queen, Knight, Bishop, Rook, Pawn};

    struct Move {
        int start;      // start square
        int target;     // final square
        int isCapture;  // see if opposite colour peice available for capture
    };

    int board[8][8];
    int colorToMove;

    vector<Move> validMoves;     // valid moves from start to end square 0 - 63
    vector<Move> previousMoves;  // Keeps track of previous moves for undo move function + checking for en passant

    void kingMoves(int);
    void pawnMove(int);  // parameter is start of square where pawn is
    void knightMove(int);
    void castleMove();
    void pawnPromotions(int);
    bool isCheck(int);

    void diagonalMove(int);
    void horizontalVerticalMoves(int);
    void generateMoves();

    int moveIndex(string);
    int getPiece(char);

    void generateMasks();

    bool isSlidingPiece(int);

    // Used to get all squares covered by a sliding piece
    unsigned long long getXRayAttackOnKing(int, int);

    // Get all the masks of all attacks on king
    unsigned long long getPinnedPositions();

    unsigned long long &bitBoard(int, int);

   public:
    Chess();
    Chess(string);

    bool playMove(string, string);  // start and end for entering
    void undoMove();

    void print();
};
