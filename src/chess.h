#ifndef CHESS_H
#define CHESS_H

#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>

#include "game.h"
#include "move.h"

typedef unsigned long long ULL;

static const short ColourType = (3 << 3);
static const short PieceType = (1 << 3) - 1;

static const bool WHITE_IND = White >> 4;
static const bool BLACK_IND = Black >> 4;

// Macros for bitboards
#define setBit(bitboard, ind) ((bitboard) |= (1ULL << (ind)))
#define getBit(bitboard, ind) ((bitboard) & (1ULL << (ind)))
#define popBit(bitboard, ind) ((bitboard) ^= (1ULL << (ind)))
#define popLsb(bitboard) (bitboard &= (bitboard - 1))
#define pieceAt(board, ind) (Piece(board[ind] & PieceType))
#define colourAt(board, ind) (Colour(board[ind] & ColourType))
#define colourInd(c) (c == White ? WHITE_IND : BLACK_IND)

class Chess : public Game {
  static const short NUM_ROWS = 8;
  static const short NUM_COLS = 8;

  const ULL rookShifts[64] = {
      12, 11, 11, 11, 11, 11, 11, 12, 11, 10, 10, 10, 10, 10, 10, 11, 11, 10, 10, 10, 10, 10,
      10, 11, 11, 10, 10, 10, 10, 10, 10, 11, 11, 10, 10, 10, 10, 10, 10, 11, 11, 10, 10, 10,
      10, 10, 10, 11, 11, 10, 10, 10, 10, 10, 10, 11, 12, 11, 11, 11, 11, 11, 11, 12,
  };
  const ULL bishopShifts[64] = {
      6, 5, 5, 5, 5, 5, 5, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 7, 7, 7, 7, 5, 5, 5, 5, 7, 9, 9, 7, 5, 5,
      5, 5, 7, 9, 9, 7, 5, 5, 5, 5, 7, 7, 7, 7, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 5, 5, 5, 5, 5, 5, 6,
  };
  const ULL diagonals[15] = {
      0x000000000000001, 0x000000000000102,  0x000000000010204,  0x000000001020408,  0x000000102040810,
      0x000010204081020, 0x001020408102040,  0x102040810204080,  0x204081020408000,  0x408102040800000,
      0x810204080000000, 0x1020408000000000, 0x2040800000000000, 0x4080000000000000, 0x8000000000000000,
  };
  const ULL rdiagonals[15] = {
      0x0100000000000000, 0x201000000000000,  0x0402010000000000, 0x0804020100000000, 0x1008040201000000,
      0x2010080402010000, 0x4020100804020100, 0x8040201008040201, 0x0080402010080402, 0x0000804020100804,
      0x0000008040201008, 0x0000000080402010, 0x0000000000804020, 0x0000000000008040, 0x0000000000000080,
  };

  ULL rookMagics[64];
  ULL bishopMagics[64];

 public:
  Colour colourToMove;

  // Takes Values: 0,2,4,8,16,24
  // Correspond to game states: Game Not Over, White in Check, Black in Check, White Win, Black Win, Draw
  short gameState;

  // Max number of legal moves in any position
  Move legalMoves[218];
  short legalMovesLen;

  inline void addMove(Move);

  // Keep track of previous moves for undo functionality
  std::vector<Move> previousMoves;

  // Used to keep track of castling rights for each player
  short castlingRights[2][2];

  short board[64];
  ULL pieceBitboards[2][7];
  ULL attackBitboards[2];
  ULL pinRays;
  bool doubleCheck;
  ULL checks;
  bool enPassantPin;

  inline void genCastleMove();
  inline void genKingMoves();
  inline void genPawnMoves();
  inline void genKnightMoves();
  inline void genRookMoves();
  inline void genBishopMoves();
  inline void genQueenMoves();

  // Fills attack bitboards and pinnedPieces
  inline void genAttacks(Colour);

  ULL kingAttacks[64];
  ULL knightAttacks[64];
  ULL pawnAttacks[2][64];
  ULL bishopAttacks[64][512];
  ULL rookAttacks[64][4096];
  short directionOffset[64];

