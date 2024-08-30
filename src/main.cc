#include <iostream>

#include "chess.h"
#include "engine4.h"
#include "human.h"

using namespace std;

// 1r1qk1nr/p2n1p1p/2b3p1/2p1p1b1/7P/1PPQ4/P3PPP1/RN2KB1R w KQk - 0 15

int main() {
  Chess *chess = new Chess{"4Rr1k/3p1Pq1/p6p/1p5Q/1P5P/1r6/6P1/1B3R1K w - - 0 1"};

  chess->addPlayers(new Engine4{chess}, new Human{chess});

  while (!chess->end()) {
    chess->print();
    chess->notifyPlayer();
  }
  return 0;
}
