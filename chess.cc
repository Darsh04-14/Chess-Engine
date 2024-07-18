#include <iostream>
#include <string>
#include <cstring>
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

void Chess::kingMoves(int startIndex) {
    int startRow = startIndex / 8;
    int startCol = startIndex % 8;
    int directions[8][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};

    int kingColor = board[startRow][startCol] & (White | Black);

    for (int i = 0; i < 8; ++i) {
        int newRow = startRow + directions[i][0];
        int newCol = startCol + directions[i][1];

        
        if (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8) {
            int targetIndex = newRow * 8 + newCol;
            int targetPiece = board[newRow][newCol];
            int targetColor = targetPiece & (White | Black);
            if (targetPiece == None || (targetColor != kingColor)) {
                validMoves.push_back({startIndex, targetIndex, targetPiece == None ? -1 : targetIndex});
            }
        }
    }
}

void Chess::castleMove() {
    //Need to check if king or rook has moved previosly and blocking checks
    int row = colorToMove == White ? 0 : 7;
    if (board[row][4] == (colorToMove | King)) {
        bool leftCastle = true, rightCastle = true;
        for (int i = 3; i > 0; --i)
            if (board[row][i] != None) leftCastle = false;
        for (int i = 5; i < 7; ++i)
            if (board[row][i] != None) rightCastle = false;
        
        if (leftCastle && board[row][0] == (colorToMove | Rook))
            validMoves.push_back({row*8 + 4, row*8 + 2, -2});
        if (rightCastle && board[row][7] == (colorToMove | Rook))
            validMoves.push_back({row*8 + 4, row*8 + 6, -2});
    }
}

void Chess::pawnMove(int startIndex) {
    int row = startIndex / 8;
    int col = startIndex % 8;

    if (board[row][col] == (Pawn | White)) { // white pawn
        if (row == 1) { // initial position
            if (board[row + 1][col] == None) validMoves.push_back({startIndex, startIndex + 8, -1}); // move one
            if (board[row + 2][col] == None) validMoves.push_back({startIndex, startIndex + 16, -1}); // move two
        } else {
            if (board[row + 1][col] == None) validMoves.push_back({startIndex, startIndex + 8, -1}); // move one
        }

        // en passant
        if (row == 4) { // en passant for white only on row 5
            // left capture check
            if (col > 0 && board[row][col - 1] == (Pawn | Black) && board[row + 1][col - 1] == None) {
                validMoves.push_back({startIndex, startIndex + 7, startIndex - 1});
            }
            // right capture check
            if (col < 7 && board[row][col + 1] == (Pawn | Black) && board[row + 1][col + 1] == None) {
                validMoves.push_back({startIndex, startIndex + 9, startIndex + 1});
            }
        }

        // regular diagonal capture
        if (col > 0 && (board[row + 1][col - 1] & Black)) {
            validMoves.push_back({startIndex, startIndex + 7, startIndex + 7});
        }
        if (col < 7 && (board[row + 1][col + 1] & Black)) {
            validMoves.push_back({startIndex, startIndex + 9, startIndex + 9});
        }

    } else if (board[row][col] == (Pawn | Black)) { // black pawn
        if (row == 6) { // initial position
            if (board[row - 1][col] == None) validMoves.push_back({startIndex, startIndex - 8, -1}); // move one
            if (board[row - 2][col] == None) validMoves.push_back({startIndex, startIndex - 16, -1}); // move two
        } else {
            if (board[row - 1][col] == None) validMoves.push_back({startIndex, startIndex - 8, -1}); // move one
        }

        // en passant
        if (row == 3) { // en passant for black only on row 4 (0 indexed)
            // left capture check
            if (col > 0 && board[row][col - 1] == (Pawn | White) && board[row - 1][col - 1] == None) {
                validMoves.push_back({startIndex, startIndex - 9, startIndex - 1});
            }
            // right capture check
            if (col < 7 && board[row][col + 1] == (Pawn | White) && board[row - 1][col + 1] == None) {
                validMoves.push_back({startIndex, startIndex - 7, startIndex + 1});
            }
        }

        // regular diagonal capture
        if (col > 0 && (board[row - 1][col - 1] & White)) {
            validMoves.push_back({startIndex, startIndex - 9, startIndex - 9});
        }
        if (col < 7 && (board[row - 1][col + 1] & White)) {
            validMoves.push_back({startIndex, startIndex - 7, startIndex - 7});
        }
    }
}


