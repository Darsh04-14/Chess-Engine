#include <iostream>
#include <string>
#include "chess.h"

int Chess::moveIndex(string a) {
    if (a.length() != 2) return -1;
    if (a[0] < 'a' || a[0] > 'h') return -1;
    if (a[1] < '1' || a[1] > '8') return -1;

    return a[0] - 'a' + 8*(a[1] - '1');
}

Chess::Chess(): colorToMove{(1 << 7)} {
    memset(board, 0, sizeof(board));
}

void Chess::pawnMove(int startIndex) {
    int row = startIndex / 8;
    int col = startIndex % 8;
    
    if (board[row][col] == (Pawn | White)) {// white pawn
        if (row == 1) { // if intial postion
            validMoves.push_back({startIndex, startIndex + 8, -1}); //move one 
            validMoves.push_back({startIndex, startIndex + 16, -1}); // move two
        } else {
            validMoves.push_back({startIndex, startIndex + 8, -1}); // move one if anywhere else
        }

        // en pasant
        if (row == 4) { // enpasant for white only on row 5
            // left capture check
            if (col > 0 && board[row][col - 1] == (Pawn | Black)) {
                if (board[row + 1][col - 1] == None) {
                    validMoves.push_back({startIndex, startIndex + 7, startIndex - 1});
                }
            }
            // right capture check
            if (col < 7 && board[row][col + 1] == (Pawn | Black)) {
                if (board[row + 1][col + 1] == None) {
                    validMoves.push_back({startIndex, startIndex + 9, startIndex + 1});
                }
            }
        }
         // left capture check no enpasant - reguaar
            if (col > 0 && board[row + 1][col - 1] == (Pawn | Black)) {
                validMoves.push_back({startIndex, startIndex + 7, startIndex + 7});
            }
            // right capture check no enpasant - regular
            if (col < 7 && board[row + 1][col + 1] == (Pawn | Black)) {
                validMoves.push_back({startIndex, startIndex + 9, startIndex + 9});
            }


    } else if (board[row][col] == (Pawn | Black)) {
        // black pawn
        if (row == 6) { // black pawn start
            validMoves.push_back({startIndex, startIndex - 8, -1}); // move forward one
            validMoves.push_back({startIndex, startIndex - 16, -1}); // move forward two
        } else {
            validMoves.push_back({startIndex, startIndex - 8, -1}); // move forware one if anywhere else
        }

        // en pasant black
        if (row == 3) { // black can only enpasant on row 4 (0 indexed)
            //  left capture check
            if (col > 0 && board[row][col - 1] == (Pawn | White)) {
                if (board[row - 1][col - 1] == None) {
                    validMoves.push_back({startIndex, startIndex - 9, startIndex - 1});
                }
            }
            // right capture check
            if (col < 7 && board[row][col + 1] == (Pawn | White)) {
                if (board[row - 1][col + 1] == None) {
                    validMoves.push_back({startIndex, startIndex - 7, startIndex + 1});
                }
            }
        }

             // left capture check no unpeasnt - regualr capture
            if (col > 0 && board[row - 1][col - 1] == (Pawn | White)) {
                validMoves.push_back({startIndex, startIndex - 9, startIndex - 9});
            }
            // right capture check  no unpeasnt - regualr capture
            if (col < 7 && board[row - 1][col + 1] == (Pawn | White)) {
                    validMoves.push_back({startIndex, startIndex - 7, startIndex - 7});
            }
    }
}

void Chess::horizontalVerticalMoves(int startIndex){
    int row = startIndex/8;
    int col = startIndex%8;
    int color = board[row][col] & (White | Black);


    //horizontal to the right
    for(int i = col + 1; i < 8; ++i){
        if(board[row][i] == None){
            validMoves.push_back({startIndex, row*8 + i, -1}); // move if nothing in path
        }else{
            if((board[row][i] & (White | Black)) != color){
                validMoves.push_back({startIndex, row*8 +i, i*8 + col}); //capture if opposite colour in path
            }
            break; //leave if peice in way
        }
    }

    //horizontal to the left
     for(int i = col - 1; i >= 0; --i){
        if(board[row][i] == None){
            validMoves.push_back({startIndex, row*8 + i, -1}); // move if nothing in path
        }else{
            if((board[row][i] & (White | Black)) != color){
                validMoves.push_back({startIndex, row*8 +i, i*8 + col}); //capture if opposite colour in path
            }
            break; //leave if peice in way
        }
    }

    //vertical upwards
    for(int i = row + 1; i < 8; ++i){
        if(board[i][col] == None){
            validMoves.push_back({startIndex, i*8 + col, -1}); // go up all the way
            cout << "(" << startIndex <<  "," << i*8+col<< ")" << endl;
        }else{
            if((board[i][col] & (White | Black)) != color){
                validMoves.push_back({startIndex, i*8 + col, i*8 + col}); // capture if opp color
            }
            break; // stop gen if in way
        }
    }

    //vertical downwards
    for(int i = row - 1; i >= 0; --i){
        if(board[i][col] == None){
            validMoves.push_back({startIndex, i*8 + col, -1}); // go up all the way
            
        }else{
            if((board[i][col] & (White | Black)) != color){
                validMoves.push_back({startIndex, i*8 + col, i*8 + col}); // capture if opp color
            }
            break; // stop gen if in way
        }
    }



}


