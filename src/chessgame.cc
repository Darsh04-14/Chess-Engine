#include "chess.h"

void Chess::makeMove(Move move) {
  previousMoves.push_back(move);

  setCastlingRights(move);
  Colour enemyColour = Colour(colourToMove ^ ColourType);

  movePiece(move.start(), move.target());

  bool colourInd = colourToMove == White ? WHITE_IND : BLACK_IND;

  if (move.isCastle()) {
    short rankSquare = colourInd ? 56 : 0;

    if (move.flag() == LEFT_CASTLE)
      movePiece(rankSquare, rankSquare + 4);
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
      movePiece(rankSquare + 4, rankSquare);
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
}

bool Chess::playMove(string start, string target) { return false; }
bool Chess::playMove(short start, short target) { return false; }
bool Chess::playMove(short index) { return false; }

vector<Move> Chess::getLegalMoves() { return {}; }
vector<short> Chess::getBoard() { return {}; }