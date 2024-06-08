#include <iostream>
#include "chess.h"
using namespace std;

// g++ -o chess -std=c++17 chess.cc chessgame.cc

int main() {
    Chess game("");
    game.print();

    while (1) {
        cout << "Play move: ";
        string start, end;
        cin >> start >> end;
        bool res = game.playMove(start, end);
        if (!res) cout << "Invalid move!\n";
        game.print();
    }
}