#ifndef GAME_H
#define GAME_H
#include <string>
using namespace std;

#include "player.h"

class Move;

class Game {
  Player *players[2];
  bool currentPlayer = 0;

 public:
  void addPlayers(Player *p1, Player *p2) {
    players[0] = p1;
    players[1] = p2;
  }
  void notifyPlayer() {
    if (players[currentPlayer]->notify()) {
      currentPlayer = !currentPlayer;
    }
  }
  virtual bool playMove(string, string) = 0;
  virtual bool playMove(short, short) = 0;
  virtual bool playMove(short) = 0;
  virtual vector<Move> getLegalMoves() = 0;
  virtual vector<short> getBoard() = 0;
  ~Game() {
    delete players[0];
    delete players[1];
  }
};

#endif
