#include "chess.h"

void Chess::addMove(Move m) {
  if (pinnedPieces[m.start()] && !(pinnedPieces[m.start()] & (1ULL << m.target()))) return;

  legalMoves[legalMovesLen++] = m;
}

void Chess::genAttacks(Colour c) {
  int colourInd = c == White ? WHITE_IND : BLACK_IND;

  attackBitboards[colourInd] = 0;
  memset(pinnedPieces, 0, sizeof(pinnedPieces));

  for (int i = 1; i <= 3; ++i) {
    ULL bitboard = pieceBitboards[colourInd][i];
    while (bitboard) {
      short square = lsbIndex(bitboard);
      attackBitboards[colourInd] |= getPieceAttack(c, Piece(i), square);
      popBit(bitboard, square);
    }
  }

  // Sliding pieces
  ULL enemyPieces = colourBitboard(Colour(c ^ ColourType)), friendPieces = colourBitboard(c);
  for (int i = 4; i <= 6; ++i) {
    ULL bitboard = pieceBitboards[colourInd][i];
    while (bitboard) {
      short square = lsbIndex(bitboard);
      ULL mask = getPieceAttack(c, Piece(i), square);
      bool attacksKing = mask & (pieceBitboards[!colourInd][King]);

      if (attacksKing) {
        short kingSquare = lsbIndex(pieceBitboards[!colourInd][King]);
        short row = square / 8, col = square % 8;
        ULL rayAttack = 0;
        ULL half = square < kingSquare ? ~((1ULL << square) - 1) : ((1ULL << square) - 1);
        if (col == kingSquare % 8 && (i == Rook || i == Queen))
          rayAttack = (72340172838076673ULL << col) & half;
        else if (row == kingSquare / 8 && (i == Rook || i == Queen))
          rayAttack = (255ULL << (row * 8)) & half;
        else if ((1ULL << kingSquare) & diagonals[row + col])
          rayAttack = diagonals[row + col] & half;
        else
          rayAttack = rdiagonals[7 - row + col] & half;

        rayAttack ^= (1ULL << kingSquare);

        ULL rayPieces = rayAttack & enemyPieces;
        if (!(rayAttack & friendPieces) && rayPieces && !(rayPieces & (rayPieces - 1)))
          pinnedPieces[lsbIndex(rayPieces)] = rayPieces;
      }

      mask ^= mask & (enemyPieces | friendPieces);
      if (i == Bishop) attackBitboards[colourInd] |= getBishopAttack(square, mask);
      if (i == Rook) attackBitboards[colourInd] |= getRookAttack(square, mask);
      if (i == Queen) {
        ULL rookMask = getPieceAttack(c, Rook, square), bishopMask = getPieceAttack(c, Bishop, square);
        rookMask ^= (friendPieces | enemyPieces) & rookMask;
        bishopMask ^= (friendPieces | enemyPieces) & bishopMask;
        attackBitboards[colourInd] |= getBishopAttack(square, bishopMask);
        attackBitboards[colourInd] |= getRookAttack(square, rookMask);
      }

      popBit(bitboard, square);
    }
  }
}

void Chess::genLegalMoves() {
  genAttacks(Colour(ColourType ^ colourToMove));

  legalMovesLen = 0;
  genKingMoves();
  genPawnMoves();
  genKnightMoves();
  genBishopMoves();
  genRookMoves();
  genQueenMoves();
  genCastleMove();
}

void Chess::genCastleMove() {
  int colourInd = colourToMove == White ? WHITE_IND : BLACK_IND;
  short leftCastle = castlingRights[colourInd][0];
  short rightCastle = castlingRights[colourInd][1];

  if (leftCastle != -1 && rightCastle != -1) return;

  short kingIndex = (colourInd ? 56 : 0) + 4;

  bool canCastle = true;
  ULL blockers = colourBitboard(White) | colourBitboard(Black);
  ULL enemyAttacks = attackBitboards[!colourInd];
  if (leftCastle == -1) {
    for (int i = 3; i > 0; --i)
      if (getBit(blockers, kingIndex - i)) canCastle = false;

    for (int i = 2; i >= 0; --i)
      if (getBit(enemyAttacks, kingIndex - i)) canCastle = false;

    if (canCastle) addMove({kingIndex, kingIndex - 2, LEFT_CASTLE});
  }
  canCastle = true;
  if (rightCastle == -1) {
    for (int i = 2; i > 0; --i)
      if (getBit(blockers, kingIndex + i)) canCastle = false;

    for (int i = 2; i >= 0; --i)
      if (getBit(enemyAttacks, kingIndex + i)) canCastle = false;

    if (canCastle) addMove({kingIndex, kingIndex + 2, RIGHT_CASTLE});
  }
}

void Chess::genKingMoves() {
  int colourInd = colourToMove == White ? WHITE_IND : BLACK_IND;

  ULL friendBitboard = colourBitboard(colourToMove);
  ULL bitboard = pieceBitboards[colourInd][King];

  while (bitboard) {
    int startSquare = lsbIndex(bitboard);

    ULL attacks = kingAttacks[startSquare];
    attacks ^= friendBitboard;
    attacks ^= attackBitboards[!colourInd];

    getAttacks(startSquare, attacks);

    popBit(bitboard, startSquare);
  }
}

