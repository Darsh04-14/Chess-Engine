#pragma once
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
#define pieceNum(board, ind) ((pieceAt(board, ind) - 1) + 6 * (colourAt(board, ind) >> 4))

inline int countBits(ULL bitboard) {
  int cnt = 0;
  while (bitboard) {
    bitboard &= (bitboard - 1);
    ++cnt;
  }
  return cnt;
}

inline int lsbIndex(ULL bitboard) { return bitboard ? countBits((bitboard & -bitboard) - 1) : -1; }