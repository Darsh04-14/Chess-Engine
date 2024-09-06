#include "engine4.h"

Engine4::Engine4(Chess* g) : Player{g}, c{g->getBoard(), g->ply} {
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

  return 0;
}

bool Engine4::cmp::operator()(const Move& lhs, const Move& rhs) { return scoreMove(lhs) > scoreMove(rhs); }

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

int Engine4::moveEvaluation(int depth, int alpha, int beta, int moveCounter = 0) {
  ++nodeCount;
  if (!depth) {
    chess->genCapturesOnly = true;
    int eval = quiescence(alpha, beta);
    chess->genCapturesOnly = false;
    return eval;
  }

  chess->genLegalMoves();

  if (chess->end()) return chess->end() == (White | Black) ? 0 : (-2e6 + moveCounter);
  if (chess->check() && moveCounter <= 12) depth += 1;

  short legalMovesLen = chess->getLegalMovesLen();
  const Move* legalMoves = chess->getLegalMoves();
  Move* currentMoves = new Move[legalMovesLen];
  std::copy(legalMoves, legalMoves + legalMovesLen, currentMoves);
  sort(currentMoves, currentMoves + legalMovesLen, c);

  int valuation = -2e6;
  for (int i = 0; i < legalMovesLen; ++i) {
    chess->makeMove(currentMoves[i]);
    valuation = max(-moveEvaluation(depth - 1, -beta, -alpha, moveCounter + 1), valuation);
    chess->undoMove();
    if (valuation > alpha) {
      alpha = valuation;
      c.historyMoves[pieceNum(chess->board, currentMoves[i].start())][currentMoves[i].target()] += depth;
    }
    if (valuation >= beta) {
      c.killerMoves[chess->ply][1] = c.killerMoves[chess->ply][0];
      c.killerMoves[chess->ply][0] = currentMoves[i];
      break;
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

    const Move* legalMoves = chess->getLegalMoves();

    vector<pair<int, Move>> moves;
    for (int i = 0; i < legalMovesLen; ++i) moves.push_back({0, legalMoves[i]});

    auto t1 = std::chrono::high_resolution_clock::now();
    Move bestMove;
    int alpha;
    for (int d = 1; d <= MAX_DEPTH; ++d) {
      nodeCount = 0;
      bestMove = moves[0].second;
      alpha = -2e6;
      for (int j = 0; j < moves.size(); ++j) {
        chess->makeMove(moves[j].second);
        int value = -moveEvaluation(d - 1, -2e6, -alpha);
        moves[j].first = value;
        if (value > alpha) {
          bestMove = moves[j].second;
          alpha = value;
        }
        chess->undoMove();
      }
      sort(moves.begin(), moves.end(), PairCmp());
    }

    auto t2 = std::chrono::high_resolution_clock::now();
    chrono::duration<double, std::milli> ms_double = (t2 - t1);

    cout << "Best evaluation: " << alpha << " nodes searched " << nodeCount << " | ";
    cout << ms_double.count() << "ms, at " << nodeCount / (ms_double.count() / 1000) << "node/s\n";

    chess->makeMove(bestMove);
    chess->genLegalMoves();

    return true;
  } else
    cout << "Invalid command!\n";
  return false;
}

Engine4::~Engine4() {}
