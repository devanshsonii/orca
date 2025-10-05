#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include <string>
#include <algorithm>
#include <cstdint>
using namespace std;


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

class Board { 
private:
    // white-centric
    uint64_t whitePawn = 0;
    uint64_t whiteBishop = 0;
    uint64_t whiteKnight = 0;
    uint64_t whiteRook = 0;
    uint64_t whiteKing = 0;
    uint64_t whiteQueen = 0;
    uint64_t whitePieces = 0;

    uint64_t blackPawn = 0;
    uint64_t blackBishop = 0;
    uint64_t blackKnight = 0;
    uint64_t blackRook = 0;
    uint64_t blackKing = 0;
    uint64_t blackQueen = 0;
    uint64_t blackPieces = 0;
    
    int knight_pst[64] = {
        -167, -89, -34, -49,  61, -97, -15, -107,
        -73, -41,  72,  36,  23,  62,   7,  -17,
        -47,  60,  37,  65,  84, 129,  73,   44,
        -9,  17,  19,  53,  37,  69,  18,   22,
        -13,   4,  16,  13,  28,  19,  21,   -8,
        -23,  -9,  12,  10,  19,  17,  25,  -16,
        -29, -53, -12,  -3,  -1,  18, -14,  -19,
        -105, -21, -58, -33, -17, -28, -19,  -23,
    };

    int pawn_pst[64] = {
        0,   0,   0,   0,   0,   0,  0,   0,
        98, 134,  61,  95,  68, 126, 34, -11,
        -6,   7,  26,  31,  65,  56, 25, -20,
        -14,  13,   6,  21,  23,  12, 17, -23,
        -27,  -2,  -5,  12,  17,   6, 10, -25,
        -26,  -4,  -4, -10,   3,   3, 33, -12,
        -35,  -1, -20, -23, -15,  24, 38, -22,
        0,   0,   0,   0,   0,   0,  0,   0,
    };

    int bishop_pst[64] = {
        -29,   4, -82, -37, -25, -42,   7,  -8,
        -26,  16, -18, -13,  30,  59,  18, -47,
        -16,  37,  43,  40,  35,  50,  37,  -2,
        -4,   5,  19,  50,  37,  37,   7,  -2,
        -6,  13,  13,  26,  34,  12,  10,   4,
        0,  15,  15,  15,  14,  27,  18,  10,
        4,  15,  16,   0,   7,  21,  33,   1,
        -33,  -3, -14, -21, -13, -12, -39, -21,
    };

    int rook_pst[64] = {
        32,  42,  32,  51, 63,  9,  31,  43,
        27,  32,  58,  62, 80, 67,  26,  44,
        -5,  19,  26,  36, 17, 45,  61,  16,
        -24, -11,   7,  26, 24, 35,  -8, -20,
        -36, -26, -12,  -1,  9, -7,   6, -23,
        -45, -25, -16, -17,  3,  0,  -5, -33,
        -44, -16, -20,  -9, -1, 11,  -6, -71,
        -19, -13,   1,  17, 16,  7, -37, -26,
    };
    
    int queen_pst[64] = {
        -28,   0,  29,  12,  59,  44,  43,  45,
        -24, -39,  -5,   1, -16,  57,  28,  54,
        -13, -17,   7,   8,  29,  56,  47,  57,
        -27, -27, -16, -16,  -1,  17,  -2,   1,
        -9, -26,  -9, -10,  -2,  -4,   3,  -3,
        -14,   2, -11,  -2,  -5,   2,  14,   5,
        -35,  -8,  11,   2,   8,  15,  -3,   1,
        -1, -18,  -9,  10, -15, -25, -31, -50,
    };
    
