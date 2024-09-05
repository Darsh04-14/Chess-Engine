#pragma once
typedef unsigned long long ULL;

// Macros for bitboards
#define setBit(bitboard, ind) ((bitboard) |= (1ULL << (ind)))
#define getBit(bitboard, ind) ((bitboard) & (1ULL << (ind)))
#define popBit(bitboard, ind) ((bitboard) ^= (1ULL << (ind)))
#define popLsb(bitboard) (bitboard &= (bitboard - 1))
#define pieceAt(board, ind) (Piece(board[ind] & PieceType))
#define colourAt(board, ind) (Colour(board[ind] & ColourType))
#define colourInd(c) (c == White ? WHITE_IND : BLACK_IND)

inline int countBits(ULL bitboard) {
  int cnt = 0;
  while (bitboard) {
    bitboard &= (bitboard - 1);
    ++cnt;
  }
  return cnt;
}

inline int lsbIndex(ULL bitboard) { return bitboard ? countBits((bitboard & -bitboard) - 1) : -1; }