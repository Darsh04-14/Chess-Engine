#ifndef CHESS_H
#define CHESS_H

#include <vector>
#include <iostream>
using namespace std;


struct Piece {
    const int King = 75;
    const int Queen = 81;
    const int Knight = 78;
    const int Bishop = 88;
    const int Rook = 82;
    const int Pawn = 80;

    const int White = (1 << 7);
    const int Black = (1 << 8);
};

class Chess {
    
};



class Board {
private:
    vector<vector <char> > board;
public:
    Board() {
        board.resize(8, vector<char>(8, ' '));
    }

    void printboard() { 
        for (int row = 0; row < 8; ++row) {
            cout << 8 - row << ' ';
            for (int col = 0; col < 8; ++col) {
                cout << board[row][col] << ' ';
            }
            cout << endl;
        }
    cout << "  a b c d e f g h\n";
    }
};

#endif
