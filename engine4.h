#ifndef ENGINE4_H
#define ENGINE4_H

#include <time.h>

#include <algorithm>
#include <cstdlib>
#include <vector>

#include "chess.h"
#include "game.h"
#include "move.h"
#include "player.h"
#include "types.h"

#define PAWN_VALUE 126
#define KNIGHT_VALUE 781
#define BISHOP_VALUE 825
#define ROOK_VALUE 1276
#define QUEEN_VALUE 2538
#define MAX_DEPTH 3

class Engine4 : public Player {
    short pieceValue[6] = {0, QUEEN_VALUE, KNIGHT_VALUE, BISHOP_VALUE, ROOK_VALUE, PAWN_VALUE};
    int nodeCount;
    int boardEvaluation();
    int moveEvaluation(int, int, int, int);

    struct cmp {
        vector<short> board;
        cmp(const vector<short> &b) : board{b} {};
        bool operator()(const Move &, const Move &);
    };

   public:
    Engine4(Chess *g) : Player{g} { srand(time(0)); }
    bool notify() override;
};

#endif