#ifndef ENGINE2_H
#define ENGINE2_H

#include <time.h>

#include <cstdlib>
#include <vector>

#include "chess.h"
#include "game.h"
#include "move.h"
#include "player.h"
#include "types.h"

class Engine2 : public Player {
   public:
    Engine2(Chess *g) : Player{g} { srand(time(0)); }
    bool notify() override;
};

#endif
