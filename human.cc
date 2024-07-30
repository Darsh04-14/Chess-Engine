#include "human.h"

bool Human::notify() {
    string cmd;
    cin >> cmd;
    if (cmd == "undo") {
        chess->unmakeMove();
        chess->unmakeMove();
        chess->generateLegalMoves();
        return false;
    }
    if (cmd == "resign") {
        chess->resign();
        return true;
    } else if (cmd == "move") {
        string start, end;
        cin >> start >> end;
        if (!chess->playMove(start, end)) {
            cout << "Invalid move!\n";
            return false;
        }
        return true;
    } else {
        cout << "Invalid command!\n";
        return false;
    }
}
