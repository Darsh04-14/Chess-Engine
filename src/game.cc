#include "game.h"

Game::~Game() {
  delete players[0];
  delete players[1];
}