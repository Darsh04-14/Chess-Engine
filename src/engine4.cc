#include "engine4.h"

bool Engine4::cmp::operator()(const Move& lhs, const Move& rhs) {
  if (!lhs.capture() || !rhs.capture()) return lhs.capture();
  short valueA = pieceValue[lhs.capture()] - pieceValue[pieceAt(board, lhs.start())];
  int valueB = pieceValue[rhs.capture()] - pieceValue[pieceAt(board, rhs.start())];

  return valueA > valueB;
}

bool Engine4::PairCmp::operator()(const pair<int, Move>& lhs, const pair<int, Move>& rhs) {
  return lhs.first > rhs.first;
}

int Engine4::boardEvaluation() {
  Colour c = chess->getCurrentPlayer();
  Colour enemyColour = Colour(c ^ ColourType);
  const short* board = chess->getBoard();

  int score = 0;
  for (int i = 0; i < 64; ++i) {
    Piece piece = Piece(board[i] & PieceType);
    Colour pieceColour = Colour(board[i] & ColourType);

    if (piece != NoPiece) {
      if (pieceColour == c) {
        if (!chess->isSquareAttacked(enemyColour, i)) {
          score += pieceValue[piece];
        } else if (chess->isSquareAttacked(c, i)) {
          score += int(pieceValue[piece] / (1.4));
        } else {
          score += pieceValue[piece] / 4;
        }
      } else {
        if (!chess->isSquareAttacked(c, i)) {
          score -= pieceValue[piece];
        } else if (chess->isSquareAttacked(enemyColour, i)) {
          score -= int(pieceValue[piece] / (1.6));
        } else {
          score -= pieceValue[piece] / 16;
        }
      }
    }

    if (chess->isSquareAttacked(c, i)) score += SQUARE_VALUE;
    if (chess->isSquareAttacked(enemyColour, i)) score -= SQUARE_VALUE;
  }

  return score;
}

int Engine4::moveEvaluation(int depth, int alpha, int beta, int moveCounter = 0) {
  chess->genLegalMoves();

  if (chess->end()) {
    return chess->end() != (White | Black) ? (-2e6 + moveCounter) : 0;
  }

  if (chess->check() && moveCounter <= 7 && alpha < 1e6 && beta < 1e6) depth += 1;

  if (!depth) {
    ++nodeCount;
    return boardEvaluation();
  }

  vector<Move> currentMoves = chess->getLegalMoves();
  sort(currentMoves.begin(), currentMoves.end(), c);

  int valuation = -2e6;
  for (int i = 0; i < currentMoves.size(); ++i) {
    chess->makeMove(currentMoves[i]);
    valuation = max(-moveEvaluation(depth - 1, -beta, -alpha, moveCounter + 1), valuation);
    chess->undoMove();
    alpha = max(alpha, valuation);
    if (valuation >= beta) break;
  }

  return valuation;
}

bool Engine4::notify() {
  string cmd;
  cin >> cmd;
  if (cmd == "move") {
    auto t1 = std::chrono::high_resolution_clock::now();

    vector<Move> currentMoves = chess->getLegalMoves();

    if (!currentMoves.size()) return false;

    vector<pair<int, Move>> moves;
    for (auto& i : currentMoves) moves.push_back({0, i});

    Move bestMove;
    int alpha;
    for (int d = 0; d <= MAX_DEPTH; ++d) {
      nodeCount = 0;
      bestMove = moves[0].second;
      alpha = -2e6;
      for (int j = 0; j < moves.size(); ++j) {
        chess->makeMove(moves[j].second);
        int value = -moveEvaluation(d, -2e6, -alpha);
        moves[j].first = value;
        if (value > alpha) {
          bestMove = moves[j].second;
          alpha = value;
        }
        chess->undoMove();
      }
      sort(moves.begin(), moves.end(), PairCmp());
    }

    cout << "Best evaluation: " << alpha << " nodes searched " << nodeCount << "\n";

    auto t2 = std::chrono::high_resolution_clock::now();
    chrono::duration<double, std::milli> ms_double = (t2 - t1);

    cout << "Best evaluation: " << alpha << " nodes searched " << nodeCount << " | ";
    cout << ms_double.count() << "ms\n";

    chess->makeMove(bestMove);
    chess->genLegalMoves();

    return true;
  } else
    cout << "Invalid command!\n";
  return false;
}

Engine4::~Engine4() {}
