#include <iostream>
#include "chess.h"

Chess::Chess() {
    memset(board, 0, sizeof(board));
}

void Chess::print() { 
    for (int row = 0; row < 8; ++row) {
        cout << 8 - row << ' ';
        for (int col = 0; col < 8; ++col) {
            int Piece = board[row][col];
            if (Piece == None) cout << ((row + col)%2 ? '-' : ' ');
            else {
                char Letter = Piece&((1 << 7) - 1);
                cout << ((Piece&Black) ? char(Letter + 32) : Letter); 
            }
        }
        cout << endl;
    }
    cout << "  abcdefgh\n";
}
