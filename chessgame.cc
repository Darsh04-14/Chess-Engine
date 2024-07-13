#include <iostream>
#include <unistd.h>
#include "chess.h"
#include "window.h"
// g++ -I/opt/X11/include -L/opt/X11/lib -o chess chessgame.cc chess.cc window.cc -lX11 -std=c++20
using namespace std;

int main() {
    Chess game("");
    ChessWindow window;
    int squareSize = 80;

    XEvent event;
    string start, end;

   
    game.print();

    while (1) {
        window.clear();
        drawBoard(window, game, squareSize);

        while (XPending(window.display) > 0) {
            XNextEvent(window.display, &event);
            if (event.type == Expose) {
                drawBoard(window, game, squareSize);
            }
        }

        cout << "Play move: ";
        cin >> start >> end;
        bool res = game.playMove(start, end);
        if (!res) {
            cout << "Invalid move!\n";
        } else {
            cout << "Move played: " << start << " to " << end << "\n";
        }
        game.print();

        usleep(100000); 
    }

    return 0;
}
