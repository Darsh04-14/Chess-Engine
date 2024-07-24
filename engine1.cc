#include "engine1.h"

void Engine1::notify() {
      vector<Move> legalMoves = chess->getLegalMoves();
    if (legalMoves.size() > 0) {
        int randidex = rand() % legalMoves.size();
        Move newMov = legalMoves[randidex];
        chess->playMove(newMov.start(), newMov.target());
    }
}