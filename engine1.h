#ifndef ENGINE1_H
#define ENGINE1_H
#include <cstdlib>
#include <ctime>
#include <vector>

#include "chess.h"
#include "game.h"
#include "player.h"
#include "types.h"

class Engine1 : public Player {
   public:
    Engine1(Chess *g) : Player{g} { srand(time(0)); }
    void notify() override;
};

#endif