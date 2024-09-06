#include "chess.h"

void Chess::makeMove(const Move& move) {
  previousMoves.push_back(move);
  ++ply;

  setCastlingRights(move);
  Colour enemyColour = Colour(colourToMove ^ ColourType);

  movePiece(move.start(), move.target());

  bool colourInd = colourInd(colourToMove);

  if (move.isCastle()) {
    short rankSquare = colourInd ? 56 : 0;

    if (move.flag() == LEFT_CASTLE)
      movePiece(rankSquare, rankSquare + 3);
    else
      movePiece(rankSquare + 7, rankSquare + 5);

  } else if (move.isEnPassant()) {
    short directionOffset = colourInd ? 8 : -8;
    popPiece(move.target() + directionOffset);
  }

  if (move.promotion()) {
    setPiece(colourToMove, move.promotion(), move.target());
  }

  colourToMove = enemyColour;
}

void Chess::undoMove() {
  if (!previousMoves.size()) return;

  Move lastMove = previousMoves.back();

  Colour prevColour = colourAt(board, lastMove.target());
  bool colourInd = prevColour == White ? WHITE_IND : BLACK_IND;

  movePiece(lastMove.target(), lastMove.start());

  if (lastMove.isCastle()) {
    short rankSquare = colourInd ? 56 : 0;

    if (lastMove.flag() == LEFT_CASTLE)
      movePiece(rankSquare + 3, rankSquare);
    else
      movePiece(rankSquare + 5, rankSquare + 7);
  } else if (lastMove.isEnPassant()) {
    short offset = (lastMove.target() % 8) - (lastMove.start() % 8);
    setPiece(colourToMove, Pawn, lastMove.start() + offset);

  } else if (lastMove.capture()) {
    setPiece(colourToMove, lastMove.capture(), lastMove.target());
  }

  if (lastMove.promotion()) {
    setPiece(prevColour, Pawn, lastMove.start());
  }

  if (castlingRights[0][0] == previousMoves.size()) castlingRights[0][0] = -1;
  if (castlingRights[0][1] == previousMoves.size()) castlingRights[0][1] = -1;
  if (castlingRights[1][0] == previousMoves.size()) castlingRights[1][0] = -1;
  if (castlingRights[1][1] == previousMoves.size()) castlingRights[1][1] = -1;

  previousMoves.pop_back();
  --ply;
  colourToMove = prevColour;
  if (gameState) gameState = 0;
}

bool Chess::playMove(string start, string target) {
  if (start.length() > 2 || target.length() > 2) return false;
  short startSquare = (start[1] - '1') * 8 + start[0] - 'a';
  short targetSquare = (target[1] - '1') * 8 + target[0] - 'a';
  if (startSquare < 0 || startSquare > 63) return false;
  if (targetSquare < 0 || targetSquare > 63) return false;

  return playMove(startSquare, targetSquare);
}
bool Chess::playMove(short startIndex, short targetIndex) {
  for (int i = 0; i < legalMovesLen; ++i) {
    Move move = legalMoves[i];
    if (move.start() == startIndex && move.target() == targetIndex) {
      if (move.promotion()) {
        cout << "Enter which piece you want to promote to (q,n,b,r): ";
        char p;
        do {
          cin >> p;
          p = tolower(p);
        } while (p != 'q' && p != 'n' && p != 'b' && p != 'r');
        Piece newPiece = getPiece(p);
        if (move.promotion()) {
          makeMove({startIndex, targetIndex, MoveFlag::PROMOTION, newPiece});
        }
      } else {
        makeMove(move);
      }
      genLegalMoves();
      return true;
    }
  }
  return false;
}
bool Chess::playMove(short index) { return false; }

Colour Chess::getCurrentPlayer() { return colourToMove; }
bool Chess::isSquareAttacked(Colour c, short square) { return getBit(attackBitboards[colourInd(c)], square); }

const Move* Chess::getLegalMoves() { return legalMoves; }
short Chess::getLegalMovesLen() { return legalMovesLen; }

const short* Chess::getBoard() { return board; }

bool Chess::sufficientMaterial(Colour c) {
  int colourInd = colourInd(c);
  if (pieceBitboards[colourInd][Pawn] || pieceBitboards[colourInd][Queen] || pieceBitboards[colourInd][Rook]) {
    return true;
  } else if (pieceBitboards[colourInd][Knight] && pieceBitboards[colourInd][Bishop]) {
    return true;
  } else if (countBits(pieceBitboards[colourInd][Knight]) > 2) {
    return true;
  } else if (pieceBitboards[colourInd][Bishop]) {
    ULL bitboard = pieceBitboards[colourInd][Bishop];
    bool squareColour = lsbIndex(bitboard) % 2;
    while (bitboard) {
      short square = lsbIndex(bitboard);
      if (square % 2 != squareColour) {
        return true;
      }
      popBit(bitboard, square);
    }
  }

  return false;
}

void Chess::setGameState() {
  Colour enemyColour = Colour(colourToMove ^ ColourType);

  bool sufficientMaterialFriend = sufficientMaterial(colourToMove),
       sufficientMaterialEnemy = sufficientMaterial(enemyColour);

  if (!sufficientMaterialFriend && !sufficientMaterialEnemy) {
    gameState = White | Black;
    return;
  }

  if (legalMovesLen) {
    if (checks) gameState = colourToMove >> 2;
  } else if (checks) {
    gameState = enemyColour;
  } else {
    gameState = White | Black;
  }
}

short Chess::check() { return (gameState & 6) ? (gameState << 1) : 0; }
short Chess::end() { return (gameState & 24); }
bool Chess::draw() { return gameState = (White | Black); }