void Chess::diagonalMove(int startIndex) {
    int notCovered = (1 << 4) - 1, r = 1;
    while (notCovered) {
        for (int i = 0; i < 4; i++) {
            int d = (1 << i);
            if (!(notCovered&d)) continue;

            int f = (i > 1) ? 2*(i%2) - 9 : 9 - 2*(i%2), t = startIndex + r*f;
            int row = t/8, col = t%8;
            if (row >= 0 && row < 8 && col >= 0 && col < 8) {
                if (board[row][col] == None) validMoves.push_back({startIndex, t, -1});
                else {
                    if (t == 54) {
                        cout << "Special case: " << board[row][col] << " " << colorToMove << " " << (board[row][col] | colorToMove) << "\n";
                    }
                    if (!(board[row][col] & colorToMove)) validMoves.push_back({startIndex, t, t});
                    notCovered ^= d;  
                }
            } else {
                notCovered ^= d;
            }
        }

        r++;
    }
}

void Chess::generateMoves() {
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (board[i][j] ==  (colorToMove | Pawn)) pawnMove(i*8+j);
            else if (board[i][j] == (colorToMove | Bishop)) diagonalMove(i*8+j);
            else if (board[i][j] == (colorToMove | Queen)) {
                diagonalMove(i*8+j);
                horizontalVerticalMoves(i*8+j);
            } else if (board[i][j] == (colorToMove | Rook)) horizontalVerticalMoves(i*8+j); 
        }
    }
}

bool Chess::playMove(string start, string target) {
    //Check if move is in list of valid moves
    generateMoves();

    int startIndex = moveIndex(start), targetIndex = moveIndex(target);

    // cout << startIndex << " " << targetIndex << "\n";

    cout << "Valid moves: ";
    for (auto i : validMoves) {
        cout << "(" << i.start << ", " << i.target << ") ";
        if (startIndex == i.start && targetIndex == i.target) {

            if (i.isCapture != -1)
                board[i.isCapture/8][i.isCapture%8] = None;

            int piece = board[startIndex/8][startIndex%8];

            board[startIndex/8][startIndex%8] = None;
            board[targetIndex/8][targetIndex%8] = piece;

            validMoves.clear();
            colorToMove ^= ((3 << 7));
            cout << "\n";

            return true;
        }
    }
    cout << "\n";

    return false;

}

Chess::Chess(string inputed): colorToMove{1 << 7} {
    memset(board, 0, sizeof(board));
    if(inputed == ""){ // does default game setup

        // setup for white
        board[0][0] = Rook | White;
        board[0][1] = Knight | White;
        board[0][2] = Bishop | White;
        board[0][3] = Queen | White;
        board[0][4] = King | White;
        board[0][5] = Bishop | White;
        board[0][6] = Knight | White;
        board[0][7] = Rook | White;
        for (int j = 0; j < 8; ++j) {
            board[1][j] = Pawn | White;
        }

        //for black 
        board[7][0] = Rook | Black;
        board[7][1] = Knight | Black;
        board[7][2] = Bishop | Black;
        board[7][3] = Queen | Black;
        board[7][4] = King | Black;
        board[7][5] = Bishop | Black;
        board[7][6] = Knight | Black;
        board[7][7] = Rook | Black;
        for (int j = 0; j < 8; ++j) {
            board[6][j] = Pawn | Black;
        }



    
    }else{
        // do customized
    }
}

void Chess::print() { 
    for (int row = 7; row >= 0; --row) {
        cout << row + 1 << ' ';
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