void Chess::genPawnMoves() {
  int colourInd = colourToMove == White ? WHITE_IND : BLACK_IND;
  int offsets[2][4] = {{8, 16, 7, 9}, {-8, -16, -9, -7}};

  int startingRank = colourInd ? 6 : 1;
  int promotionRank = colourInd ? 0 : 7;
  ULL bitboard = pieceBitboards[colourInd][Pawn];
  ULL whiteBitboard = colourBitboard(White), blackBitboard = colourBitboard(Black);

  while (bitboard) {
    int square = lsbIndex(bitboard);
    // Single Push
    int pushSquare = square + offsets[colourInd][0];
    if (!((whiteBitboard | blackBitboard) & (1ULL << pushSquare))) {
      if (pushSquare / 8 == promotionRank)
        for (int i = 3; i <= 6; ++i) addMove({square, pushSquare, PROMOTION, Piece(i)});
      else
        addMove({square, pushSquare, CAPTURE, NoPiece});

      // Double push
      int doublePushSquare = square + offsets[colourInd][1];
      if (!((whiteBitboard | blackBitboard) & (1ULL << doublePushSquare)) && square / 8 == startingRank)
        addMove({square, doublePushSquare, CAPTURE, NoPiece});
    }

    // Captures
    ULL attackBitboard = pawnAttacks[colourInd][square] & (colourInd ? whiteBitboard : blackBitboard);
    while (attackBitboard) {
      int attackSquare = lsbIndex(attackBitboard);

      if (attackSquare / 8 == promotionRank)
        for (int i = 3; i <= 6; ++i) addMove({square, pushSquare, pieceAt(board, attackSquare), Piece(i)});
      else
        addMove({square, pushSquare, CAPTURE, pieceAt(board, attackSquare)});

      popBit(attackBitboard, attackSquare);
    }

    // En Passant
    if (previousMoves.size() && previousMoves.back().flag() == DOUBLE_PAWN_PUSH &&
        previousMoves.back().target() / 8 == square / 8) {
      if (previousMoves.back().target() == square - 1)
        addMove({square, square + offsets[colourInd][2], ENPASSANT, Pawn});
      if (previousMoves.back().target() == square + 1)
        addMove({square, square + offsets[colourInd][3], ENPASSANT, Pawn});
    }

    popBit(bitboard, square);
  }
}

void Chess::genKnightMoves() {
  int colourInd = colourToMove == White ? WHITE_IND : BLACK_IND;

  ULL friendBitboard = colourBitboard(colourToMove);
  ULL bitboard = pieceBitboards[colourInd][Knight];

  while (bitboard) {
    int startSquare = lsbIndex(bitboard);

    ULL attacks = knightAttacks[startSquare];
    attacks ^= friendBitboard;

    getAttacks(startSquare, attacks);

    popBit(bitboard, startSquare);
  }
}

void Chess::genRookMoves() {
  int colourInd = colourToMove == White ? WHITE_IND : BLACK_IND;

  ULL friendBitboard = colourBitboard(colourToMove), enemyBitboard = colourBitboard(Colour(colourToMove ^ ColourType));
  ULL bitboard = pieceBitboards[colourInd][Rook];

  while (bitboard) {
    short startSquare = lsbIndex(bitboard);

    ULL mask = getPieceAttack(colourToMove, Rook, startSquare);
    mask ^= (friendBitboard | enemyBitboard) & mask;

    ULL attacks = getRookAttack(startSquare, mask);
    attacks ^= friendBitboard;

    getAttacks(startSquare, attacks);

    popBit(bitboard, startSquare);
  }
}

void Chess::genBishopMoves() {
  int colourInd = colourToMove == White ? WHITE_IND : BLACK_IND;

  ULL friendBitboard = colourBitboard(colourToMove), enemyBitboard = colourBitboard(Colour(colourToMove ^ ColourType));
  ULL bitboard = pieceBitboards[colourInd][Bishop];

  while (bitboard) {
    int startSquare = lsbIndex(bitboard);
    int row = startSquare / 8, col = startSquare % 8;

    ULL mask = diagonals[row + col] | rdiagonals[7 - row + col];
    mask ^= (friendBitboard | enemyBitboard) & mask;

    ULL attacks = getBishopAttack(startSquare, mask);
    attacks ^= friendBitboard;

    getAttacks(startSquare, attacks);

    popBit(bitboard, startSquare);
  }
}

void Chess::genQueenMoves() {
  int colourInd = colourToMove == White ? WHITE_IND : BLACK_IND;

  ULL friendBitboard = colourBitboard(colourToMove), enemyBitboard = colourBitboard(Colour(colourToMove ^ ColourType));
  ULL bitboard = pieceBitboards[colourInd][Queen];

  while (bitboard) {
    int startSquare = lsbIndex(bitboard);
    int row = startSquare / 8, col = startSquare % 8;

    ULL rookMask = (255ULL << (row * 8)) | (72340172838076673ULL << col);
    rookMask ^= (friendBitboard | enemyBitboard) & rookMask;

    ULL bishopMask = diagonals[row + col] | rdiagonals[7 - row + col];
    bishopMask ^= (friendBitboard | enemyBitboard) & bishopMask;

    ULL attacks = getRookAttack(startSquare, rookMask) | getBishopAttack(startSquare, bishopMask);
    attacks ^= friendBitboard;

    getAttacks(startSquare, attacks);

    popBit(bitboard, startSquare);
  }
}
