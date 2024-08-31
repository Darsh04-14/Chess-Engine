#include <chrono>
#include <fstream>
#include <iostream>
using namespace std;

#include "chess.h"

int main() {
  ifstream file("./tests/quickSuite.in", fstream::in);

  if (!file) cout << "Unable to open fenTests.in!\n";

  string fen;
  double ms_time = 0;
  int totalNodes = 0;

  while (getline(file, fen, '\n')) {
    int depth, expNodeCount;
    file >> depth >> expNodeCount;

    file.ignore(numeric_limits<streamsize>::max(), '\n');

    Chess *chess = new Chess{fen};

    auto t1 = std::chrono::high_resolution_clock::now();
    int nodeCount = chess->perft(depth);

    totalNodes += nodeCount;

    auto t2 = std::chrono::high_resolution_clock::now();
    chrono::duration<double, std::milli> ms_double = (t2 - t1);
    ms_time += ms_double.count();

    if (nodeCount == expNodeCount) {
      cout << "Passed Test.\n";
    } else {
      cout << "Failed Test! " << nodeCount << " v " << expNodeCount << " | " << fen << "\n";
    }
  }

  std::cout << "Total time: " << ms_time / 1000 << "s | " << ms_time << "ms, at "
            << (unsigned long long)(totalNodes / (ms_time / 1000)) << " node/s\n";
}
