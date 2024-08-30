#include "chess.h"

void Chess::addMove(Move m) {
  if (pinnedPieces[m.start()] && !getBit(pinnedPieces[m.start()], m.target())) return;
  if (m.isEnPassant() && enPassantPin) return;

  short offset = colourToMove == White ? -8 : 8;
  short targetSquare = m.target() + (m.isEnPassant() ? offset : 0);
  if (checks[0] && !checks[1] && pieceAt(board, m.start()) != King && !getBit(checks[0], targetSquare)) return;

  legalMoves[legalMovesLen++] = m;
}

inline void Chess::genAttacks(Colour c) {
  int colourInd = colourInd(c);

  attackBitboards[colourInd] = 0;
  memset(pinnedPieces, 0, sizeof(pinnedPieces));
  checks[0] = checks[1] = 0;
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
  ULL enemyPieces = colourBitboard(Colour(c ^ ColourType)) ^ enemyKingBitboard, friendPieces = colourBitboard(c);

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

    ULL mask = vertical | horizontal;
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

    ULL mask = d | rd;
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

    ULL mask1 = d | rd, mask2 = vertical | horizontal;
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

  if (!checks[0] || !checks[1]) {
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

  if (leftCastle != -1 && rightCastle != -1) return;

  short kingIndex = (colourInd == BLACK_IND ? 56 : 0) + 4;

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

inline void Chess::genKingMoves() {
  int colourInd = colourInd(colourToMove);

  ULL friendBitboard = colourBitboard(colourToMove);
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
        addMove({square, doublePushSquare, DOUBLE_PAWN_PUSH, NoPiece});
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

    popLsb(bitboard);
  }
}

inline void Chess::genKnightMoves() {
  int colourInd = colourInd(colourToMove);

  ULL friendBitboard = colourBitboard(colourToMove);
  ULL bitboard = pieceBitboards[colourInd][Knight];

  while (bitboard) {
    int startSquare = lsbIndex(bitboard);

    ULL attacks = knightAttacks[startSquare];
    attacks ^= (friendBitboard & attacks);

    getAttacks(startSquare, attacks);

    popLsb(bitboard);
  }
}

inline void Chess::genRookMoves() {
  int colourInd = colourInd(colourToMove);

  ULL friendBitboard = colourBitboard(colourToMove), enemyBitboard = colourBitboard(Colour(colourToMove ^ ColourType));
  ULL bitboard = pieceBitboards[colourInd][Rook];

  while (bitboard) {
    short startSquare = lsbIndex(bitboard);

    ULL mask = (((255UL << (startSquare / 8 * 8)) | (72340172838076673ULL << startSquare % 8)) ^ (1ULL << startSquare));
    mask ^= (friendBitboard | enemyBitboard) & mask;

    ULL attacks = getRookAttack(startSquare, mask);

    attacks ^= (friendBitboard & attacks);

    getAttacks(startSquare, attacks);

    popLsb(bitboard);
  }
}

inline void Chess::genBishopMoves() {
  int colourInd = colourInd(colourToMove);

  ULL friendBitboard = colourBitboard(colourToMove), enemyBitboard = colourBitboard(Colour(colourToMove ^ ColourType));
  ULL bitboard = pieceBitboards[colourInd][Bishop];

  while (bitboard) {
    int startSquare = lsbIndex(bitboard);

    ULL mask = (diagonals[startSquare / 8 + startSquare % 8] | rdiagonals[7 - startSquare / 8 + startSquare % 8]) ^
               (1ULL << startSquare);
    mask ^= ((friendBitboard | enemyBitboard) & mask);

    ULL attacks = getBishopAttack(startSquare, mask);
    attacks ^= (attacks & friendBitboard);

    getAttacks(startSquare, attacks);

    popLsb(bitboard);
  }
}

inline void Chess::genQueenMoves() {
  int colourInd = colourInd(colourToMove);

  ULL friendBitboard = colourBitboard(colourToMove), enemyBitboard = colourBitboard(Colour(colourToMove ^ ColourType));
  ULL bitboard = pieceBitboards[colourInd][Queen];

  while (bitboard) {
    int startSquare = lsbIndex(bitboard);

    ULL rookMask =
        (((255UL << (startSquare / 8 * 8)) | (72340172838076673ULL << startSquare % 8)) ^ (1ULL << startSquare));
    rookMask ^= (friendBitboard | enemyBitboard) & rookMask;

    ULL bishopMask =
        (diagonals[startSquare / 8 + startSquare % 8] | rdiagonals[7 - startSquare / 8 + startSquare % 8]) ^
        (1ULL << startSquare);
    bishopMask ^= (friendBitboard | enemyBitboard) & bishopMask;

    ULL attacks = getRookAttack(startSquare, rookMask) | getBishopAttack(startSquare, bishopMask);

    attacks ^= (friendBitboard & attacks);

    getAttacks(startSquare, attacks);

    popLsb(bitboard);
  }
}
