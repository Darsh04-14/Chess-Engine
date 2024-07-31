#include <fstream>
#include <iostream>
using namespace std;

#include "chess.h"

// g++ -std=c++14 -o test test.cc chess.cc move.cc movegen.cc

int main() {
    ifstream file("fenTests.in", fstream::in);

    string fen;
    while (getline(file, fen, '\n')) {
        int depth, expNodeCount;
        file >> depth >> expNodeCount;

        file.ignore(numeric_limits<streamsize>::max(), '\n');

        Chess* chess = new Chess(fen);

        int nodeCount = chess->perft(depth);

        if (nodeCount == expNodeCount) {
            cout << "Passed Test.\n";
        } else {
            cout << "Failed Test! " << fen << "\n";
        }
    }
}
