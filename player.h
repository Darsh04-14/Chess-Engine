#ifndef PLAYER_H
#define PLAYER_H
#include <iostream>
using namespace std;

class Chess;

class Player {
   protected:
    Chess* chess;

   public:
    Player(Chess* g) : chess{g} {};
    virtual bool notify() = 0;
};

#endif