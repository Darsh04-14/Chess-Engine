#include "engine3.h"

bool Engine3::notify() {
    string cmd;
    cin >> cmd;
    if (cmd == "move") {
        vector<Move> currentMoves = chess->getLegalMoves();
        Colour currentColour = chess->getCurrentPlayer();
        Colour enemyColour = Colour(ColourType ^ currentColour);

        short enemyKingIndex = chess->getKing(enemyColour);

        for (auto i : currentMoves) {
            chess->makeMove(i);
            chess->generateLegalMoves();
            if (chess->isSquareAttacked(currentColour, enemyKingIndex)) {
                return true;
            }
            chess->unmakeMove();
        }

        for (auto i : currentMoves) {
            if (i.isCapture()) {
                chess->makeMove(i);
                chess->generateLegalMoves();
                return true;
            }
        }

        chess->generateLegalMoves();
        for (auto i : currentMoves) {
            if (chess->isSquareAttacked(enemyColour, i.start()) && !chess->isSquareAttacked(enemyColour, i.target())) {
                chess->makeMove(i);
                chess->generateLegalMoves();
                return true;
            }
        }

        int randidex = rand() % currentMoves.size();
        Move newMov = currentMoves[randidex];
        chess->makeMove(newMov);
        chess->generateLegalMoves();
        return true;
    } else
        cout << "Invalid command!\n";
    return false;
}