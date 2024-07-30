#include "human.h"

void Human::notify() {
    string cmd;
    cin >> cmd;
    if (cmd == "undo") {
        chess->unmakeMove();
        chess->unmakeMove();
        chess->generateLegalMoves();
    }
    if (cmd == "resign") {
        chess->resign();
    } else if (cmd == "move") {
        string start, end;
        cin >> start >> end;
        while (!chess->playMove(start, end)) {
            cin >> start >> end;
        }
    } else {
        cout << "Invalid move!\n";
    }
}