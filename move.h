#ifndef MOVE_H
#define MOVE_H
struct Move {
    bool isWhite;          // True if white, false if black
    int pieceType;         // 0: Pawn, 1: Rook, 2: Bishop, 3: Queen, 4: Knight, 5: King
    int startSquare;       // 0-63
    int endSquare;         // 0-63
    int pieceCaptured;     // -1: None, otherwise piece type
    char promotion;        // 'Q', 'R', 'B', 'N' for pawn promotions, '-' otherwise

    Move() : isWhite(true), pieceType(-1), startSquare(-1), endSquare(-1), pieceCaptured(-1), promotion('-') {}
    Move(bool iw, int pt, int ss, int es, int pc, char promo = '-') 
        : isWhite(iw), pieceType(pt), startSquare(ss), endSquare(es), pieceCaptured(pc), promotion(promo) {}
};
#endif
