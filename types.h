#ifndef TYPES_H
#define TYPES_H

// The special piece is simply needed to make the castle and en passant flag bits
enum Piece { NoPiece, King, Queen, Knight, Bishop, Rook, Pawn, Special };
enum Colour { White = 8, Black = 16 };
enum MoveFlag { CAPTURE, PROMOTION, ENPASSANT = 14, CASTLE = 15 };

#endif
