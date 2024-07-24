#include "engine3.h"

void Engine3::notify() {
    vector<Move> currentMoves = chess->getLegalMoves();
    Colour currentColour = chess->getCurrentPlayer();
    Colour enemyColour = Colour((3 << 3) ^ currentColour);

    short enemyKingIndex = chess->getKing(enemyColour);

    for (auto i : currentMoves) {
        chess->makeMove(i);
        chess->generateLegalMoves();
        if (chess->isSquareAttacked(currentColour, enemyKingIndex)) {
            return;
        }
        chess->unmakeMove();
    }

    for (auto i : currentMoves) {
        if (i.isCapture()) {
            chess->makeMove(i);
            chess->generateLegalMoves();
            return;
        }
    }

    chess->generateLegalMoves();
    for (auto i : currentMoves) {
        if (chess->isSquareAttacked(enemyColour, i.start()) && !chess->isSquareAttacked(enemyColour, i.target())) {
            chess->makeMove(i);
            chess->generateLegalMoves();
            return;
        }
    }

    int randidex = rand() % currentMoves.size();
    Move newMov = currentMoves[randidex];
    chess->makeMove(newMov);
    chess->generateLegalMoves();
}