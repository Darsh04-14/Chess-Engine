#include <iostream>

#include "chess.h"
#include "engine1.h"
#include "engine2.h"
#include "engine3.h"
#include "engine4.h"
#include "human.h"
#include "window.h"

using namespace std;

void displayMenu() {
    cout << "Welcome to Chess!" << endl;
    cout << "Select mode:" << endl;
    cout << "1. Player vs Player" << endl;
    cout << "2. Player vs Engine" << endl;
    cout << "Enter your choice: ";
}

Player *getPlayer(string p, Chess *c) {
    if (p == "human") {
        return new Human{c};
    } else if (p == "computer[1]") {
        return new Engine1{c};
    } else if (p == "computer[2]") {
        return new Engine2{c};
    } else if (p == "computer[3]") {
        return new Engine3{c};
    } else if (p == "computer[4]") {
        return new Engine4{c};
    } else {
        return nullptr;
    }
}

int main() {
    // cout << "Enter FEN for game or 'd' for default setup: ";
    string defaultFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    Chess *chess = new Chess(defaultFEN);
    ChessWindow window;

    XMapWindow(window.display, window.win);
    XFlush(window.display);

    double whiteScore = 0, blackScore = 0;
    string cmd;
    while (true) {
        cmd = "q";
        cin >> cmd;
        if (cmd == "game") {
            string p1, p2;
            cin >> p1 >> p2;
            Player *player1 = getPlayer(p1, chess), *player2 = getPlayer(p2, chess);
            if (!player1 || !player2) {
                cout << "Error: Players can only be human, computer[1], computer[2], computer[3] or computer[4]\n";
            } else if (cin.fail()) {
            } else {
                chess->addPlayers(player1, player2);
                chess->print();
                drawBoard(window, *chess, 80);
                XFlush(window.display);
                while (!chess->end()) {
                    chess->notifyPlayer();
                    chess->print();
                    chess->printGameState();
                    drawBoard(window, *chess, 80);
                    XFlush(window.display);
                    cout << "\n";
                }

                if (chess->end() == White) {
                    whiteScore += 1;
                } else if (chess->end() == Black) {
                    blackScore += 1;
                } else {
                    whiteScore += 0.5;
                    blackScore += 0.5;
                }

                chess = new Chess(defaultFEN);
            }
        } else if (cmd == "setup") {
            chess->clear();
            while (true) {
                string s;
                cin >> s;
                if (s == "+") {
                    char piece;
                    string pos;
                    cin >> piece >> pos;
                    if (!chess->addPiece(piece, pos)) {
                        cout << "Invalid piece or position.\n";
                    }
                    chess->print();
                    drawBoard(window, *chess, 80);
                    XFlush(window.display);
                } else if (s == "-") {
                    string pos;
                    cin >> pos;
                    if (!chess->removePiece(pos)) {
                        cout << "";
                    }
                    chess->print();
                    drawBoard(window, *chess, 80);
                    XFlush(window.display);
                } else if (s == "=") {
                    string colour;
                    cin >> colour;
                    chess->setColour(colour);
                } else if (s == "done") {
                    if (!chess->validBoard()) {
                        cout << "Invalid board!\n";
                    } else {
                        chess->generateLegalMoves();
                        break;
                    }
                } else if (cin.fail()) {
                    break;
                } else {
                    cout << "Invalid command!\n";
                }
                drawBoard(window, *chess, 80);
                XFlush(window.display);
            }
        } else if (cin.fail() || cmd == "q") {
            cout << "White Score: " << whiteScore << "\nBlack Score: " << blackScore << "\n";
            break;
        } else {
            cout << "Invalid command!\n";
        }
    }

    return 0;
}
