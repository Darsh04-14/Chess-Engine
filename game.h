#ifndef GAME_H
#define GAME_H
#include <string>
using namespace std;

#include "player.h"

class Move;

class Game {
    Player *players[2];

   public:
    void addPlayers(Player *p1, Player *p2) {
        players[0] = p1;
        players[1] = p2;
    }
    void notifyPlayer(int ind) { players[ind]->notify(); }
    virtual bool playMove(string, string) = 0;
    virtual bool playMove(short, short) = 0;
    virtual vector<Move> getLegalMoves() = 0;
    virtual vector<short> getBoard() = 0;
};

#endif