    int king_pst[64] = {
        -65,  23,  16, -15, -56, -34,   2,  13,
        29,  -1, -20,  -7,  -8,  -4, -38, -29,
        -9,  24,   2, -16, -20,   6,  22, -22,
        -17, -20, -12, -27, -30, -25, -14, -36,
        -49,  -1, -27, -39, -46, -44, -33, -51,
        -14, -14, -22, -46, -44, -30, -15, -27,
        1,   7,  -8, -64, -43, -16,   9,   8,
        -15,  36,  12, -54,   8, -28,  24,  14,
    };

    int* psts[6] = {
        pawn_pst,
        rook_pst,
        bishop_pst,
        queen_pst,
        knight_pst,
        king_pst
    };

    bool whiteCanCastleKingSide;
    bool whiteCanCastleQueenSide;
    bool blackCanCastleKingSide;
    bool blackCanCastleQueenSide;
    int enPassantSquare; 
    int halfmoveClock;
    int fullmoveNumber;

    uint8_t allPieces[64];

    uint64_t knight_attacks[64] = {
        0x20400ULL, 0x50800ULL, 0xa1100ULL, 0x142200ULL, 0x284400ULL, 0x508800ULL, 0xa01000ULL, 0x402000ULL,
        0x2040004ULL, 0x5080008ULL, 0xa110011ULL, 0x14220022ULL, 0x28440044ULL, 0x50880088ULL, 0xa0100010ULL, 0x40200020ULL,
        0x204000402ULL, 0x508000805ULL, 0xa1100110aULL, 0x1422002214ULL, 0x2844004428ULL, 0x5088008850ULL, 0xa0100010a0ULL, 0x4020002040ULL,
        0x20400040200ULL, 0x50800080500ULL, 0xa1100110a00ULL, 0x142200221400ULL, 0x284400442800ULL, 0x508800885000ULL, 0xa0100010a000ULL, 0x402000204000ULL,
        0x2040004020000ULL, 0x5080008050000ULL, 0xa1100110a0000ULL, 0x14220022140000ULL, 0x28440044280000ULL, 0x50880088500000ULL, 0xa0100010a00000ULL, 0x40200020400000ULL,
        0x204000402000000ULL, 0x508000805000000ULL, 0xa1100110a000000ULL, 0x1422002214000000ULL, 0x2844004428000000ULL, 0x5088008850000000ULL, 0xa0100010a0000000ULL, 
        0x4020002040000000ULL, 0x400040200000000ULL, 0x800080500000000ULL, 0x1100110a00000000ULL, 0x2200221400000000ULL, 0x4400442800000000ULL, 0x8800885000000000ULL,  0x100010a000000000ULL,
        0x2000204000000000ULL, 0x4020000000000ULL, 0x8050000000000ULL, 0x110a0000000000ULL, 0x22140000000000ULL, 0x44280000000000ULL, 0x88500000000000ULL, 0x10a00000000000ULL, 0x20400000000000ULL
    };

    bool turn;

public:
    Move bestMove;

    int moveSearched = 0;

    Board() {}

    void printGame();

    void setupGameFromFEN(string moveSet);

    void outputFen();

    void generatePieceMoves(bool isWhite, Move moveArray[], int &moveCount);

    void printMoves(Move moveArray[], int moveCount);

    long perft(int depth, bool isWhite, int initialDepth);
    
    void make_move(Move& mv);

    void unmake_move(Move & mv);

    void order_moves(Move *moves, int moveCount);

    int evaluateBoard();

    int alphaBeta(bool isWhite, int depth, int alpha, int beta);

    long perft2(int depth, bool isWhite);

    // new stuff added, remove if engine breaks

    bool isSquareAttacked(int square, bool byWhite);

    bool isInCheck(bool isWhite);

    bool isMoveLegal(Move &mv);

    bool play(string moveStr);

    string indexToAlgebraic(int index) {
        if(index < 0 || index > 63) return "??";
        char file = 'a' + (index % 8);
        char rank = '1' + 7-(index / 8);
        return string(1, file) + string(1, rank);
    }
    
    int algebraicToIndex(string square);
    
    Move search(int depth);
    
    bool getTurn() { return turn; }

    int getPieceAt(int square);

};

#endif