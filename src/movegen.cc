#include "chess.h"

void Chess::addMove(Move m) {
  if (pinnedPieces[m.start()] && !getBit(pinnedPieces[m.start()], m.target())) return;

  if (checks[0] && !checks[1] && pieceAt(board, m.start()) != King && !getBit(checks[0], m.target())) return;

  legalMoves[legalMovesLen++] = m;
}

void Chess::genAttacks(Colour c) {
  int colourInd = colourInd(c);

  attackBitboards[colourInd] = 0;
  memset(pinnedPieces, 0, sizeof(pinnedPieces));
  checks[0] = checks[1] = 0;

  ULL enemyKingBitboard = pieceBitboards[!colourInd][King];

  for (int i = 1; i <= 3; ++i) {
    ULL bitboard = pieceBitboards[colourInd][i];
    while (bitboard) {
      short square = lsbIndex(bitboard);
      ULL pieceMove = getPieceMoves(c, Piece(i), square);
      attackBitboards[colourInd] |= pieceMove;
      setPinsAndChecks(c, pieceMove & enemyKingBitboard, square);
      popBit(bitboard, square);
    }
  }

  // Sliding pieces
  ULL enemyPieces = colourBitboard(Colour(c ^ ColourType)), friendPieces = colourBitboard(c);
  for (int i = 4; i <= 6; ++i) {
    ULL bitboard = pieceBitboards[colourInd][i];
    while (bitboard) {
      short square = lsbIndex(bitboard);
      vector<ULL> rays = getRayAttacks(Piece(i), square);

      ULL mask1 = 0, mask2 = 0;
      for (int i = 0; i < rays.size(); ++i) {
        setPinsAndChecks(c, rays[i], square);
        if (i < 4)
          mask1 |= rays[i];
        else
          mask2 |= rays[i];
      }

      mask1 ^= mask1 & (friendPieces | enemyPieces);
      mask2 ^= mask2 & (friendPieces | enemyPieces);

      if (Piece(i) == Rook)
        attackBitboards[colourInd] |= getRookAttack(square, mask1);
      else if (Piece(i) == Bishop)
        attackBitboards[colourInd] |= getBishopAttack(square, mask1);
      else {
        attackBitboards[colourInd] |= getRookAttack(square, mask1);
        attackBitboards[colourInd] |= getBishopAttack(square, mask2);
      }

      popBit(bitboard, square);
    }
  }
}

void Chess::genLegalMoves() {
  genAttacks(Colour(colourToMove ^ ColourType));
  // cout << "Checks[0]\n";
  // printBitboard(checks[0]);
  // cout << "Checks[1]\n";
  // printBitboard(checks[1]);

  // for (int i = 0; i < 64; i++) {
  //   if (pinnedPieces[i]) {
  //     cout << "Pinned piece at " << i << "\n";
  //     printBitboard(pinnedPieces[i]);
  //   }
  // }

  legalMovesLen = 0;

  if (!checks[0] || !checks[1]) {
    genPawnMoves();
    genKnightMoves();
    genBishopMoves();
    genRookMoves();
    genQueenMoves();
    genCastleMove();
  }
  genKingMoves();
}

void Chess::genCastleMove() {
  int colourInd = colourInd(colourToMove);
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
  int colourInd = colourInd(colourToMove);

  ULL friendBitboard = colourBitboard(colourToMove);
  ULL bitboard = pieceBitboards[colourInd][King];

  while (bitboard) {
    int startSquare = lsbIndex(bitboard);

    ULL attacks = kingAttacks[startSquare];
    attacks ^= (attacks & friendBitboard);
    attacks ^= (attacks & attackBitboards[!colourInd]);

    getAttacks(startSquare, attacks);

    popBit(bitboard, startSquare);
  }
}

