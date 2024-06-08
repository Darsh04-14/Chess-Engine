#include <iostream>
#include <string>
#include "chess.h"

int Chess::moveIndex(string a) {
    if (a.length() > 2) return -1;
    if (a[0] < 'a' || a[1] > 'h') return -1;
    if (a[1] < '1' || a[1] > '8') return -1;

    return 8*(a[0] - 'a') + a[1] - '1';
}

Chess::Chess(): colorToMove{1} {
    memset(board, 0, sizeof(board));
}

bool Chess::playMove(string start, string target) {
    //Check if move is in list of valid moves
    int startIndex = moveIndex(start), targetIndex = moveIndex(target);

    bool validMove = false;

    for (auto i : validMoves) {
        if (startIndex == i.start && targetIndex == i.target) {
            validMove = true;
            break;
        }
    }

    if (!validMove) return false;

}

void Chess::pawnMove(string start) {
    int startIndex = moveIndex(start);
    int row = startIndex / 8;
    int col = startIndex % 8;
    
    if (board[row][col] == (Pawn | White)) {// white pawn
        if (row == 1) { // if intial postion
            validMoves.push_back({startIndex, startIndex + 8}); //move one 
            validMoves.push_back({startIndex, startIndex + 16}); // move two
        } else {
            validMoves.push_back({startIndex, startIndex + 8}); // move one if anywhere else
        }

        // en pasant
        if (row == 4) { // enpasant for white only on row 5
            // left capture check
            if (col > 0 && board[row][col - 1] == (Pawn | Black)) {
                if (board[row + 1][col - 1] == None) {
                    validMoves.push_back({startIndex, startIndex + 7});
                }
            }
            // right capture check
            if (col < 7 && board[row][col + 1] == (Pawn | Black)) {
                if (board[row + 1][col + 1] == None) {
                    validMoves.push_back({startIndex, startIndex + 9});
                }
            }
        }
    } else if (board[row][col] == (Pawn | Black)) {
        // black pawn
        if (row == 6) { // black pawn start
            validMoves.push_back({startIndex, startIndex - 8}); // move forward one
            validMoves.push_back({startIndex, startIndex - 16}); // move forward two
        } else {
            validMoves.push_back({startIndex, startIndex - 8}); // move forware one if anywhere else
        }

        // en pasant black
        if (row == 4) { // black can only enpasant on row 4 (0 indexed)
            //  left capture check
            if (col > 0 && board[row][col - 1] == (Pawn | White)) {
                if (board[row - 1][col - 1] == None) {
                    validMoves.push_back({startIndex, startIndex - 9});
                }
            }
            // right capture check
            if (col < 7 && board[row][col + 1] == (Pawn | White)) {
                if (board[row - 1][col + 1] == None) {
                    validMoves.push_back({startIndex, startIndex - 7});
                }
            }
        }
    }
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
