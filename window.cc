#include "window.h"
#include <iostream>

ChessWindow::ChessWindow() {
    display = XOpenDisplay(NULL);
    if (!display) {
        std::cerr << "Unable to open X display" << std::endl;
        exit(1);
    }

    screen = DefaultScreen(display);
    win = XCreateSimpleWindow(display, RootWindow(display, screen), 10, 10, 720, 720, 0,
                              0x0000FF, WhitePixel(display, screen));

    XSelectInput(display, win, ExposureMask | KeyPressMask);
    gc = XCreateGC(display, win, 0, NULL);
    XSetForeground(display, gc, BlackPixel(display, screen));

    XMapWindow(display, win);
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
    XSetForeground(display, gc, isWhite ? 0xE5E6D5 :  0x799359);
    XFillRectangle(display, win, gc, x, y, size, size);
}

void ChessWindow::drawPiece(int x, int y, char piece, bool isBlack) {
    unsigned long color = isBlack ?  0x000000 : 0x000000;
    XSetForeground(display, gc, color);
    XDrawString(display, win, gc, x + 10, y + 30, &piece, 1);
}


void ChessWindow::drawText(int x, int y, const char* text) {
    XSetForeground(display, gc, BlackPixel(display, screen));
    XDrawString(display, win, gc, x, y, text, strlen(text));
}

void drawBoard(ChessWindow& window, const Chess& game, int squareSize) {
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            bool isWhite = (row + col) % 2 == 0;
            window.drawSquare((col + 1) * squareSize, (7 - row + 1) * squareSize, squareSize, isWhite);
            int piece = game.getPiece(row, col);
            if (piece != game.getEmpty()) {
                char pieceChar = piece & ((1 << 7) - 1);
                bool isBlack = piece & game.getBlack();
                if (isBlack) {
                    pieceChar = tolower(pieceChar);
                }
                window.drawPiece((col + 1) * squareSize, (7 - row + 1) * squareSize, pieceChar, isBlack);
            }
        }
    }

for (int col = 0; col < 8; ++col) {
    char letter = 'a' + col;
    window.drawText((col + 1) * squareSize + squareSize / 2, 9 * squareSize + 20, &letter); // bottom letters
    window.drawText((col + 1) * squareSize + squareSize / 2, squareSize - 10, &letter); // top letters
}

for (int row = 0; row < 8; ++row) {
    char number = '1' + row;
    window.drawText(10, (8 - row) * squareSize + squareSize / 2, &number); // left numbers
    window.drawText(9 * squareSize + 10, (8 - row) * squareSize + squareSize / 2, &number); // right numbers
}

}
