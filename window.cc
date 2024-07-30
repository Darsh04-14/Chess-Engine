#include "window.h"
#include <iostream>
#include <cstring>

ChessWindow::ChessWindow() {
    display = XOpenDisplay(NULL);
    if (!display) {
        std::cerr << "Unable to open X display" << std::endl;
        exit(1);
    }

    screen = DefaultScreen(display);
    // Create window with more space to fit the board and margins
    win = XCreateSimpleWindow(display, RootWindow(display, screen), 10, 10, 720 + 100, 720 + 100, 0, 0x0000FF, WhitePixel(display, screen));

    XSelectInput(display, win, ExposureMask | KeyPressMask);
    gc = XCreateGC(display, win, 0, NULL);
    XSetForeground(display, gc, BlackPixel(display, screen));

    // Load a larger and bolder font and set it for the graphics context
    XFontStruct *font = XLoadQueryFont(display, "-*-helvetica-bold-r-*-*-24-*-*-*-*-*-*-*");
    if (!font) {
        std::cerr << "Unable to load font" << std::endl;
        exit(1);
    }
    XSetFont(display, gc, font->fid);

    XMapWindow(display, win);
    XFlush(display);
}

ChessWindow::~ChessWindow() {
    XFreeGC(display, gc);
    XDestroyWindow(display, win);
    XCloseDisplay(display);
}

void ChessWindow::clear() {
    XClearWindow(display, win);
}

void ChessWindow::drawSquare(int x, int y, int size, bool isWhite) {
    XSetForeground(display, gc, isWhite ? 0xE5E6D5 : 0x799359); // Light and dark green for the squares
    XFillRectangle(display, win, gc, x, y, size, size);
}

void ChessWindow::drawPiece(int x, int y, int size, char piece, bool isBlack) {
    // Draw black outline
    XSetForeground(display, gc, 0x000000); // Black color for outline
    XFontStruct *font = XQueryFont(display, XGContextFromGC(gc));
    int textWidth = XTextWidth(font, &piece, 1);
    int textHeight = font->ascent + font->descent;
    int textX = x + (size - textWidth) / 2;
    int textY = y + (size + font->ascent) / 2; // Center vertically

    // Draw outline by drawing the text multiple times with offset
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            if (dx != 0 || dy != 0) { // Skip drawing at the same position
                XDrawString(display, win, gc, textX + dx, textY + dy, &piece, 1);
            }
        }
    }

    // Draw piece in desired color
    XSetForeground(display, gc, isBlack ? 0x000000 : 0xFFFFFF); // White color for white pieces
    XDrawString(display, win, gc, textX, textY, &piece, 1);
}

void ChessWindow::drawText(int x, int y, const char* text) {
    XSetForeground(display, gc, 0x799359); // Brown color for text
    XDrawString(display, win, gc, x, y, text, strlen(text));
}

void drawBoard(ChessWindow& window, const Chess& game, int squareSize) {
    int offset = 50; // Increased offset for the labels around the board
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            bool isWhite = (row + col) % 2 == 0;
            // Adjusting coordinates to match chessboard orientation
            window.drawSquare(col * squareSize + offset, (7 - row) * squareSize + offset, squareSize, isWhite);
            int piece = game.getPieceAt(row, col);
            if (piece != game.getEmpty()) {
                char pieceChar;
                bool isBlack = piece & game.getBlack();
                switch (piece & PieceType) {
                    case King:   pieceChar = isBlack ? 'k' : 'K'; break;
                    case Queen:  pieceChar = isBlack ? 'q' : 'Q'; break;
                    case Knight: pieceChar = isBlack ? 'n' : 'N'; break;
                    case Bishop: pieceChar = isBlack ? 'b' : 'B'; break;
                    case Rook:   pieceChar = isBlack ? 'r' : 'R'; break;
                    case Pawn:   pieceChar = isBlack ? 'p' : 'P'; break;
                    default:     pieceChar = ' '; break;
                }
                window.drawPiece(col * squareSize + offset, (7 - row) * squareSize + offset, squareSize, pieceChar, isBlack);
            }
        }
    }

    // Adjusting text drawing coordinates
    for (int col = 0; col < 8; ++col) {
        char letter[2] = {static_cast<char>('a' + col), '\0'};
        window.drawText(col * squareSize + squareSize / 2 + offset - 8, 8 * squareSize + offset + 24, letter); // bottom letters
    }

    for (int row = 0; row < 8; ++row) {
        char number[2] = {static_cast<char>('1' + (7 - row)), '\0'};
        window.drawText(offset / 2 - 8, row * squareSize + squareSize / 2 + offset + 8, number); // left numbers
    }
}
