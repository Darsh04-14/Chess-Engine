#include "chess.h"
using namespace std;

void Chess::genCastleMove() {
    short leftCastle = castlingRights[colourToMove >> 4][0];
    short rightCastle = castlingRights[colourToMove >> 4][1];

    if (leftCastle != -1 && rightCastle != -1) return;

    short kingIndex = (colourToMove == White ? 0 : 56) + 4;

    bool canCastle = true;
    if (leftCastle == -1) {
        for (int i = 3; i > 0; --i)
            if (board[kingIndex - i] != NoPiece) canCastle = false;

        for (int i = 2; i >= 0; --i)
            if (isSquareAttacked(colourToMove ^ ColourType, kingIndex - i)) canCastle = false;

        if (canCastle) addMove({kingIndex, kingIndex - 2, MoveFlag::CASTLE});
    }
    canCastle = true;
    if (rightCastle == -1) {
        for (int i = 2; i > 0; --i) {
            if (board[kingIndex + i] != NoPiece) {
                canCastle = false;
            }
        }

        for (int i = 2; i >= 0; --i) {
            if (isSquareAttacked(colourToMove ^ ColourType, kingIndex + i)) {
                canCastle = false;
            }
        }

        if (canCastle) addMove({kingIndex, kingIndex + 2, MoveFlag::CASTLE});
    }
}

void Chess::genKingMoves(short startIndex) {
    short offsets[8][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};

    short row = startIndex / 8, col = startIndex % 8;

    for (int i = 0; i < 8; ++i) {
        int newRow = row + offsets[i][0];
        int newCol = col + offsets[i][1];

        if (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8) {
            int targetIndex = newRow * 8 + newCol;
            Piece targetPiece = Piece(board[targetIndex] & PieceType);
            addMove({row * 8 + col, targetIndex, MoveFlag::CAPTURE, targetPiece});
        }
    }
}

