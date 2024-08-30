#include "chess.h"

void Chess::makeMove(const Move& move) {
  previousMoves.push_back(move);

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

  for (int i = 0; i < 2; ++i) {
    if (castlingRights[i][0] == previousMoves.size()) castlingRights[i][0] = -1;
    if (castlingRights[i][1] == previousMoves.size()) castlingRights[i][1] = -1;
  }

  previousMoves.pop_back();
  colourToMove = prevColour;
  if (gameState) gameState = 0;
}

bool Chess::playMove(string start, string target) { return false; }
bool Chess::playMove(short start, short target) { return false; }
bool Chess::playMove(short index) {
  if (index < 0 || index >= legalMovesLen) return false;

  makeMove(legalMoves[index]);
  return true;
}

Colour Chess::getCurrentPlayer() { return colourToMove; }
bool Chess::isSquareAttacked(Colour c, short square) { return getBit(pieceBitboards[colourInd(c)][0], square); }

vector<Move> Chess::getLegalMoves() { return vector<Move>(legalMoves, legalMoves + legalMovesLen); }
const short* Chess::getBoard() { return board; }

bool Chess::sufficientMaterial(Colour c) {
  int colourInd = colourInd(c);
  bool sufficientMaterial = false;
  if (pieceBitboards[colourInd][Pawn] || pieceBitboards[colourInd][Queen] || pieceBitboards[colourInd][Rook]) {
    sufficientMaterial = true;
  } else if (pieceBitboards[colourInd][Knight] && pieceBitboards[colourInd][Bishop]) {
    sufficientMaterial = true;
  } else if (countBits(pieceBitboards[colourInd][Knight]) > 2) {
    sufficientMaterial = true;
  } else if (pieceBitboards[colourInd][Bishop]) {
    ULL bitboard = pieceBitboards[colourInd][Bishop];
    bool squareColour = lsbIndex(bitboard) % 2;
    while (bitboard) {
      short square = lsbIndex(bitboard);
      if (square % 2 != squareColour) {
        sufficientMaterial = true;
        break;
      }
      popBit(bitboard, square);
    }
  }

  return sufficientMaterial;
}

void Chess::setGameState() {
  int colourInd = colourInd(colourToMove);
  short kingIndex = lsbIndex(pieceBitboards[colourInd][King]);
  Colour enemyColour = Colour(colourToMove ^ ColourType);

  bool sufficientMaterialFriend = sufficientMaterial(colourToMove),
       sufficientMaterialEnemy = sufficientMaterial(enemyColour);

  if (!sufficientMaterialFriend && !sufficientMaterialEnemy) {
    gameState = White | Black;
    return;
  }

  if (legalMovesLen) {
    if (isSquareAttacked(enemyColour, kingIndex)) gameState = colourToMove >> 2;
  } else if (isSquareAttacked(enemyColour, kingIndex)) {
    gameState = enemyColour;
  } else {
    gameState = White | Black;
  }
}

short Chess::check() { return (gameState & 6) ? (gameState << 1) : 0; }
short Chess::end() { return (gameState & 24) ? gameState : 0; }
bool Chess::draw() { return gameState = (White | Black); }
