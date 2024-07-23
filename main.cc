#include <iostream>

#include "chess.h"
using namespace std;

// g++ -o chess -std=c++17 chess.cc chessgame.cc
// nbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 - FEN for normal game setup

int main() {
    // cout << "Enter FEN for game or 'd' for default setup: ";
    string FEN = "nbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    Chess game(FEN);

    game.print();
    while (!game.end()) {
        string start, end;
        cin >> start >> end;
        game.playMove(start, end);
        game.print();
    }
}