void Chess::genPawnMoves(short startIndex) {
    short row = startIndex / 8, col = startIndex % 8;
    if (colourToMove == White) {                                                                     // white pawn
        if (board[startIndex + 8] == NoPiece) {                                                         // initial position
            if (row == 1 && board[startIndex + 16] == NoPiece) addMove({startIndex, startIndex + 16});  // move two
            if (row < 6)
                addMove({startIndex, startIndex + 8});
            else {
                for (int i = 2; i <= 5; ++i) addMove({startIndex, startIndex + 8, MoveFlag::PROMOTION, i});
            }
        }

        // regular diagonal capture
        short leftDiagonal = board[startIndex + 7];
        if (col > 0) {
            Move m{startIndex, startIndex + 7, MoveFlag::CAPTURE, leftDiagonal & PieceType};
            if (startIndex + 7 >= 56) {
                for (int i = 2; i <= 5; ++i) {
                    m.capturePromotionData = i;
                    addMove(m);
                }
            } else {
                addMove(m);
            }
        }

        short rightDiagonal = board[startIndex + 9];
        if (col < 7) {
            Move m{startIndex, startIndex + 9, MoveFlag::CAPTURE, rightDiagonal & PieceType};
            if (startIndex + 9 >= 56) {
                for (int i = 2; i <= 5; ++i) {
                    m.capturePromotionData = i;
                    addMove(m);
                }
            } else {
                addMove(m);
            }
        }

        if (previousMoves.empty()) return;
        int prevStart = previousMoves.back().start();
        int prevEnd = previousMoves.back().target();

        bool isEnPassant = prevStart >= 48 && prevStart <= 55 && prevEnd >= 32 && prevEnd <= 39;
        // en passant
        if (row == 4 && isEnPassant) {  // en passant for white only on row 4
            // left capture check
            if (col > 0 && board[startIndex - 1] == (Pawn | Black) && startIndex - 1 == prevEnd) {
                addMove({startIndex, startIndex + 7, MoveFlag::ENPASSANT});
            }
            // right capture check
            if (col < 7 && board[startIndex + 1] == (Pawn | Black) && startIndex + 1 == prevEnd) {
                addMove({startIndex, startIndex + 9, MoveFlag::ENPASSANT});
            }
        }
    } else {                                                                                         // black pawn
        if (board[startIndex - 8] == NoPiece) {                                                         // initial position
            if (row == 6 && board[startIndex - 16] == NoPiece) addMove({startIndex, startIndex - 16});  // move two
            if (row > 1)
                addMove({startIndex, startIndex - 8});
            else {
                for (int i = 2; i <= 5; ++i) addMove({startIndex, startIndex - 8, MoveFlag::PROMOTION, i});
            }
        }

        // regular diagonal capture
        short leftDiagonal = board[startIndex - 9];
        if (col > 0) {
            Move m{startIndex, startIndex - 9, MoveFlag::CAPTURE, leftDiagonal & PieceType};
            if (startIndex - 9 <= 7) {
                for (int i = 2; i <= 5; ++i) {
                    m.capturePromotionData = i;
                    addMove(m);
                }
            } else {
                addMove(m);
            }
        }

        short rightDiagonal = board[startIndex - 7];
        if (col < 7) {
            Move m{startIndex, startIndex - 7, MoveFlag::CAPTURE, rightDiagonal & PieceType};
            if (startIndex - 7 <= 7) {
                for (int i = 2; i <= 5; ++i) {
                    m.capturePromotionData = i;
                    addMove(m);
                }
            } else {
                addMove(m);
            }
        }

        if (previousMoves.empty()) return;
        int prevStart = previousMoves.back().start();
        int prevEnd = previousMoves.back().target();

        bool isEnPassant = prevStart >= 8 && prevStart <= 15 && prevEnd >= 24 && prevEnd <= 31;
        // en passant
        if (row == 3 && isEnPassant) {  // en passant for black only on row 5
            // left capture check
            if (col > 0 && board[startIndex - 1] == (Pawn | White) && startIndex - 1 == prevEnd) {
                addMove({startIndex, startIndex - 9, MoveFlag::ENPASSANT});
            }
            // right capture check
            if (col < 7 && board[startIndex + 1] == (Pawn | White) && startIndex + 1 == prevEnd) {
                addMove({startIndex, startIndex - 7, MoveFlag::ENPASSANT});
            }
        }
    }
}

void Chess::genKnightMoves(short startIndex) {
    // cout << "Gen knight moves, for index " << startIndex << "\n";
    int row = startIndex / 8, col = startIndex % 8;
    int offsets[8][2] = {{2, 1}, {1, 2}, {-1, 2}, {-2, 1}, {-2, -1}, {-1, -2}, {1, -2}, {2, -1}};
    for (int i = 0; i < 8; ++i) {
        int newRow = row + offsets[i][0];
        int newCol = col + offsets[i][1];

        if (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8) {
            int targetIndex = newRow * 8 + newCol;
            Piece targetPiece = Piece(board[targetIndex] & PieceType);
            addMove({row * 8 + col, targetIndex, MoveFlag::CAPTURE, targetPiece});
        }
    }
}

void Chess::genSlidingMoves(short startIndex) {
    int row = startIndex / 8, col = startIndex % 8;
    int offsets[8][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {-1, -1}, {1, -1}, {-1, 1}};

    Piece piece = Piece(board[startIndex] & PieceType);
    int begin = (piece == Queen || piece == Rook) ? 0 : 4;
    int end = (piece == Queen || piece == Bishop) ? 8 : 4;

    for (int i = begin; i < end; ++i) {
        int newRow = row + offsets[i][0];
        int newCol = col + offsets[i][1];

        while (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8) {
            int targetIndex = newRow * 8 + newCol;
            Piece targetPiece = Piece(board[targetIndex] & PieceType);

            addMove({startIndex, targetIndex, MoveFlag::CAPTURE, targetPiece});
            if (targetPiece != NoPiece) break;

            newRow += offsets[i][0];
            newCol += offsets[i][1];
        }
    }
}
