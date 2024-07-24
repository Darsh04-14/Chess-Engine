#include <vector>
using namespace std;

#include "chess.h"

void Chess::getAttackSquares() {
    getAttackMasks = true;

    attackMask[0] = attackMask[1] = 0;

    for (int i = 0; i < 2; ++i) {
        generatePseudoLegalMoves();
        colourToMove = Colour(colourToMove ^ ColourType);
    }

    getAttackMasks = false;
}

bool Chess::isSquareAttacked(short colour, short index) { return attackMask[colour >> 4] & ((ULL)1 << index); }

Piece Chess::getPiece(char a) {
    a = tolower(a);
    if (a == 'r')
        return Piece::Rook;
    else if (a == 'b')
        return Piece::Bishop;
    else if (a == 'n')
        return Piece::Knight;
    else if (a == 'q')
        return Piece::Queen;
    else if (a == 'k')
        return Piece::King;
    else if (a == 'p')
        return Piece::Pawn;
    else
        return Piece::None;
}

short Chess::getKing(Colour c) {
    for (int i = 0; i < 64; ++i)
        if ((c | King) == board[i]) return i;
    return -1;  // Should never happen
}

void Chess::generatePseudoLegalMoves() {
    legalMovesLen = 0;
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            short index = i * 8 + j;
            if (board[index] == None)
                continue;
            else if (board[index] == (colourToMove | Pawn))
                genPawnMoves(index);
            else if (board[index] == (colourToMove | King))
                genKingMoves(index);
            else if (board[index] == (colourToMove | Knight))
                genKnightMoves(index);
            else if ((board[index] & ColourType) == colourToMove)
                genSlidingMoves(index);
        }
    }
    genCastleMove();
}

void Chess::generateLegalMoves() {
    getAttackSquares();
    generatePseudoLegalMoves();

    Move pseudoLegalMoves[225];

    short currentMoveLen = legalMovesLen;

    for (int i = 0; i < legalMovesLen; ++i) pseudoLegalMoves[i] = legalMoves[i];

    short currentkingPosition = getKing(colourToMove);

    int j = 0;
    for (int i = 0; i < currentMoveLen; ++i) {
        short kingPostion = currentkingPosition;
        if (pseudoLegalMoves[i].start() == kingPostion) kingPostion = pseudoLegalMoves[i].target();

        makeMove(pseudoLegalMoves[i]);
        generatePseudoLegalMoves();

        bool legalMove = true;
        for (int k = 0; k < legalMovesLen; ++k) {
            if (legalMoves[k].target() == kingPostion) {
                legalMove = false;
                break;
            }
        }

        if (legalMove) {
            pseudoLegalMoves[j++] = pseudoLegalMoves[i];
        }

        unmakeMove();
    }

    for (int i = 0; i < j; ++i) {
        legalMoves[i] = pseudoLegalMoves[i];
    }
    legalMovesLen = j;
}

void Chess::addMove(const Move& m) {
    if (!getAttackMasks && (board[m.start()] & ColourType) != (board[m.target()] & ColourType)) {
        // Make sure if it's a pawn diagonal capture then it's actually capturing something
        bool isDiagonal = abs(abs(m.target() - m.start()) - 8) == 1;
        if ((board[m.start()] & PieceType) == Pawn && isDiagonal && !m.isEnPassant() && m.isCapture() == None) return;
        legalMoves[legalMovesLen++] = m;
    } else {
        bool isPawnStraight = (board[m.start()] & PieceType) == Pawn && (m.target() - m.start()) % 8 == 0;
        if (isPawnStraight || m.isEnPassant() || m.isCastle()) return;
        attackMask[colourToMove >> 4] |= ((ULL)1 << m.target());
    }
}

Chess::Chess() : legalMovesLen{0} {
    memset(castlingRights, -1, sizeof(castlingRights));
    memset(board, 0, sizeof(board));
}

Chess::Chess(string FEN) : Chess() {
    int row = 7, col = 0, i = 0;

    // Position segment of FEN
    for (; i < FEN.length() && FEN[i] != ' '; ++i) {
        if (FEN[i] == '/') {
            row -= 1;
            col = 0;
        } else if (isdigit(FEN[i])) {
            col += FEN[i] - '0';
        } else if (islower(FEN[i])) {
            board[row * 8 + col++] = Black | getPiece(FEN[i]);
        } else {
            board[row * 8 + col++] = White | getPiece(FEN[i]);
        }
    }

    // Colour to move flag
    if (++i >= FEN.length()) {
        colourToMove = Colour::White;
    } else {
        colourToMove = FEN[i] == 'w' ? Colour::White : Colour::Black;
    }

    i += 2;
    fill(&castlingRights[0][0], &castlingRights[0][0] + 4, -2);
    if (i < FEN.length() && FEN[i] != '-') {
        // Castling rights
        for (; i < FEN.length() && FEN[i] != ' '; ++i) {
            bool colour = (isupper(FEN[i]) ? White : Black) >> 4;
            bool side = tolower(FEN[i]) == 'q' ? 0 : 1;
            castlingRights[colour][side] = -1;
        }
    } else {
        ++i;
    }

    ++i;

    if (i < FEN.length() && FEN[i] != '-') {
        short targetIndex = (FEN[i + 1] - '1') * 8 + FEN[i] - 'a';
        if (FEN[i + 1] == '3') {
            targetIndex += 8;
            previousMoves.push_back({targetIndex - 16, targetIndex});
        } else {
            targetIndex -= 8;
            previousMoves.push_back({targetIndex + 16, targetIndex});
        }
    }

    generateLegalMoves();
}

