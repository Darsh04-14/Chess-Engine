#include "chess.h"

inline void Chess::genAttacks(Colour c) {
  int colourInd = colourInd(c);

  attackBitboards[colourInd] = 0;
  pinRays = 0;
  checks = 0;
  doubleCheck = false;
  enPassantPin = false;

  ULL enemyKingBitboard = pieceBitboards[!colourInd][King];

  for (int i = 1; i <= 3; ++i) {
    ULL bitboard = pieceBitboards[colourInd][i];
    while (bitboard) {
      short square = lsbIndex(bitboard);
      ULL pieceMove = getPieceMoves(c, Piece(i), square);
      attackBitboards[colourInd] |= pieceMove;
      setPinsAndChecks(c, pieceMove & enemyKingBitboard, square);
      popLsb(bitboard);
    }
  }

  // Don't treat king piece as a blocker
  ULL enemyPieces = pieceBitboards[!colourInd][0] ^ enemyKingBitboard, friendPieces = pieceBitboards[colourInd][0];

  // Rook Attacks
  ULL rookBitboard = pieceBitboards[colourInd][Rook];
  while (rookBitboard) {
    short square = lsbIndex(rookBitboard);
    ULL vertical = ((72340172838076673ULL << square % 8) ^ (1ULL << square));
    ULL horizontal = ((255UL << (square / 8 * 8)) ^ (1ULL << square));
    ULL half = (1ULL << square) - 1;
    setPinsAndChecks(c, vertical & half, square);
    setPinsAndChecks(c, vertical & ~half, square);
    setPinsAndChecks(c, horizontal & half, square);
    setPinsAndChecks(c, horizontal & ~half, square);

    ULL mask = rookMasks[square];
    mask ^= mask & (friendPieces | enemyPieces);

    attackBitboards[colourInd] |= getRookAttack(square, mask);

    popLsb(rookBitboard);
  }

  // Bishop Attacks
  ULL bishopBitboard = pieceBitboards[colourInd][Bishop];
  while (bishopBitboard) {
    short square = lsbIndex(bishopBitboard);
    ULL d = diagonals[square / 8 + square % 8] ^ (1ULL << square);
    ULL rd = rdiagonals[7 - square / 8 + square % 8] ^ (1ULL << square);
    ULL half = (1ULL << square) - 1;
    setPinsAndChecks(c, d & half, square);
    setPinsAndChecks(c, d & ~half, square);
    setPinsAndChecks(c, rd & half, square);
    setPinsAndChecks(c, rd & ~half, square);

    ULL mask = bishopMasks[square];

    mask ^= mask & (friendPieces | enemyPieces);

    attackBitboards[colourInd] |= getBishopAttack(square, mask);

    popLsb(bishopBitboard);
  }

  // Queen Attacks
  ULL queenBitboard = pieceBitboards[colourInd][Queen];
  while (queenBitboard) {
    short square = lsbIndex(queenBitboard);
    ULL vertical = ((72340172838076673ULL << square % 8) ^ (1ULL << square));
    ULL horizontal = ((255UL << (square / 8 * 8)) ^ (1ULL << square));
    ULL d = diagonals[square / 8 + square % 8] ^ (1ULL << square);
    ULL rd = rdiagonals[7 - square / 8 + square % 8] ^ (1ULL << square);
    ULL half = (1ULL << square) - 1;

    setPinsAndChecks(c, d & half, square);
    setPinsAndChecks(c, d & ~half, square);
    setPinsAndChecks(c, rd & half, square);
    setPinsAndChecks(c, rd & ~half, square);
    setPinsAndChecks(c, vertical & half, square);
    setPinsAndChecks(c, vertical & ~half, square);
    setPinsAndChecks(c, horizontal & half, square);
    setPinsAndChecks(c, horizontal & ~half, square);

    ULL mask1 = bishopMasks[square], mask2 = rookMasks[square];
    mask1 ^= mask1 & (friendPieces | enemyPieces);
    mask2 ^= mask2 & (friendPieces | enemyPieces);

    attackBitboards[colourInd] |= getRookAttack(square, mask2);
    attackBitboards[colourInd] |= getBishopAttack(square, mask1);

    popLsb(queenBitboard);
  }
}

