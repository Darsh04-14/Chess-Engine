#include <chrono>
#include <fstream>
#include <iostream>
using namespace std;

#include "chess.h"

int main() {
  ifstream file("./tests/fenTests.in", fstream::in);

  if (!file) cout << "Unable to open fenTests.in!\n";

  string fen;
  auto t1 = std::chrono::high_resolution_clock::now();
  getline(file, fen, '\n');
  int depth, expNodeCount;
  file >> depth >> expNodeCount;

  file.ignore(numeric_limits<streamsize>::max(), '\n');

  cout << "FEN: " << fen << "\n";
  Chess chess = Chess(fen);

  //   int nodeCount = chess.perft(depth);

  // if (nodeCount == expNodeCount) {
  //   cout << "Passed Test.\n";
  // } else {
  //   cout << "Failed Test! " << fen << "\n";
  // }

  auto t2 = std::chrono::high_resolution_clock::now();
  auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
  chrono::duration<double, std::milli> ms_double = (t2 - t1);

  std::cout << ms_double.count() / 1000 << "s | " << ms_int.count() << "ms\n";
}
