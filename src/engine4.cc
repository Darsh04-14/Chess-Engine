#include "engine4.h"

Engine4::Engine4(Chess* g) : Player{g}, c{g->getBoard(), ply} {
  ifstream file("./position_scores/king.txt");

  for (int i = 7; i >= 0; --i) {
    for (int j = 0; j < 8; ++j) file >> positionScores[King][i * 8 + j];
  }
  file.close();
  file.open("./position_scores/pawn.txt");
  for (int i = 7; i >= 0; --i) {
    for (int j = 0; j < 8; ++j) file >> positionScores[Pawn][i * 8 + j];
  }
  file.close();
  file.open("./position_scores/knight.txt");
  for (int i = 7; i >= 0; --i) {
    for (int j = 0; j < 8; ++j) file >> positionScores[Knight][i * 8 + j];
  }
  file.close();
  file.open("./position_scores/bishop.txt");
  for (int i = 7; i >= 0; --i) {
    for (int j = 0; j < 8; ++j) {
      file >> positionScores[Bishop][i * 8 + j];
      positionScores[Queen][i * 8 + j] = positionScores[Bishop][i * 8 + j];
    }
  }
  file.close();
  file.open("./position_scores/rook.txt");
  for (int i = 7; i >= 0; --i) {
    for (int j = 0; j < 8; ++j) {
      file >> positionScores[Rook][i * 8 + j];
      positionScores[Queen][i * 8 + j] += positionScores[Rook][i * 8 + j];
    }
  }
  file.close();
}

inline int Engine4::cmp::scoreMove(const Move& move) {
  if (move.capture()) {
    return pieceValue[move.capture()] - pieceValue[pieceAt(board, move.start())] + 2e4;
  }

  if (killerMoves[ply][0] == move) {
    return 9000;
  } else if (killerMoves[ply][1] == move) {
    return 8000;
  } else {
    return historyMoves[pieceNum(board, move.start())][move.target()];
  }
}

bool Engine4::cmp::operator()(const Move& lhs, const Move& rhs) { return scoreMove(lhs) > scoreMove(rhs); }

void Engine4::cmp::clear() {
  memset(killerMoves, 0, sizeof(killerMoves));
  memset(historyMoves, 0, sizeof(historyMoves));
}

bool Engine4::PairCmp::operator()(const pair<int, Move>& lhs, const pair<int, Move>& rhs) {
  return lhs.first > rhs.first;
}

int Engine4::quiescence(int alpha, int beta) {
  ++nodeCount;
  chess->genLegalMoves();
  short legalMovesLen = chess->getLegalMovesLen();

  int evaluation = boardEvaluation();

  if (!legalMovesLen) {
    return evaluation;
  }

  if (evaluation >= beta) {
    return evaluation;
  }

  if (evaluation > alpha) {
    alpha = evaluation;
  }

  const Move* legalMoves = chess->getLegalMoves();
  Move* currentMoves = new Move[legalMovesLen];
  std::copy(legalMoves, legalMoves + legalMovesLen, currentMoves);
  sort(currentMoves, currentMoves + legalMovesLen, c);

  int valuation = -2e6;
  for (int i = 0; i < legalMovesLen; ++i) {
    chess->makeMove(currentMoves[i]);
    valuation = max(-quiescence(-beta, -alpha), valuation);
    chess->undoMove();
    alpha = max(alpha, valuation);
    if (valuation >= beta) break;
  }

  delete[] currentMoves;
  currentMoves = nullptr;

  return valuation;
}