void Chess::makeMove(const Move& takenMove) {
    // Check for win
    previousMoves.push_back(takenMove);
    Colour enemyColour = Colour(colourToMove ^ ColourType);
    Piece startPiece = Piece(board[takenMove.start()] & PieceType);
    Piece targetPiece = Piece(takenMove.piece());

    board[takenMove.target()] = board[takenMove.start()];
    board[takenMove.start()] = None;

    bool kingMove = startPiece == King;
    bool rookMove = startPiece == Rook;

    if (takenMove.isCastle()) {
        if (castlingRights[colourToMove >> 4][0] == -1) castlingRights[colourToMove >> 4][0] = previousMoves.size();
        if (castlingRights[colourToMove >> 4][1] == -1) castlingRights[colourToMove >> 4][1] = previousMoves.size();

        short offset = (takenMove.target() > takenMove.start() ? 1 : -2);
        short rookIndex = takenMove.target() + offset;

        board[rookIndex] = None;
        offset = -offset + (offset < 0 ? 1 : -1);

        short rookTargetIndex = rookIndex + offset;
        board[rookTargetIndex] = colourToMove | Rook;
    } else if (takenMove.isCapture() || takenMove.isCapturePromotion() || kingMove || rookMove) {
        if (castlingRights[colourToMove >> 4][0] == -1) {
            if (kingMove) castlingRights[colourToMove >> 4][0] = previousMoves.size();
            bool side = (takenMove.start()) % 2;
            if (rookMove && side == 0) castlingRights[colourToMove >> 4][0] = previousMoves.size();
        }

        if (castlingRights[colourToMove >> 4][1] == -1) {
            if (kingMove) castlingRights[colourToMove >> 4][1] = previousMoves.size();
            bool side = (takenMove.start()) % 2;
            if (rookMove && side == 1) castlingRights[colourToMove >> 4][1] = previousMoves.size();
        }

        if (castlingRights[enemyColour >> 4][0] == -1) {
            bool leftRookCapture = (enemyColour == White ? 0 : 56) == takenMove.target();
            if (leftRookCapture) castlingRights[enemyColour >> 4][0] = previousMoves.size();
        }
        if (castlingRights[enemyColour >> 4][1] == -1) {
            bool rightRookCapture = (enemyColour == White ? 7 : 63) == takenMove.target();
            if (rightRookCapture) castlingRights[enemyColour >> 4][1] = previousMoves.size();
        }

        if (takenMove.isCapturePromotion()) {
            board[takenMove.target()] = colourToMove | takenMove.capturePromotionData;
        }
    } else if (takenMove.isEnPassant()) {
        short offset = takenMove.target() - takenMove.start() + (colourToMove == White ? -8 : 8);
        board[takenMove.start() + offset] = None;
    } else if (takenMove.isPromotion()) {
        board[takenMove.target()] = colourToMove | targetPiece;
    }
    colourToMove = Colour(colourToMove ^ ColourType);
}

void Chess::print() {
    const char letters[] = {'K', 'Q', 'N', 'B', 'R', 'P'};
    for (int row = 7; row >= 0; --row) {
        cout << row + 1 << ' ';
        for (int col = 0; col < 8; ++col) {
            short p = board[row * 8 + col];
            if ((p & PieceType) == None) {
                cout << ((row + col + 1) % 2 ? '-' : ' ');
            } else {
                char Letter = letters[(p & PieceType) - 1];
                cout << ((p & Black) ? char(Letter + 32) : Letter);
            }
        }
        cout << "\n";
    }
    cout << "  abcdefgh\n";
}

bool Chess::playMove(short startIndex, short targetIndex) {
    for (int i = 0; i < legalMovesLen; ++i) {
        Move move = legalMoves[i];
        if (move.start() == startIndex && move.target() == targetIndex) {
            if (move.isPromotion() || move.isCapturePromotion()) {
                cout << "Enter which piece you want to promote to (q,n,b,r): ";
                char p;
                do {
                    cin >> p;
                    p = tolower(p);
                } while (p != 'q' && p != 'n' && p != 'b' && p != 'r');
                Piece newPiece = getPiece(p);
                if (move.isPromotion()) {
                    makeMove({startIndex, targetIndex, MoveFlag::PROMOTION, newPiece});
                } else {
                    move.capturePromotionData = newPiece;
                    makeMove(move);
                }
            } else {
                makeMove(move);
            }
            legalMovesLen = 0;
            generateLegalMoves();
            checkGameState();
            return true;
        }
    }
    cout << "Invalid Move!\n";
    return false;
}

