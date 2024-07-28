#include <iostream>
#include "chess.h"
#include "window.h"
#include "engine1.h"
#include "engine2.h"
#include "engine3.h"
#include "engine4.h"
#include "human.h"

using namespace std;

void displayMenu() {
    cout << "Welcome to Chess!" << endl;
    cout << "Select mode:" << endl;
    cout << "1. Player vs Player" << endl;
    cout << "2. Player vs Engine" << endl;
    cout << "Enter your choice: ";
}

int main() {
    string FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    Chess *chess = new Chess(FEN);
    ChessWindow window;

    // Ensure the window is mapped and ready
    XMapWindow(window.display, window.win);
    XFlush(window.display);

    displayMenu();
    int choice;
    cin >> choice;

    if (choice == 1) {
        chess->addPlayers(new Human(chess), new Human(chess));
    } else if (choice == 2) {
        cout << "Select engine difficulty (1-4): ";
        int engineChoice;
        cin >> engineChoice;
        switch (engineChoice) {
            case 1:
                chess->addPlayers(new Human(chess), new Engine1(chess));
                break;
            case 2:
                chess->addPlayers(new Human(chess), new Engine2(chess));
                break;
            case 3:
                chess->addPlayers(new Human(chess), new Engine3(chess));
                break;
            case 4:
                chess->addPlayers(new Human(chess), new Engine4(chess));
                break;
            default:
                cout << "Invalid choice, defaulting to Engine4." << endl;
                chess->addPlayers(new Human(chess), new Engine4(chess));
                break;
        }
    } else {
        cout << "Invalid choice, defaulting to Player vs Player." << endl;
        chess->addPlayers(new Human(chess), new Human(chess));
    }

    // Clear and redraw the board immediately after mode selection
    window.clear();
    drawBoard(window, *chess, 80);  // Set the square size to 80
    XFlush(window.display);
    chess->print();  // Print the text-based board to the console
    chess->printGameState();  // Print the game state to the console

    bool currentPlayer = 0;

    while (!chess->end()) {
        chess->notifyPlayer(currentPlayer);

        window.clear();
        drawBoard(window, *chess, 80);  // Set the square size to 80
        XFlush(window.display);

        chess->print();  // Print the text-based board to the console
        chess->printGameState();  // Print the game state to the console

        currentPlayer = !currentPlayer;
    }

    delete chess;
    return 0;
}
