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

#define KING_VALUE 20000
#define SQUARE_VALUE 63
#define PAWN_VALUE 126
#define KNIGHT_VALUE 781
#define BISHOP_VALUE 825
#define ROOK_VALUE 1276
#define QUEEN_VALUE 2538
#define MAX_DEPTH 5

const short pieceValue[7] = {0, KING_VALUE, PAWN_VALUE, KNIGHT_VALUE, BISHOP_VALUE, ROOK_VALUE, QUEEN_VALUE};

class Engine4 : public Player {
  int nodeCount;
  int boardEvaluation();
  int moveEvaluation(int, int, int, int);

  struct cmp {
    const short *board;
    cmp(const short *b) : board{b} {};
    bool operator()(const Move &, const Move &);
  };

  cmp c;

 public:
  Engine4(Chess *g) : Player{g}, c{g->getBoard()} {}
  bool notify() override;
  ~Engine4() override;
};

#endif
