#ifndef CHESSWINDOW_H
#define CHESSWINDOW_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "chess.h"

class ChessWindow {
   public:
    Display* display;
    int screen;
    Window win;
    GC gc;

    ChessWindow();
    ~ChessWindow();
    void clear();
    void drawSquare(int x, int y, int size, bool isWhite);
    void drawPiece(int x, int y, int size, char piece, bool isBlack);
    void drawText(int x, int y, const char* text);
};

void drawBoard(ChessWindow& window, const Chess& game, int squareSize);

// https://www.x.org/releases/X11R7.5/doc/libX11/libX11.html

#endif