void Chess::genLegalMoves() {
  genAttacks(Colour(colourToMove ^ ColourType));

  legalMovesLen = 0;

  if (!doubleCheck) {
    genPawnMoves();
    genKnightMoves();
    genBishopMoves();
    genRookMoves();
    genQueenMoves();
    genCastleMove();
  }
  genKingMoves();
  setGameState();
}

inline void Chess::genCastleMove() {
  int colourInd = colourInd(colourToMove);
  short leftCastle = castlingRights[colourInd][0];
  short rightCastle = castlingRights[colourInd][1];

  if ((leftCastle != -1 && rightCastle != -1) || checks) return;

  short colourShift = colourInd * 56;
  short kingIndex = colourShift + 4;

  bool canCastle = true;
  ULL blockers = pieceBitboards[colourInd][0] | pieceBitboards[!colourInd][0];
  ULL enemyAttacks = attackBitboards[!colourInd];
  if (leftCastle == -1) {
    ULL mask = 14ULL << colourShift;
    if (blockers & mask) canCastle = false;
    mask <<= 1;

    if (enemyAttacks & mask) canCastle = false;

    if (canCastle) addMove({kingIndex, kingIndex - 2, LEFT_CASTLE});
  }
  canCastle = true;
  if (rightCastle == -1) {
    ULL mask = 96ULL << colourShift;
    if (blockers & mask) canCastle = false;
    mask |= (1ULL << kingIndex);

    if (mask & enemyAttacks) canCastle = false;

    if (canCastle) addMove({kingIndex, kingIndex + 2, RIGHT_CASTLE});
  }
}

inline void Chess::genKingMoves() {
  int colourInd = colourInd(colourToMove);

  ULL friendBitboard = pieceBitboards[colourInd][0];
  ULL bitboard = pieceBitboards[colourInd][King];

  while (bitboard) {
    int startSquare = lsbIndex(bitboard);

    ULL attacks = kingAttacks[startSquare];
    attacks ^= (attacks & friendBitboard);
    attacks ^= (attacks & attackBitboards[!colourInd]);

    getAttacks(startSquare, attacks);

    popLsb(bitboard);
  }
}

inline void Chess::genPawnMoves() {
  int colourInd = colourInd(colourToMove);
  int offsets[2][4] = {{8, 16, 7, 9}, {-8, -16, -9, -7}};

  int startingRank = colourInd ? 6 : 1;
  int promotionRank = colourInd ? 0 : 7;
  ULL bitboard = pieceBitboards[colourInd][Pawn];
  ULL friendBitboard = pieceBitboards[colourInd][0], enemyBitboard = pieceBitboards[!colourInd][0];

  while (bitboard) {
    int square = lsbIndex(bitboard);
    // Single Push
    int pushSquare = square + offsets[colourInd][0];
    if (!((friendBitboard | enemyBitboard) & (1ULL << pushSquare))) {
      if (pushSquare / 8 == promotionRank) {
        addMove({square, pushSquare, PROMOTION, Knight});
        addMove({square, pushSquare, PROMOTION, Bishop});
        addMove({square, pushSquare, PROMOTION, Rook});
        addMove({square, pushSquare, PROMOTION, Queen});
      } else
        addMove({square, pushSquare, CAPTURE, NoPiece});

      // Double push
      int doublePushSquare = square + offsets[colourInd][1];
      if (!((friendBitboard | enemyBitboard) & (1ULL << doublePushSquare)) && square / 8 == startingRank)
        addMove({square, doublePushSquare, DOUBLE_PAWN_PUSH, NoPiece});
    }

    // Captures
    ULL attackBitboard = pawnAttacks[colourInd][square] & enemyBitboard;
    while (attackBitboard) {
      int attackSquare = lsbIndex(attackBitboard);

      if (attackSquare / 8 == promotionRank) {
        addMove({square, attackSquare, pieceAt(board, attackSquare), Knight});
        addMove({square, attackSquare, pieceAt(board, attackSquare), Bishop});
        addMove({square, attackSquare, pieceAt(board, attackSquare), Rook});
        addMove({square, attackSquare, pieceAt(board, attackSquare), Queen});
      } else
        addMove({square, attackSquare, CAPTURE, pieceAt(board, attackSquare)});

      popBit(attackBitboard, attackSquare);
    }

    // En Passant
    if (previousMoves.size() && previousMoves.back().flag() == DOUBLE_PAWN_PUSH &&
        previousMoves.back().target() / 8 == square / 8 && !enPassantPin) {
      if (previousMoves.back().target() == square - 1)
        addMove({square, square + offsets[colourInd][2], ENPASSANT, Pawn});
      if (previousMoves.back().target() == square + 1)
        addMove({square, square + offsets[colourInd][3], ENPASSANT, Pawn});
    }

    popLsb(bitboard);
  }
}

