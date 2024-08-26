#include "chess.h"

int Chess::countBits(ULL bitboard) {
  int cnt = 0;
  while (bitboard) {
    bitboard &= (bitboard - 1);
    ++cnt;
  }
  return cnt;
}

int Chess::lsbIndex(ULL bitboard) { return bitboard ? countBits((bitboard & -bitboard) - 1) : -1; }

Piece Chess::getPiece(char a) {
  if (!isalpha(a)) return NoPiece;
  a = tolower(a);

  if (a == 'q')
    return Queen;
  else if (a == 'k')
    return King;
  else if (a == 'b')
    return Bishop;
  else if (a == 'r')
    return Rook;
  else if (a == 'n')
    return Knight;
  else if (a == 'p')
    return Pawn;
  else
    return NoPiece;
}

void Chess::setPiece(Colour c, Piece p, int i) {
  popPiece(i);
  board[i] = c | p;
  int colourInd = colourInd(c);
  pieceBitboards[colourInd][p] |= (1ULL << i);
}

void Chess::popPiece(int i) {
  Piece p = pieceAt(board, i);
  Colour c = colourAt(board, i);
  int colourInd = colourInd(c);
  board[i] = NoPiece;
  if (p != NoPiece) pieceBitboards[colourInd][p] ^= (1ULL << i);
}

void Chess::movePiece(int start, int target) {
  popPiece(target);
  Piece p = pieceAt(board, start);
  Colour c = colourAt(board, start);
  setPiece(c, p, target);
  popPiece(start);
}

ULL Chess::colourBitboard(Colour c) {
  int colourInd = colourInd(c);
  ULL bitboard = 0;
  for (int i = 1; i < 7; ++i) bitboard |= pieceBitboards[colourInd][i];
  return bitboard;
}

void Chess::clearEdgeBits(short sq, ULL& mask) {
  if (sq / 8) mask &= ~(255ULL);
  if (sq / 8 < 7) mask &= ~(255ULL << 56);
  if (sq % 8) mask &= ~(72340172838076673ULL);
  if (sq % 8 < 7) mask &= ~(72340172838076673ULL << 7);
}

ULL Chess::getRookAttack(short sq, ULL mask) {
  clearEdgeBits(sq, mask);
  int key = (mask * rookMagics[sq]) >> (64 - rookShifts[sq]);
  return rookAttacks[sq][key];
}

ULL Chess::getBishopAttack(short sq, ULL mask) {
  clearEdgeBits(sq, mask);
  int key = (mask * bishopMagics[sq]) >> (64 - bishopShifts[sq]);
  return bishopAttacks[sq][key];
}

void Chess::getAttacks(short startSquare, ULL attacks) {
  while (attacks) {
    int attackSquare = lsbIndex(attacks);
    addMove({startSquare, attackSquare, CAPTURE, pieceAt(board, attackSquare)});
    popBit(attacks, attackSquare);
  }
}

ULL Chess::getPieceMoves(Colour c, Piece p, short square) {
  int colourInd = colourInd(c);
  if (p == King)
    return kingAttacks[square];
  else if (p == Pawn)
    return pawnAttacks[colourInd][square];
  else if (p == Knight)
    return knightAttacks[square];
  else if (p == Rook)
    return (((255UL << (square / 8 * 8)) | (72340172838076673ULL << square % 8)) ^ (1ULL << square));
  else if (p == Bishop)
    return (diagonals[square / 8 + square % 8] | rdiagonals[7 - square / 8 + square % 8]) ^ (1ULL << square);
  else
    return (((255UL << (square / 8 * 8)) | (72340172838076673ULL << square % 8) | diagonals[square / 8 + square % 8] |
             rdiagonals[7 - square / 8 + square % 8]) ^
            (1ULL << square));
}

