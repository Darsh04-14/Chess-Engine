#include <iostream>

#include "chess.h"
using namespace std;

// g++ -o chess -std=c++17 chess.cc chessgame.cc
// nbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 - FEN for normal game setup

int main() {
    // cout << "Enter FEN for game or 'd' for default setup: ";
    string FEN = "nbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    Chess game(FEN);

    cout << game.perft(4, 4) << "\n";

    cout << "Final Position:\n";
    game.print();
    cout << "\n";
}