inline void Chess::genKnightMoves() {
  int colourInd = colourInd(colourToMove);

  ULL friendBitboard = pieceBitboards[colourInd][0];
  ULL bitboard = pieceBitboards[colourInd][Knight];

  while (bitboard) {
    int startSquare = lsbIndex(bitboard);
    popLsb(bitboard);

    if (getBit(pinRays, startSquare)) continue;

    ULL attacks = knightAttacks[startSquare];
    attacks ^= (friendBitboard & attacks);

    getAttacks(startSquare, attacks);
  }
}

inline void Chess::genRookMoves() {
  int colourInd = colourInd(colourToMove);

  ULL friendBitboard = pieceBitboards[colourInd][0], enemyBitboard = pieceBitboards[!colourInd][0];
  ULL bitboard = pieceBitboards[colourInd][Rook];

  while (bitboard) {
    short startSquare = lsbIndex(bitboard);

    ULL mask = rookMasks[startSquare];
    mask &= ~(friendBitboard | enemyBitboard);

    ULL attacks = getRookAttack(startSquare, mask);

    attacks &= ~friendBitboard;

    if (getBit(checks, startSquare)) {
      attacks &= checks;
    }

    getAttacks(startSquare, attacks);

    popLsb(bitboard);
  }
}

inline void Chess::genBishopMoves() {
  int colourInd = colourInd(colourToMove);

  ULL friendBitboard = pieceBitboards[colourInd][0], enemyBitboard = pieceBitboards[!colourInd][0];
  ULL bitboard = pieceBitboards[colourInd][Bishop];

  while (bitboard) {
    int startSquare = lsbIndex(bitboard);

    ULL mask = bishopMasks[startSquare];
    mask ^= ((friendBitboard | enemyBitboard) & mask);

    ULL attacks = getBishopAttack(startSquare, mask);
    attacks ^= (attacks & friendBitboard);

    if (getBit(checks, startSquare)) {
      attacks &= checks;
    }

    getAttacks(startSquare, attacks);

    popLsb(bitboard);
  }
}

inline void Chess::genQueenMoves() {
  int colourInd = colourInd(colourToMove);

  ULL friendBitboard = pieceBitboards[colourInd][0], enemyBitboard = pieceBitboards[!colourInd][0];
  ULL bitboard = pieceBitboards[colourInd][Queen];

  while (bitboard) {
    int startSquare = lsbIndex(bitboard);

    ULL rookMask = rookMasks[startSquare];
    rookMask ^= (friendBitboard | enemyBitboard) & rookMask;

    ULL bishopMask = bishopMasks[startSquare];
    bishopMask ^= (friendBitboard | enemyBitboard) & bishopMask;

    ULL attacks = getRookAttack(startSquare, rookMask) | getBishopAttack(startSquare, bishopMask);

    attacks ^= (friendBitboard & attacks);

    if (getBit(checks, startSquare)) {
      attacks &= checks;
    }

    getAttacks(startSquare, attacks);

    popLsb(bitboard);
  }
}
