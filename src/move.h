#ifndef MOVE_H
#define MOVE_H
#include "types.h"

class Move {
 public:
  static const short PieceType = (1 << 3) - 1;
  static const short positionMask = (1 << 6) - 1;
  int moveData;

  // moveData will be represented as SSSSSS TTTTTT FFF CCC PPP XXXXXXXXXXX
  // S - Starting square bits
  // T - Target Sqaure bits
  // F - Flag bits
  // C - Piece type for captures
  // P - Piece type for promotions
  // X - Discarded bits
  // * You may capture nothing, which is just a regular move

  Move();
  Move(int, int, MoveFlag = MoveFlag::CAPTURE, Piece = Piece::NoPiece);
  Move(int, int, MoveFlag, int);
  Move(int, int, Piece, Piece);
  MoveFlag flag() const;
  short start() const;
  short target() const;
  Piece capture() const;
  bool isEnPassant() const;
  bool isCastle() const;
  Piece promotion() const;
  bool isCapturePromotion() const;
  bool operator==(const Move&) const;
};

#endif
