#include <iostream>
#include <string>
#include "chess.h"

Chess::Chess() {
    memset(board, 0, sizeof(board));
}


Chess::Chess(string inputed){
    memset(board, 0, sizeof(board));
if(inputed == ""){ // does default game setup

// setup for white
board[7][0] = Rook | White;
board[7][1] = Knight | White;
board[7][2] = Bishop | White;
board[7][3] = Queen | White;
board[7][4] = King | White;
board[7][5] = Bishop | White;
board[7][6] = Knight | White;
board[7][7] = Rook | White;
for (int j = 0; j < 8; ++j) {
    board[6][j] = Pawn | White;
}

//for black 
board[0][0] = Rook | Black;
board[0][1] = Knight | Black;
board[0][2] = Bishop | Black;
board[0][3] = Queen | Black;
board[0][4] = King | Black;
board[0][5] = Bishop | Black;
board[0][6] = Knight | Black;
board[0][7] = Rook | Black;
for (int j = 0; j < 8; ++j) {
    board[1][j] = Pawn | Black;
}



   
}else{
    // do customized
}
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
