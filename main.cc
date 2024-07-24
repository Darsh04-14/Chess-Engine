#include <iostream>

#include "chess.h"
#include "engine1.h"
#include "engine2.h"
#include "engine3.h"
#include "human.h"
using namespace std;

// g++ -o chess -std=c++17 chess.cc chessgame.cc
// nbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 - FEN for normal game setup

int main() {
    // cout << "Enter FEN for game or 'd' for default setup: ";
    string FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    Chess *chess = new Chess(FEN);
    chess->addPlayers(new Human(chess), new Engine3(chess));

    bool currentPlayer = 0;
    chess->print();
    while (!chess->end()) {
        chess->notifyPlayer(currentPlayer);
        chess->print();
        cout << "\n";
        currentPlayer = !currentPlayer;
    }
}