void Chess::setCastlingRights(Move move) {
  Piece p = pieceAt(board, move.start());
  Colour c = colourAt(board, move.start());
  int colourInd = colourInd(c);

  bool leftSide = !(move.start() % 8);

  // Castling Rights - colourToMove
  if (move.isCastle() || p == King) {
    if (castlingRights[colourInd][0] == -1) castlingRights[colourInd][0] = previousMoves.size();
    if (castlingRights[colourInd][1] == -1) castlingRights[colourInd][1] = previousMoves.size();
  } else if (p == Rook && leftSide) {
    if (castlingRights[colourInd][0] == -1) castlingRights[colourInd][0] = previousMoves.size();
  } else if (p == Rook && !leftSide) {
    if (castlingRights[colourInd][1] == -1) castlingRights[colourInd][1] = previousMoves.size();
  }

  // Castling Rights - enemyColour
  Piece capturePiece = move.capture();
  leftSide = !(move.target() % 8);
  if (capturePiece == Rook && leftSide) {
    if (castlingRights[!colourInd][0] == -1) castlingRights[!colourInd][0] = previousMoves.size();
  } else if (capturePiece == Rook && !leftSide) {
    if (castlingRights[!colourInd][1] == -1) castlingRights[!colourInd][1] = previousMoves.size();
  }
}

// Attack should be an attack not a full movement mask
void Chess::setPinsAndChecks(Colour c, ULL attack, short square) {
  int colourInd = colourInd(c);
  ULL enemyKing = pieceBitboards[!colourInd][King], friendPieces = colourBitboard(c);
  if (!(enemyKing & attack) || (friendPieces & attack)) return;

  attack ^= enemyKing;
  short enemyKingSquare = lsbIndex(enemyKing);

  if (square < enemyKingSquare)
    attack &= enemyKing - 1;
  else
    attack &= ~(enemyKing - 1);

  ULL enemyPieces = colourBitboard(Colour(c ^ ColourType));

  if (!(attack & enemyPieces)) {
    if (!checks[0]) checks[0] = attack;
    if (!checks[1]) checks[1] = attack;
    return;
  }

  ULL blocker = attack & enemyPieces;

  // More than one blocker
  if (blocker & (blocker - 1)) return;

  short blockerSq = lsbIndex(blocker);
  pinnedPieces[blockerSq] = attack;
}

int Chess::perft(int depth, int debug) {
  if (!depth) {
    return 1;
  }

  // genLegalMoves();

  Move* currentlegalMoves = new Move[312];

  // short currentMoveLen = legalMovesLen;

  // cout << legalMovesLen << "\n";

  // for (int i = 0; i < legalMovesLen; ++i) {
  //   currentlegalMoves[i] = legalMoves[i];
  // }

  int count = 0;
  // for (int i = 0; i < currentMoveLen; ++i) {
  //   short s = currentlegalMoves[i].start(), t = currentlegalMoves[i].target();
  //   makeMove(currentlegalMoves[i]);
  //   int n = perft(depth - 1, debug);
  //   count += n;
  //   undoMove();
  //   if (depth == debug) {
  //     cout << char(s % 8 + 'a') << char(s / 8 + '1') << char(t % 8 + 'a') << char(t / 8 + '1') << ": " << n << "\n";
  //   }
  // }
  delete[] currentlegalMoves;
  currentlegalMoves = nullptr;

  return count;
}

vector<ULL> Chess::getRayAttacks(Piece p, short square) {
  if (p != Rook && p != Bishop && p != Queen) return {};
  ULL moves[4] = {
      ((255UL << (square / 8 * 8)) ^ (1ULL << square)),
      ((72340172838076673ULL << square % 8) ^ (1ULL << square)),
      (diagonals[square / 8 + square % 8] ^ (1ULL << square)),
      (rdiagonals[7 - square / 8 + square % 8] ^ (1ULL << square)),
  };
  short begin = (p == Rook || p == Queen) ? 0 : 2, end = (p == Bishop || p == Queen) ? 4 : 2;
  ULL half = (1ULL << square) - 1;
  vector<ULL> rayAttack;
  for (int i = begin; i < end; ++i) {
    rayAttack.push_back(moves[i] & half);
    rayAttack.push_back(moves[i] & ~half);
  }
  return rayAttack;
}

void Chess::printBitboard(ULL bitboard) {
  for (int i = 7; i >= 0; --i) {
    for (int j = 0; j < 8; ++j) cout << bool(bitboard & (1ULL << (i * 8 + j)));
    cout << "\n";
  }
}