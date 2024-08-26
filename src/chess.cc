#include "chess.h"

void Chess::pawnAttackTable() {
  memset(pawnAttacks, 0, sizeof(pawnAttacks));
  ULL notAFile = 0, notHFile = 0;
  for (int i = 0; i < 8; ++i) {
    notAFile |= (1 << (i * 8));
    notHFile |= (1 << (i * 8 + 7));
  }
  notAFile = ~notAFile;
  notHFile = ~notHFile;
  for (int i = 8; i < 56; ++i) {
    if (i & notAFile) {
      setBit(pawnAttacks[0][i], i + 7);
      setBit(pawnAttacks[1][i], i - 9);
    }
    if (i & notHFile) {
      setBit(pawnAttacks[0][i], i + 9);
      setBit(pawnAttacks[1][i], i - 7);
    }
  }
}

void Chess::knightAttackTable() {
  memset(knightAttacks, 0, sizeof(knightAttacks));
  int offsets[8][2] = {{2, 1}, {1, 2}, {-1, 2}, {-2, 1}, {-2, -1}, {-1, -2}, {1, -2}, {2, -1}};
  for (int i = 0; i < 64; ++i) {
    for (int j = 0; j < 8; ++j) {
      int row = i / 8, col = i % 8;
      int newRow = row + offsets[j][0];
      int newCol = col + offsets[j][1];

      if (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8) {
        int targetIndex = newRow * 8 + newCol;
        setBit(knightAttacks[i], targetIndex);
      }
    }
  }
}

void Chess::rookAttackTable() {
  // Initializing rook attacks
  std::ifstream rookAtt("./attacks/rookAttacks.txt");

  if (!rookAtt) std::cerr << "Could not open rook attacks!\n";
  for (int i = 0; i < 64; ++i) {
    int N;
    rookAtt >> rookMagics[i] >> N;
    for (int j = 0; j < N; ++j) {
      ULL magicKey, magicValue;
      rookAtt >> magicKey >> magicValue;
      rookAttacks[i][magicKey] = magicValue;
    }
  }
  rookAtt.close();
}

void Chess::bishopAttackTable() {
  std::ifstream bishopAtt("./attacks/bishopAttacks.txt");
  if (!bishopAtt) std::cerr << "Could not open bishop attacks!\n";
  for (int i = 0; i < 64; ++i) {
    int N;
    bishopAtt >> bishopMagics[i] >> N;
    for (int j = 0; j < N; ++j) {
      ULL magicKey, magicValue;
      bishopAtt >> magicKey >> magicValue;
      bishopAttacks[i][magicKey] = magicValue;
    }
  }
  bishopAtt.close();
}

void Chess::kingAttackTable() {
  memset(kingAttacks, 0, sizeof(kingAttacks));
  short offsets[8][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};

  for (int sq = 0; sq < 64; ++sq) {
    int row = sq / 8, col = sq % 8;

    for (int i = 0; i < 8; ++i) {
      int newRow = row + offsets[i][0];
      int newCol = col + offsets[i][1];

      if (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8) {
        int targetIndex = newRow * 8 + newCol;
        setBit(kingAttacks[sq], targetIndex);
      }
    }
  }
}

Chess::Chess() : legalMovesLen{0} {
  memset(castlingRights, -1, sizeof(castlingRights));
  memset(board, 0, sizeof(board));

  // Initializing all attack tables
  pawnAttackTable();
  knightAttackTable();
  rookAttackTable();
  bishopAttackTable();
  kingAttackTable();
}

Chess::Chess(string FEN) : Chess() {
  int row = 7, col = 0, i = 0;

  // Position segment of FEN
  for (; i < FEN.length() && FEN[i] != ' '; ++i) {
    if (FEN[i] == '/') {
      row -= 1;
      col = 0;
    } else if (isdigit(FEN[i])) {
      col += FEN[i] - '0';
    } else if (islower(FEN[i])) {
      setPiece(Black, getPiece(FEN[i]), row * 8 + col++);
    } else {
      setPiece(White, getPiece(FEN[i]), row * 8 + col++);
    }
  }

  // Colour to move flag
  if (++i >= FEN.length()) {
    colourToMove = White;
  } else {
    colourToMove = FEN[i] == 'w' ? White : Black;
  }

  i += 2;
  std::fill(&castlingRights[0][0], &castlingRights[0][0] + 4, -2);
  if (i < FEN.length() && FEN[i] != '-') {
    // Castling rights
    for (; i < FEN.length() && FEN[i] != ' '; ++i) {
      bool colour = (isupper(FEN[i]) ? White : Black) >> 4;
      bool side = tolower(FEN[i]) == 'q' ? 0 : 1;
      castlingRights[colour][side] = -1;
    }
  } else {
    ++i;
  }

  ++i;

  if (i < FEN.length() && FEN[i] != '-') {
    short targetIndex = (FEN[i + 1] - '1') * 8 + FEN[i] - 'a';
    if (FEN[i + 1] == '3') {
      targetIndex += 8;
      previousMoves.push_back({targetIndex - 16, targetIndex, DOUBLE_PAWN_PUSH});
    } else {
      targetIndex -= 8;
      previousMoves.push_back({targetIndex + 16, targetIndex, DOUBLE_PAWN_PUSH});
    }
  }

  // genLegalMoves();
}