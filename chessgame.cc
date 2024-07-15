#include <iostream>

#include "chess.h"
using namespace std;

// g++ -o chess -std=c++17 chess.cc chessgame.cc
// rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR - FEN for normal game setup

int main() {
    Chess game("4k3/1p6/8/8/P7/8/8/4K3");
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