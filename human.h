#ifndef HUMAN_H
#define HUMAN_H

#include "chess.h"
#include "player.h"

class Human : public Player {
   public:
    Human(Chess *g) : Player{g} {}
    void notify() override;
};

#endif