#ifndef TYPES_H
#define TYPES_H

enum Piece { NoPiece, King, Pawn, Knight, Bishop, Rook, Queen };

enum Colour { White = 8, Black = 16 };

enum MoveFlag { CAPTURE, PROMOTION, CAPTURE_PROMOTION, DOUBLE_PAWN_PUSH, ENPASSANT, LEFT_CASTLE, RIGHT_CASTLE };

#endif