  // Initialize attack tables for different pieces
  void pawnAttackTable();
  void knightAttackTable();
  void bishopAttackTable();
  void rookAttackTable();
  void kingAttackTable();
  void directionOffsetTable();

  // Utility Functions
  inline int countBits(ULL);
  inline int lsbIndex(ULL);
  inline void setPiece(Colour, Piece, int);
  inline void popPiece(int);
  inline void movePiece(int, int);
  inline short getDirection(short, short);
  Piece getPiece(char);
  char getChar(Piece);
  void clearEdgeBits(short, ULL&);
  ULL getRookAttack(short, ULL);
  ULL getBishopAttack(short, ULL);
  inline void getAttacks(short, ULL);
  ULL getPieceMoves(Colour, Piece, short);
  void setCastlingRights(Move);
  void setPinsAndChecks(Colour, ULL, short);
  bool sufficientMaterial(Colour);
  void printBitboard(ULL);  // Debugging

  Chess();
  Chess(string);

  bool playMove(short, short) override;
  bool playMove(string, string) override;
  bool playMove(short) override;

  const Move* getLegalMoves() override;
  short getLegalMovesLen();
  const short* getBoard() override;

  bool isSquareAttacked(Colour c, short);
  Colour getCurrentPlayer();

  void setGameState();
  short end();
  short check();
  bool draw();

  int perft(int, int = 0);  // For testing purposes
  void print();

  // Internal game functions
  void makeMove(const Move&);
  void undoMove();

  void genLegalMoves();

  ~Chess();
};

// Inline function definitions

inline void Chess::getAttacks(short startSquare, ULL attacks) {
  while (attacks) {
    int attackSquare = lsbIndex(attacks);
    addMove({startSquare, attackSquare, CAPTURE, pieceAt(board, attackSquare)});
    popLsb(attacks);
  }
}

inline int Chess::countBits(ULL bitboard) {
  int cnt = 0;
  while (bitboard) {
    bitboard &= (bitboard - 1);
    ++cnt;
  }
  return cnt;
}

inline int Chess::lsbIndex(ULL bitboard) { return bitboard ? countBits((bitboard & -bitboard) - 1) : -1; }

inline void Chess::addMove(Move m) {
  if (getBit(pinRays, m.start())) {
    short kingInd = lsbIndex(pieceBitboards[colourInd(colourToMove)][King]);
    if (getDirection(m.start(), kingInd) != getDirection(m.start(), m.target())) return;
  }
  if (m.isEnPassant() && enPassantPin) return;

  short offset = colourToMove == White ? -8 : 8;
  short targetSquare = m.target() + (m.isEnPassant() ? offset : 0);
  if (checks && pieceAt(board, m.start()) != King && !getBit(checks, targetSquare)) return;

  legalMoves[legalMovesLen++] = m;
}

inline void Chess::setPiece(Colour c, Piece p, int i) {
  if (p == NoPiece) return;
  popPiece(i);
  board[i] = c | p;
  int colourInd = colourInd(c);
  setBit(pieceBitboards[colourInd][p], i);
  setBit(pieceBitboards[colourInd][0], i);
}

inline void Chess::popPiece(int i) {
  Piece p = pieceAt(board, i);
  if (p == NoPiece) return;
  Colour c = colourAt(board, i);
  int colourInd = colourInd(c);
  board[i] = NoPiece;
  popBit(pieceBitboards[colourInd][p], i);
  popBit(pieceBitboards[colourInd][0], i);
}

inline void Chess::movePiece(int start, int target) {
  popPiece(target);
  Piece p = pieceAt(board, start);
  Colour c = colourAt(board, start);
  setPiece(c, p, target);
  popPiece(start);
}

inline ULL Chess::getRookAttack(short sq, ULL mask) {
  clearEdgeBits(sq, mask);
  int key = (mask * rookMagics[sq]) >> (64 - rookShifts[sq]);
  return rookAttacks[sq][key];
}

inline ULL Chess::getBishopAttack(short sq, ULL mask) {
  clearEdgeBits(sq, mask);
  int key = (mask * bishopMagics[sq]) >> (64 - bishopShifts[sq]);
  return bishopAttacks[sq][key];
}

inline short Chess::getDirection(short start, short target) { return directionOffset[abs(target - start)]; }

#endif
