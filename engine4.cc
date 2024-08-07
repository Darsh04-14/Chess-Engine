#include "engine4.h"

bool Engine4::cmp::operator()(const Move& lhs, const Move& rhs) {
    bool captureA = lhs.isCapture() || lhs.isCapturePromotion();
    bool captureB = rhs.isCapture() || rhs.isCapturePromotion();

    if (!captureA || !captureB) return captureA;

    int valueA = (lhs.isCapture() ? lhs.piece() : lhs.capturePromotionData) - (board[lhs.start()] & PieceType);
    int valueB = (rhs.isCapture() ? rhs.piece() : rhs.capturePromotionData) - (board[rhs.start()] & PieceType);

    return valueA > valueB;
}

int Engine4::boardEvaluation() {
    Colour c = chess->getCurrentPlayer();
    Colour enemyColour = Colour(c ^ ColourType);
    vector<short> board = chess->getBoard();

    int score = 0;
    for (int i = 0; i < board.size(); ++i) {
        Piece piece = Piece(board[i] & PieceType);
        Colour pieceColour = Colour(board[i] & ColourType);

        if (piece != NoPiece) {
            if (pieceColour == c) {
                if (!chess->isSquareAttacked(enemyColour, i)) {
                    score += pieceValue[piece - 1];
                } else if (chess->isSquareAttacked(c, i)) {
                    score += int(pieceValue[piece - 1] / (1.4));
                } else {
                    score += pieceValue[piece - 1] / 4;
                }
                if (chess->movesPlayed() >= 15 && piece == Pawn) {
                    score += (i / 8) * 10;
                }
            } else {
                if (!chess->isSquareAttacked(c, i)) {
                    score -= pieceValue[piece - 1];
                } else if (chess->isSquareAttacked(enemyColour, i)) {
                    score -= int(pieceValue[piece - 1] / (1.6));
                } else {
                    score -= pieceValue[piece - 1] / 16;
                }
                if (chess->movesPlayed() >= 15 && piece == Pawn) {
                    score -= (i / 8) * 10;
                }
            }
        }

        if (chess->isSquareAttacked(c, i)) score += SQUARE_VALUE;
        if (chess->isSquareAttacked(enemyColour, i)) score -= SQUARE_VALUE;
    }

    return score;
}

int Engine4::moveEvaluation(int depth, int alpha, int beta, int moveCounter = 0) {
    chess->generateLegalMoves();
    if (chess->end()) return chess->end() != (White | Black) ? (-2e6 + moveCounter) : 0;

    if (chess->check() && moveCounter <= MAX_DEPTH + 10) depth += 1;

    if (!depth) {
        ++nodeCount;
        return boardEvaluation();
    }

    vector<Move> currentMoves = chess->getLegalMoves();
    cmp c{chess->getBoard()};
    sort(currentMoves.begin(), currentMoves.end(), c);

    int valuation = -2e6;
    for (int i = 0; i < currentMoves.size(); ++i) {
        chess->makeMove(currentMoves[i]);
        valuation = max(-moveEvaluation(depth - 1, -beta, -alpha, moveCounter + 1), valuation);
        chess->unmakeMove();
        alpha = max(alpha, valuation);
        if (valuation >= beta) break;
    }

    return valuation;
}

bool Engine4::notify() {
    string cmd;
    cin >> cmd;
    if (cmd == "move") {
        vector<Move> currentMoves = chess->getLegalMoves();

        cmp c{chess->getBoard()};
        sort(currentMoves.begin(), currentMoves.end(), c);

        if (!currentMoves.size()) return false;

        Move bestMove = currentMoves[0];

        nodeCount = 0;
        int alpha = -2e6;
        for (int i = 0; i < currentMoves.size(); ++i) {
            chess->makeMove(currentMoves[i]);
            int value = -moveEvaluation(MAX_DEPTH, -2e6, -alpha);
            if (value > alpha) {
                bestMove = currentMoves[i];
                alpha = value;
            }
            chess->unmakeMove();
        }

        cout << "Best evaluation: " << alpha << " nodes searched " << nodeCount << "\n";

        chess->makeMove(bestMove);
        chess->generateLegalMoves();
        return true;
    } else if (cin.fail()) {
        chess->draw();
    } else
        cout << "Invalid command!\n";
    return false;
}
