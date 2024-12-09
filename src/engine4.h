#ifndef ENGINE4_H
#define ENGINE4_H

#include <time.h>

#include <algorithm>
#include <cstdlib>
#include <utility>
#include <vector>

#include "chess.h"
#include "game.h"
#include "move.h"
#include "player.h"
#include "types.h"

#define KING_VALUE 10000
#define SQUARE_VALUE 5
#define PAWN_VALUE 126
#define KNIGHT_VALUE 781
#define BISHOP_VALUE 825
#define ROOK_VALUE 1276
#define QUEEN_VALUE 2538
#define MAX_DEPTH 6
#define MAX_PLY 64

const short pieceValue[7] = {0, KING_VALUE, PAWN_VALUE, KNIGHT_VALUE, BISHOP_VALUE, ROOK_VALUE, QUEEN_VALUE};

class Engine4 : public Player {
  int nodeCount;
  short positionScores[7][64];

  short pvLength[MAX_PLY];
  Move pvTable[MAX_PLY][MAX_PLY];

  int boardEvaluation();
  int moveEvaluation(int, int, int);
  int quiescence(int, int);

  struct cmp {
    const short *board;
    Move killerMoves[MAX_PLY][2];
    int historyMoves[12][64];

    inline int scoreMove(const Move &);

    int &ply;

    cmp(const short *b, int &p) : board{b}, ply{p} {};
    bool operator()(const Move &, const Move &);
    void clear();
  };

  struct PairCmp {
    bool operator()(const pair<int, Move> &, const pair<int, Move> &);
  };

  int ply;
  cmp c;

 public:
  Engine4(Chess *g);
  bool notify() override;
  ~Engine4() override;
};

#endif
