#include "move.h"

Move::Move() : moveData{0}, capturePromotionData{0} {}
Move::Move(int start, int target, MoveFlag flag, Piece pieceType) : Move() {
    moveData = start | (target << 6) | (flag << 12) | (pieceType << 13);
}
Move::Move(int start, int target, MoveFlag flag, int pieceType) : Move{start, target, flag, Piece(pieceType)} {}

bool Move::flag() const { return (moveData & (1 << 12)); }
short Move::piece() const { return ((moveData >> 13) & PieceType); }

short Move::start() const { return moveData & positionMask; }
short Move::target() const { return (moveData >> 6) & positionMask; }

short Move::isCapture() const { return flag() == 0 && piece() < Special ? piece() : 0; }

bool Move::isEnPassant() const { return flag() == 0 && piece() == Special; }

bool Move::isCastle() const { return flag() == 1 && piece() == Special; }

short Move::isPromotion() const { return flag() == 1 && piece() < Special ? piece() : 0; }

short Move::isCapturePromotion() const { return capturePromotionData; }

bool Move::operator==(const Move& m) const { return start() == m.start() && target() == m.target(); }