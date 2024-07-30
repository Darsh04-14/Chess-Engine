#ifndef MOVE_H
#define MOVE_H
#include "types.h"

class Move {
   public:
    static const short PieceType = (1 << 3) - 1;
    static const short positionMask = (1 << 6) - 1;
    short moveData;
    short capturePromotionData;
    // moveData will be represented as SSSSSS TTTTTT F PPP
    // S - Starting square bits
    // T - Target Sqaure bits
    // F - Flag bits, capture -> 0, promotion -> 1
    // P - Piece type for captures or promotions
    // Special Cases: En Passant and Castling use FPPP bits together, with values 0111 and 1111 respectively
    // * You may capture nothing, which is just a regular move
    // * The capture promotion variable stores promoted piece data when a pawn capture leads to a promotion

    Move();
    Move(int, int, MoveFlag = MoveFlag::CAPTURE, Piece = Piece::NoPiece);
    Move(int, int, MoveFlag, int);
    bool flag() const;
    short piece() const;
    short start() const;
    short target() const;
    short isCapture() const;
    bool isEnPassant() const;
    bool isCastle() const;
    short isPromotion() const;
    short isCapturePromotion() const;
    bool operator==(const Move&) const;
};

#endif