#ifndef ENGINE3_H
#define ENGINE3_H

#include <time.h>

#include <cstdlib>
#include <vector>

#include "chess.h"
#include "game.h"
#include "move.h"
#include "player.h"
#include "types.h"

class Engine3 : public Player {
   public:
    Engine3(Chess *g) : Player{g} { srand(time(0)); }
    bool notify() override;
};

#endif
