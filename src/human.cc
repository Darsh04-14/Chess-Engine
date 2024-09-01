#include "human.h"

bool Human::notify() {
  string cmd;
  cin >> cmd;
  if (cmd == "move") {
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

Human::~Human() {}
