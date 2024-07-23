#include "human.h"

void Human::notify() {
    string start, end;
    cin >> start >> end;
    while (!chess->playMove(start, end)) {
        cin >> start >> end;
    };
}