void Chess::genPawnMoves() {
  int colourInd = colourInd(colourToMove);
  int offsets[2][4] = {{8, 16, 7, 9}, {-8, -16, -9, -7}};

  int startingRank = colourInd ? 6 : 1;
  int promotionRank = colourInd ? 0 : 7;
  ULL bitboard = pieceBitboards[colourInd][Pawn];
  ULL friendBitboard = colourBitboard(colourToMove), enemyBitboard = colourBitboard(Colour(colourToMove ^ ColourType));

  while (bitboard) {
    int square = lsbIndex(bitboard);
    // Single Push
    int pushSquare = square + offsets[colourInd][0];
    if (!((friendBitboard | enemyBitboard) & (1ULL << pushSquare))) {
      if (pushSquare / 8 == promotionRank)
        for (int i = 3; i <= 6; ++i) addMove({square, pushSquare, PROMOTION, Piece(i)});
      else
        addMove({square, pushSquare, CAPTURE, NoPiece});

      // Double push
      int doublePushSquare = square + offsets[colourInd][1];
      if (!((friendBitboard | enemyBitboard) & (1ULL << doublePushSquare)) && square / 8 == startingRank)
        addMove({square, doublePushSquare, CAPTURE, NoPiece});
    }

    // Captures
    ULL attackBitboard = pawnAttacks[colourInd][square] & enemyBitboard;

    while (attackBitboard) {
      int attackSquare = lsbIndex(attackBitboard);

      if (attackSquare / 8 == promotionRank)
        for (int i = 3; i <= 6; ++i) addMove({square, attackSquare, pieceAt(board, attackSquare), Piece(i)});
      else
        addMove({square, attackSquare, CAPTURE, pieceAt(board, attackSquare)});

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
  int colourInd = colourInd(colourToMove);

  ULL friendBitboard = colourBitboard(colourToMove);
  ULL bitboard = pieceBitboards[colourInd][Knight];

  while (bitboard) {
    int startSquare = lsbIndex(bitboard);

    ULL attacks = knightAttacks[startSquare];
    attacks ^= (friendBitboard & attacks);

    getAttacks(startSquare, attacks);

    popBit(bitboard, startSquare);
  }
}

void Chess::genRookMoves() {
  int colourInd = colourInd(colourToMove);

  ULL friendBitboard = colourBitboard(colourToMove), enemyBitboard = colourBitboard(Colour(colourToMove ^ ColourType));
  ULL bitboard = pieceBitboards[colourInd][Rook];

  while (bitboard) {
    short startSquare = lsbIndex(bitboard);

    ULL mask = getPieceMoves(colourToMove, Rook, startSquare);
    mask ^= (friendBitboard | enemyBitboard) & mask;

    ULL attacks = getRookAttack(startSquare, mask);

    attacks ^= (friendBitboard & attacks);

    getAttacks(startSquare, attacks);

    popBit(bitboard, startSquare);
  }
}

void Chess::genBishopMoves() {
  int colourInd = colourInd(colourToMove);

  ULL friendBitboard = colourBitboard(colourToMove), enemyBitboard = colourBitboard(Colour(colourToMove ^ ColourType));
  ULL bitboard = pieceBitboards[colourInd][Bishop];

  while (bitboard) {
    int startSquare = lsbIndex(bitboard);

    ULL mask = getPieceMoves(colourToMove, Bishop, startSquare);
    mask ^= ((friendBitboard | enemyBitboard) & mask);

    ULL attacks = getBishopAttack(startSquare, mask);
    attacks ^= (attacks & friendBitboard);

    getAttacks(startSquare, attacks);

    popBit(bitboard, startSquare);
  }
}

void Chess::genQueenMoves() {
  int colourInd = colourInd(colourToMove);

  ULL friendBitboard = colourBitboard(colourToMove), enemyBitboard = colourBitboard(Colour(colourToMove ^ ColourType));
  ULL bitboard = pieceBitboards[colourInd][Queen];

  while (bitboard) {
    int startSquare = lsbIndex(bitboard);

    ULL rookMask = getPieceMoves(colourToMove, Rook, startSquare);
    rookMask ^= (friendBitboard | enemyBitboard) & rookMask;

    ULL bishopMask = getPieceMoves(colourToMove, Bishop, startSquare);
    bishopMask ^= (friendBitboard | enemyBitboard) & bishopMask;

    ULL attacks = getRookAttack(startSquare, rookMask) | getBishopAttack(startSquare, bishopMask);

    attacks ^= (friendBitboard & attacks);

    getAttacks(startSquare, attacks);

    popBit(bitboard, startSquare);
  }
}
