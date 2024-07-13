#include <iostream>
#include <unistd.h>
#include "chess.h"
#include "window.h"

using namespace std;

int main() {
    Chess game("");
    ChessWindow window;
    int squareSize = 80;

    XEvent event;
    string start, end;

    // Print the initial board
    game.print();

    while (1) {
        window.clear();
        drawBoard(window, game, squareSize);

        // Handle X11 events
        while (XPending(window.display) > 0) {
            XNextEvent(window.display, &event);
            if (event.type == Expose) {
                drawBoard(window, game, squareSize);
            }
        }

        // Check for user input
        cout << "Play move: ";
        cin >> start >> end;
        bool res = game.playMove(start, end);
        if (!res) {
            cout << "Invalid move!\n";
        } else {
            cout << "Move played: " << start << " to " << end << "\n";
        }
        game.print();

        usleep(100000); // Slow down the refresh rate
    }

    return 0;
}
