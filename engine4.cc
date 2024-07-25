#include "engine4.h"

bool Engine4::cmp::operator()(const Move& lhs, const Move& rhs) {
    bool captureA = lhs.isCapture() || lhs.isCapturePromotion();
    bool captureB = rhs.isCapture() || rhs.isCapturePromotion();

    if (!(captureA & captureB)) return captureA;

    int valueA = lhs.isCapture() ? lhs.piece() : lhs.capturePromotionData;
    int valueB = rhs.isCapture() ? rhs.piece() : rhs.capturePromotionData;

    return valueA > valueB;
}

int Engine4::boardEvaluation() {
    Colour c = chess->getCurrentPlayer();
    vector<short> board = chess->getBoard();
    int score = 0;
    for (int i = 0; i < board.size(); ++i) {
        Piece piece = Piece(board[i] & PieceType);
        Colour pieceColour = Colour(board[i] & ColourType);

        if (piece != None) score = score + (pieceColour == c ? 1 : -1) * int(pieceValue[piece - 1]);
    }
    return score;
}

int Engine4::moveEvaluation(int depth, int alpha, int beta) {
    chess->generateLegalMoves();
    if (chess->end()) return chess->end() ? -2e6 : 0;

    if (depth == MAX_DEPTH) {
        ++nodeCount;
        return boardEvaluation();
    }

    vector<Move> currentMoves = chess->getLegalMoves();
    sort(currentMoves.begin(), currentMoves.end(), cmp());

    int valuation = INT_MIN;
    for (int i = 0; i < currentMoves.size(); ++i) {
        chess->makeMove(currentMoves[i]);
        valuation = max(-moveEvaluation(depth + 1, -beta, -alpha), valuation);
        chess->unmakeMove();
        alpha = max(alpha, valuation);
        if (alpha >= beta) break;
    }

    return valuation;
}

void Engine4::notify() {
    vector<Move> currentMoves = chess->getLegalMoves();

    if (!currentMoves.size()) return;

    Move bestMove = currentMoves[0];
    int bestEvaluation = INT_MIN;

    nodeCount = 0;
    int alpha = -2e6;
    for (int i = 0; i < currentMoves.size(); ++i) {
        chess->makeMove(currentMoves[i]);
        int value = -moveEvaluation(0, alpha, 2e6);
        if (value > bestEvaluation) {
            bestMove = currentMoves[i];
            bestEvaluation = value;
            alpha = bestEvaluation;
        }
        chess->unmakeMove();
    }

    // cout << "Best evaluation: " << bestEvaluation << " | Nodes searched: " << nodeCount << "\n";

    chess->makeMove(bestMove);
    chess->generateLegalMoves();
}