int Engine4::boardEvaluation() {
  Colour c = chess->getCurrentPlayer();
  bool colourInd = colourInd(c);
  ULL whiteAttack = chess->attackBitboards[WHITE_IND], blackAttack = chess->attackBitboards[BLACK_IND];
  ULL *whitePieces = chess->pieceBitboards[WHITE_IND], *blackPieces = chess->pieceBitboards[BLACK_IND];
  int score = 0;
  for (int i = 1; i < 7; ++i) {
    ULL bitboard = whitePieces[i];
    while (bitboard) {
      int square = lsbIndex(bitboard);
      popLsb(bitboard);
      score += pieceValue[i];
      score += positionScores[i][square];
    }
    bitboard = blackPieces[i];
    while (bitboard) {
      int square = lsbIndex(bitboard);
      popLsb(bitboard);
      score -= pieceValue[i];
      score -= positionScores[i][8 * (7 - square / 8) + square % 8];
    }
  }

  score += (countBits(whiteAttack) - countBits(blackAttack)) * SQUARE_VALUE;

  return colourInd ? -score : score;
}

int Engine4::moveEvaluation(int depth, int alpha, int beta) {
  ++nodeCount;
  chess->genLegalMoves();

  if (chess->end()) return chess->end() == (White | Black) ? 0 : (-2e6 + ply);

  if (chess->check() == chess->colourToMove) ++depth;

  pvLength[ply] = ply;

  if (!depth) {
    chess->genCapturesOnly = true;
    int eval = quiescence(alpha, beta);
    chess->genCapturesOnly = false;
    return eval;
  }

  short legalMovesLen = chess->getLegalMovesLen();
  const Move* legalMoves = chess->getLegalMoves();
  Move* currentMoves = new Move[legalMovesLen];
  std::copy(legalMoves, legalMoves + legalMovesLen, currentMoves);
  sort(currentMoves, currentMoves + legalMovesLen, c);

  int valuation = -2e6;
  for (int i = 0; i < legalMovesLen; ++i) {
    ++ply;
    chess->makeMove(currentMoves[i]);
    valuation = max(-moveEvaluation(depth - 1, -beta, -alpha), valuation);
    chess->undoMove();
    --ply;
    if (valuation >= beta) {
      if (!currentMoves[i].capture()) {
        c.killerMoves[ply][1] = c.killerMoves[ply][0];
        c.killerMoves[ply][0] = currentMoves[i];
      }
      valuation = beta;
      break;
    }

    if (valuation > alpha) {
      alpha = valuation;

      if (!currentMoves[i].capture())
        c.historyMoves[pieceNum(chess->board, currentMoves[i].start())][currentMoves[i].target()] += depth;

      pvTable[ply][ply] = currentMoves[i];

      for (int i = ply + 1; i < pvLength[ply + 1]; ++i) pvTable[ply][i] = pvTable[ply + 1][i];

      pvLength[ply] = pvLength[ply + 1];
    }
  }

  delete[] currentMoves;
  currentMoves = nullptr;

  return valuation;
}

bool Engine4::notify() {
  string cmd;
  cin >> cmd;
  if (cmd == "move") {
    short legalMovesLen = chess->getLegalMovesLen();

    if (!legalMovesLen) return false;

    c.clear();
    memset(pvTable, 0, sizeof(pvTable));
    memset(pvLength, 0, sizeof(pvLength));

    ply = 0;
    int evaluation = 0;

    auto t1 = std::chrono::high_resolution_clock::now();

    for (int depth = 1; depth <= MAX_DEPTH; ++depth) evaluation = moveEvaluation(depth, -2e6, 2e6);

    auto t2 = std::chrono::high_resolution_clock::now();
    chrono::duration<double, std::milli> ms_double = (t2 - t1);

    cout << "Best evaluation: " << evaluation << " nodes searched " << nodeCount << " | ";
    cout << ms_double.count() << "ms, at " << nodeCount / (ms_double.count() / 1000) << "node/s";

    // cout << ", principle variation: ";
    // for (int i = 0; i < pvLength[0]; ++i) {
    //   Move pvMove = pvTable[0][i];
    //   cout << "(" << pvMove.start() << " -> " << pvMove.target() << "), ";
    // }
    cout << "\n";

    chess->makeMove(pvTable[0][0]);
    chess->genLegalMoves();

    return true;
  } else
    cout << "Invalid command!\n";
  return false;
}

Engine4::~Engine4() {}