void Chess::knightMove(int startIndex) {
    cout << "Gen knight moves, for index " << startIndex << "\n";
    for (int i = 0; i < 4; i++) {
        int a = (i&1 ? -1 : 1)*8, b = (i&2 ? -1 : 1)*2;
        int row = (startIndex + a)/8, col = (startIndex + b)%8;
        if (row >= 0 && row < 8 && col >= 0 && col < 8) {
            if (board[row][col] == None) validMoves.push_back({startIndex, row*8+col, -1});
            else if (!(board[row][col] & colorToMove)) validMoves.push_back({startIndex, row*8+col, row*8+col});
        }
    }

    for (int i = 0; i < 4; i++) {
        int a = (i&1 ? -1 : 1)*16, b = (i&2 ? -1 : 1);
        int row = (startIndex + a)/8, col = (startIndex + b)%8;
        cout << row << " " << col << '\n';
        if (row >= 0 && row < 8 && col >= 0 && col < 8) {
            if (board[row][col] == None) validMoves.push_back({startIndex, row*8+col, -1});
            else if (!(board[row][col] & colorToMove)) validMoves.push_back({startIndex, row*8+col, row*8+col});
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

bool Chess::isCheck(int kingColor) {
    int kingPosition = -1;
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (board[i][j] == (King | kingColor)) {
                kingPosition = i * 8 + j;  
                cout << "King is at position: " << i << ", " << j << endl;
                break;
            }
        }
        if (kingPosition != -1) break;
    }

   
    int originalColor = colorToMove;
    vector<Move> originalValidMoves = validMoves;  

    colorToMove = (kingColor == White ? Black : White); //color switch for alll other moves
    validMoves.clear(); 
    generateMoves();

    
    bool isKingInCheck = false;
    for (const Move& move : validMoves) {
        if (move.target == kingPosition) {
            isKingInCheck = true;
            break;
        }
    }


    colorToMove = originalColor;
    validMoves = originalValidMoves;  

    return isKingInCheck;
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

void Chess::pawnPromotions(int startIndex) {
    int row = startIndex / 8;
    int col = startIndex % 8;
    int pawnPiece = board[row][col];
    int pawnColor = pawnPiece & (White|Black);
    cout << row << " " << col << endl;
    cout << pawnColor << endl;

    if ((pawnColor == White && row == 7) || (pawnColor == Black && row == 0)) {
        cout << "pawn has reached end! Enter a promotion: ";
        char choice;
        cin >> choice;
        int newPiece = pawnPiece; 

        
        if (tolower(choice) == 'q') {
            newPiece = Queen | pawnColor;
            cout << "Pawn promoted on to Queen.\n";
        }

        else if (tolower(choice) == 'r') {
            newPiece = Rook | pawnColor;
            std::cout << "Pawn promoted on to Rook.\n";
        }
    
        else if (tolower(choice) == 'b') {
            newPiece = Bishop | pawnColor;
            std::cout << "Pawn promoted on to Bishop.\n";
        }

        else if (tolower(choice) == 'n') {
            newPiece = Knight | pawnColor;
            std::cout << "Pawn promoted on to Knight.\n";
        }

        // update the board with the new piece
        board[row][col] = newPiece;
    } else {
        // else keep pawn to pawn if no selection matches
        cout << "no pawn promotion needed.\n";
    }
}

void Chess::generateMoves() {
    castleMove();
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (board[i][j] ==  (colorToMove | Pawn)) pawnMove(i*8+j);
            else if (board[i][j] == (colorToMove | Bishop)) diagonalMove(i*8+j);
            else if (board[i][j] == (colorToMove | Queen)) {
                diagonalMove(i*8+j);
                horizontalVerticalMoves(i*8+j);
            } else if (board[i][j] == (colorToMove | Rook)) horizontalVerticalMoves(i*8+j);
            else if (board[i][j] == (colorToMove | Knight)) knightMove(i*8+j);
             else if (board[i][j] == (colorToMove | King)) kingMoves(i*8+j);
        }
    }
}

bool Chess::playMove(string start, string target) {

    generateMoves();

    int startIndex = moveIndex(start), targetIndex = moveIndex(target);

     cout  << "start " << startIndex << " " << targetIndex << "\n";

    cout << "Valid moves: ";
    for (auto i : validMoves) {
        cout << "(" << i.start << ", " << i.target << ") ";
        if (startIndex == i.start && targetIndex == i.target) {

            if (i.isCapture != -1)
                board[i.isCapture/8][i.isCapture%8] = None;
            


            int piece = board[startIndex/8][startIndex%8];
            int capturedPiece = board[targetIndex/8][targetIndex%8];

            board[startIndex/8][startIndex%8] = None;
            board[targetIndex/8][targetIndex%8] = piece;

            // Is a castling move, so put rook in appropriate position
            if (i.isCapture == -2) {
                if (targetIndex%8 > 4)
                    swap(board[targetIndex/8][targetIndex%8 - 1], board[targetIndex/8][7]);
                else
                    swap(board[targetIndex/8][targetIndex%8 + 1], board[targetIndex/8][0]);
            }
               if (!isCheck(piece & (White | Black))) {
                pawnPromotions(targetIndex);
                validMoves.clear();
                colorToMove ^= ((3 << 7));
                return true;
            }

           
            board[startIndex / 8][startIndex % 8] = piece;
            board[targetIndex / 8][targetIndex % 8] = capturedPiece;
            if (i.isCapture != -1) {
                board[i.isCapture / 8][i.isCapture % 8] = capturedPiece;
            }

            cout << "Your king is in Check! ";
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
