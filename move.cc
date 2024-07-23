#include "chess.h"
using namespace std;

Chess::Move::Move() : moveData{0}, capturePromotionData{0} {}
Chess::Move::Move(int start, int target, MoveFlag flag, Piece pieceType) : Move() {
    moveData = start | (target << 6) | (flag << 12) | (pieceType << 13);
}
Chess::Move::Move(int start, int target, MoveFlag flag, int pieceType) : Move{start, target, flag, Piece(pieceType)} {}

bool Chess::Move::flag() const { return (moveData & (1 << 12)); }
short Chess::Move::piece() const { return ((moveData >> 13) & PieceType); }

short Chess::Move::start() const { return moveData & positionMask; }
short Chess::Move::target() const { return (moveData >> 6) & positionMask; }

short Chess::Move::isCapture() const { return flag() == 0 && piece() < Special ? piece() : 0; }

bool Chess::Move::isEnPassant() const { return flag() == 0 && piece() == Special; }

bool Chess::Move::isCastle() const { return flag() == 1 && piece() == Special; }

short Chess::Move::isPromotion() const { return flag() == 1 && piece() < Special ? piece() : 0; }

short Chess::Move::isCapturePromotion() const { return capturePromotionData; }

bool Chess::Move::operator==(const Move& m) const { return start() == m.start() && target() == m.target(); }