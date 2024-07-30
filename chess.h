#ifndef CHESS_H
#define CHESS_H

#include <cstring>
#include <iostream>
#include <vector>
using namespace std;

#include "game.h"
#include "move.h"

typedef unsigned long long ULL;

static const short ColourType = (3 << 3);
static const short PieceType = (1 << 3) - 1;

class Chess : public Game {
    static const short NUM_ROWS = 8;
    static const short NUM_COLS = 8;

    Move legalMoves[250];  // Max number of pseudolegal moves in any position
    short legalMovesLen;

    // Keep track of previous moves for undo functionality
    vector<Move> previousMoves;

    // Used to keep track of castling rights for each player
    short castlingRights[2][2];

    // Boolean value to determine if we are generating legal moves or attack masks
    bool getAttackMasks;

    Colour colourToMove;
    // Takes Values: 0,2,4,8,16,24
    // Correspond to game states: Game Not Over, White Check, Black Check, White Win, Black Win, Draw
    int gameState;

    // A piece will be represented as CCPPP
    // C - Colour bits
    // P - Piece bits
    short board[64];

    // Used to keep track of squares that each colour is able to capture or move to
    ULL attackMask[2];

    Piece getPiece(char);

    void getAttackSquares();

    // Generates legal moves
    void generatePseudoLegalMoves();
    void genCastleMove();
    void genKingMoves(short);
    void genPawnMoves(short);
    void genKnightMoves(short);
    void genSlidingMoves(short);

    // Responsible for adding moves to vector, updating captureMask state, and filtering out any illegal moves
    void addMove(const Move&);

    int getPosition(string);

   public:
    Chess();
    Chess(string);
    void generateLegalMoves();
    bool isSquareAttacked(short, short);
    bool playMove(short, short) override;
    bool playMove(string, string) override;
    Colour getCurrentPlayer();
    void makeMove(const Move&);
    void unmakeMove();
    void print();
    void printLegalMoves();
    void printCastleRights();
    void setGameState();
    void printGameState();
    int end();
    int check();
    short getKing(Colour);
    int perft(int, int = 0);
    void resignPlayer();
    void clear();
    bool addPiece(char, string);
    bool removePiece(string);
    bool validBoard();
    void setColour(string);
    void resign();
    void draw();
    vector<Move> getLegalMoves() override;
    vector<short> getBoard() override;
    Piece getPieceAt(int row, int col) const;  // New method
    short getEmpty() const;                    // New method
    short getBlack() const;                    // New method
};

#endif