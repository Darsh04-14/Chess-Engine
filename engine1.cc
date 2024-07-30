#include "engine1.h"

bool Engine1::notify() {
    string cmd;
    cin >> cmd;
    if (cmd == "move") {
        vector<Move> legalMoves = chess->getLegalMoves();
        if (legalMoves.size() > 0) {
            int randidex = rand() % legalMoves.size();
            Move newMov = legalMoves[randidex];
            chess->playMove(newMov.start(), newMov.target());
        }
        return true;
    } else
        cout << "Invalid command!\n";
    return false;
}