bool Chess::playMove(string start, string target) {
    if (start.length() != 2 || target.length() != 2) return false;

    short startIndex = (start[1] - '1') * 8 + start[0] - 'a';
    short targetIndex = (target[1] - '1') * 8 + target[0] - 'a';

    if (startIndex < 0 || startIndex > 63 || targetIndex < 0 || targetIndex > 63) return false;

    return playMove(startIndex, targetIndex);
}

void Chess::unmakeMove() {
    if (!previousMoves.size()) return;

    Colour prevColour = Colour(colourToMove ^ ColourType);

    Move lastMove = previousMoves.back();
    Piece targetPiece = Piece(lastMove.piece());
    short startIndex = lastMove.start();
    short targetIndex = lastMove.target();

    board[startIndex] = board[targetIndex];
    board[targetIndex] = None;

    if (lastMove.isCastle()) {
        short rookStartIndex = startIndex + (targetIndex > startIndex ? 3 : -4);
        short rookTargetIndex = targetIndex + (targetIndex > startIndex ? -1 : 1);
        swap(board[rookStartIndex], board[rookTargetIndex]);
    } else if (lastMove.isEnPassant()) {
        short offset = targetIndex - startIndex + (prevColour == White ? -8 : 8);
        board[startIndex + offset] = colourToMove | Pawn;
    } else if (lastMove.isPromotion()) {
        board[startIndex] = prevColour | Pawn;
    } else if (lastMove.piece()) {
        board[targetIndex] = colourToMove | targetPiece;
    }

    if (lastMove.isCapturePromotion()) {
        board[startIndex] = prevColour | Pawn;
    }

    // Give back castling rights
    for (int i = 0; i < 2; ++i) {
        if (castlingRights[i][0] == previousMoves.size()) castlingRights[i][0] = -1;
        if (castlingRights[i][1] == previousMoves.size()) castlingRights[i][1] = -1;
    }

    previousMoves.pop_back();
    colourToMove = prevColour;
    if (gameOver) gameOver = false;
}

int Chess::perft(int depth, int debug) {
    if (!depth) {
        return 1;
    }

    generateLegalMoves();

    Move* currentlegalMoves = new Move[312];

    short currentMoveLen = legalMovesLen;

    for (int i = 0; i < legalMovesLen; ++i) {
        currentlegalMoves[i] = legalMoves[i];
    }
    int count = 0;
    for (int i = 0; i < currentMoveLen; ++i) {
        short s = currentlegalMoves[i].start(), t = currentlegalMoves[i].target();
        makeMove(currentlegalMoves[i]);
        int n = perft(depth - 1, debug);
        count += n;
        unmakeMove();
        if (depth == debug) {
            cout << char(s % 8 + 'a') << char(s / 8 + '1') << char(t % 8 + 'a') << char(t / 8 + '1') << ": " << n
                 << "\n";
        }
    }
    delete[] currentlegalMoves;

    return count;
}

void Chess::printLegalMoves() {
    for (int i = 0; i < legalMovesLen; ++i) {
        short s = legalMoves[i].start(), t = legalMoves[i].target();
        cout << char(s % 8 + 'a') << char(s / 8 + '1') << char(t % 8 + 'a') << char(t / 8 + '1') << "\n";
    }
}

void Chess::printCastleRights() {
    cout << "White: " << castlingRights[0][0] << " " << castlingRights[0][1] << "\n";
    cout << "Black: " << castlingRights[1][0] << " " << castlingRights[1][1] << "\n";
}

void Chess::checkGameState() {
    short kingIndex = getKing(colourToMove);
    Colour enemyColour = Colour(colourToMove ^ ColourType);
    string colour = colourToMove == White ? "White" : "Black";

    bool pieceFound = false;
    for (int i = 0; i < 64; ++i) {
        if ((board[i] & PieceType) != King) {
            pieceFound = true;
            break;
        }
    }

    if (!pieceFound) {
        gameOver = true;
        cout << "Stalemate!";
        return;
    }

    if (legalMovesLen) {
        if (isSquareAttacked(enemyColour, kingIndex)) cout << colour << "is in check.\n\n";
    } else if (isSquareAttacked(enemyColour, kingIndex)) {
        gameOver = true;
        cout << "Checkmate! " << colour << " wins!\n\n";
    } else {
        gameOver = true;
        cout << "Stalemate!";
    }
}

bool Chess::end() { return gameOver; }

void Chess::resignPlayer() {
    string enemyColour = colourToMove == White ? "Black" : "White";
    cout << enemyColour << " wins!\n";
}

vector<Move> Chess::getLegalMoves() { return vector<Move>(legalMoves, legalMoves + legalMovesLen); }

Colour Chess::getCurrentPlayer() { return colourToMove; }