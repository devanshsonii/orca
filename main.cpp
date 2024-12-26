#include <iostream> 
using namespace std;

const uint64_t A_FILE = 0x0101010101010101;
const uint64_t B_FILE = 0x0202020202020202;
const uint64_t C_FILE = 0x0404040404040404;
const uint64_t D_FILE = 0x0808080808080808;
const uint64_t E_FILE = 0x1010101010101010;
const uint64_t F_FILE = 0x2020202020202020;
const uint64_t G_FILE = 0x4040404040404040;
const uint64_t H_FILE = 0x8080808080808080;

const uint64_t RANK_8 = 0x00000000000000FF;
const uint64_t RANK_7 = 0x000000000000FF00;
const uint64_t RANK_6 = 0x0000000000FF0000;
const uint64_t RANK_5 = 0x00000000FF000000;
const uint64_t RANK_4 = 0x000000FF00000000;
const uint64_t RANK_3 = 0x0000FF0000000000;
const uint64_t RANK_2 = 0x00FF000000000000;
const uint64_t RANK_1 = 0xFF00000000000000;

const int DIRECTIONS_STRAIGHT[4] = {8, -8, 1, -1}; // down, up, right, left
const int DIRECTIONS_DIAGONAL[4] = {9, -9, 7, -7}; // bottom-right, top-left, bottom-left, top-right


const int PIECE_NONE = 6;
const int PIECE_PAWN = 0;
const int PIECE_ROOK = 1;
const int PIECE_BISHOP = 2; 
const int PIECE_QUEEN = 3;
const int PIECE_KNIGHT = 4;
const int PIECE_KING = 5;

const uint8_t COLOR_WHITE = 0x8;
const uint8_t COLOR_BLACK = 0;
const uint8_t PIECE_MASK = 0x7;
const uint8_t COLOR_MASK = 0x8;

int initialDepth = 5;

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
    
    int knight_pref[64] = {
        -66, -53, -75, -75, -10, -55, -58, -70,
        -3,  -6, 100, -36,   4,  62,  -4, -14,
        10,  67,   1,  74,  73,  27,  62,  -2,
        24,  24,  45,  37,  33,  41,  25,  17,
        -1,   5,  31,  21,  22,  35,   2,   0,
        -18,  10,  13,  22,  18,  15,  11, -14,
        -23, -15,   2,   0,   2,   0, -23, -20,
        -74, -23, -26, -24, -19, -35, -22, -69
    };

    int pawn_pref[64] = {
        0,   0,   0,   0,   0,   0,   0,   0,
        78,  83,  86,  73, 102,  82,  85,  90,
        7,  29,  21,  44,  40,  31,  44,   7,
        -17,  16,  -2,  15,  14,   0,  15, -13,
        -26,   3,  10,   9,   6,   1,   0, -23,
        -22,   9,   5, -11, -10,  -2,   3, -19,
        -31,   8,  -7, -37, -36, -14,   3, -31,
        0,   0,   0,   0,   0,   0,   0,   0
    };

    int bishop_pref[64] = {
        -59, -78, -82, -76, -23,-107, -37, -50,
        -11,  20,  35, -42, -39,  31,   2, -22,
        -9,  39, -32,  41,  52, -10,  28, -14,
        25,  17,  20,  34,  26,  25,  15,  10,
        13,  10,  17,  23,  17,  16,   0,   7,
        14,  25,  24,  15,   8,  25,  20,  15,
        19,  20,  11,   6,   7,   6,  20,  16,
        -7,   2, -15, -12, -14, -15, -10, -10
    };

    int rook_pref[64] = {
        35,  29,  33,   4,  37,  33,  56,  50,
        55,  29,  56,  67,  55,  62,  34,  60,
        19,  35,  28,  33,  45,  27,  25,  15,
        0,   5,  16,  13,  18,  -4,  -9,  -6,
        -28, -35, -16, -21, -13, -29, -46, -30,
        -42, -28, -42, -25, -25, -35, -26, -46,
        -53, -38, -31, -26, -29, -43, -44, -53,
        -30, -24, -18,   5,  -2, -18, -31, -32
    };
    
    int queen_pref[64] = {
        6,   1,  -8,-104,  69,  24,  88,  26,
        14,  32,  60, -10,  20,  76,  57,  24,
        -2,  43,  32,  60,  72,  63,  43,   2,
        1, -16,  22,  17,  25,  20, -13,  -6,
        -14, -15,  -2,  -5,  -1, -10, -20, -22,
        -30,  -6, -13, -11, -16, -11, -16, -27,
        -36, -18,   0, -19, -15, -15, -21, -38,
        -39, -30, -31, -13, -31, -36, -34, -42
    };
    
    int king_pref[64] = {
        4,  54,  47, -99, -99,  60,  83, -62,
        -32,  10,  55,  56,  56,  55,  10,   3,
        -62,  12, -57,  44, -67,  28,  37, -31,
        -55,  50,  11,  -4, -19,  13,   0, -49,
        -55, -43, -52, -28, -51, -47,  -8, -50,
        -47, -42, -43, -79, -64, -32, -29, -32,
        -4,   3, -14, -50, -57, -18,  13,   4,
        17,  30,  -3, -14,   6,  -1,  40,  18
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

    string indexToAlgebraic(int index) {
        if(index < 0 || index > 63) return "??";
        char file = 'a' + (index % 8);
        char rank = '1' + 7-(index / 8);
        return string(1, file) + string(1, rank);
    }


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



};

void Board::setupGameFromFEN(string moveSet) {
    // Reset all bitboards
    whitePawn = whiteRook = whiteBishop = whiteKnight = whiteQueen = whiteKing = 0;
    blackPawn = blackRook = blackBishop = blackKnight = blackQueen = blackKing = 0;
    
    for(int i = 0; i < 64; i++) allPieces[i] = PIECE_NONE;

    int i = 0, j = 0, index = 0;
    
    // Parse piece placement
    for(char c: moveSet) {
        index++;
        int set = i*8 + j;
        if(c == ' ') break;
        if(c == '/') {
            i++;
            j = 0;
            continue;
        }
        else if(c == 'r') {
            blackRook |= (1LL << set);
            allPieces[set] = COLOR_BLACK | PIECE_ROOK;
        }
        else if(c == 'n') {
            blackKnight |= (1LL << set);
            allPieces[set] = COLOR_BLACK | PIECE_KNIGHT;
        }
        else if(c == 'b') {
            blackBishop |= (1LL << set);
            allPieces[set] = COLOR_BLACK | PIECE_BISHOP;
        }
        else if(c == 'q') {
            blackQueen |= (1LL << set);
            allPieces[set] = COLOR_BLACK | PIECE_QUEEN;
        }
        else if(c == 'k') {
            blackKing |= (1LL << set);
            allPieces[set] = COLOR_BLACK | PIECE_KING;
        }
        else if(c == 'p') {
            blackPawn |= (1LL << set);
            allPieces[set] = COLOR_BLACK | PIECE_PAWN;
        }
        else if(c == 'R') {
            whiteRook |= (1LL << set);
            allPieces[set] = COLOR_WHITE | PIECE_ROOK;
        }
        else if(c == 'N') {
            whiteKnight |= (1LL << set);
            allPieces[set] = COLOR_WHITE | PIECE_KNIGHT;
        }
        else if(c == 'B') {
            whiteBishop |= (1LL << set);
            allPieces[set] = COLOR_WHITE | PIECE_BISHOP;
        }
        else if(c == 'Q') {
            whiteQueen |= (1LL << set);
            allPieces[set] = COLOR_WHITE | PIECE_QUEEN;
        }
        else if(c == 'K') {
            whiteKing |= (1LL << set);
            allPieces[set] = COLOR_WHITE | PIECE_KING;
        }
        else if(c == 'P') {
            whitePawn |= (1LL << set);
            allPieces[set] = COLOR_WHITE | PIECE_PAWN;
        }
        else if(isdigit(c)) {
            j += (c - '0') - 1;
        }
        j++;
    }
    whitePieces = whiteBishop | whiteKing | whitePawn | whiteKnight | whiteQueen | whiteRook;
    blackPieces = blackBishop | blackKing | blackPawn | blackKnight | blackQueen | blackRook;
    
    index += 2; 
    
    whiteCanCastleKingSide = whiteCanCastleQueenSide = false;
    blackCanCastleKingSide = blackCanCastleQueenSide = false;
    
    while(moveSet[index] != ' ') {
        switch(moveSet[index]) {
            case 'K': whiteCanCastleKingSide = true; break;
            case 'Q': whiteCanCastleQueenSide = true; break;
            case 'k': blackCanCastleKingSide = true; break;
            case 'q': blackCanCastleQueenSide = true; break;
            case '-': break;
        }
        index++;
    }
    index++; 
    
    if(moveSet[index] == '-') {
        enPassantSquare = -1;
        index++;
    } else {
        int file = moveSet[index] - 'a';
        int rank = '8' - moveSet[index + 1];
        enPassantSquare = rank * 8 + file;
        index += 2;
    }
    index++; 
    
    halfmoveClock = 0;
    while(moveSet[index] != ' ') {
        halfmoveClock = halfmoveClock * 10 + (moveSet[index] - '0');
        index++;
    }
    index++; 
    fullmoveNumber = 0;
    while(index < moveSet.length() && isdigit(moveSet[index])) {
        fullmoveNumber = fullmoveNumber * 10 + (moveSet[index] - '0');
        index++;
    }
}

void Board::printGame(){
    for(int i = 0; i < 8; i++){
        cout << 8-i << "  ";
        for(int j = 0; j < 8; j++){
            int mask = i*8 + j;
            string piece = ".";
            if((whiteQueen >> mask) & 1) piece = "♛";
            else if((whitePawn >> mask) & 1) piece = "♟";
            else if((whiteBishop >> mask) & 1) piece = "♝";
            else if((whiteKing >> mask) & 1) piece = "♚";
            else if((whiteRook >> mask) & 1) piece = "♜";
            else if((whiteKnight >> mask) & 1) piece = "♞";

            else if((blackQueen >> mask) & 1) piece = "♕";
            else if((blackPawn >> mask) & 1) piece = "♙";
            else if((blackBishop >> mask) & 1) piece = "♗";
            else if((blackKing >> mask) & 1) piece = "♔";
            else if((blackRook >> mask) & 1) piece = "♖";
            else if((blackKnight >> mask) & 1) piece = "♘";

            cout << piece << " ";
        }
        cout << "\n";
    }
    cout << "\n   A B C D E F G H\n";
}

void Board::outputFen(){
    string fen = "";

    // 1. Piece Placement
    for(int rank = 0; rank < 8; rank++) { // From rank 8 to 1
        int emptyCount = 0;
        for(int file = 0; file < 8; file++) { // From file a to h
            int square = (rank) * 8 + file;
            char piece = '-';

            if(whitePawn & (1ULL << square)) piece = 'P';
            else if(whiteRook & (1ULL << square)) piece = 'R';
            else if(whiteBishop & (1ULL << square)) piece = 'B';
            else if(whiteKnight & (1ULL << square)) piece = 'N';
            else if(whiteQueen & (1ULL << square)) piece = 'Q';
            else if(whiteKing & (1ULL << square)) piece = 'K';

            else if(blackPawn & (1ULL << square)) piece = 'p';
            else if(blackRook & (1ULL << square)) piece = 'r';
            else if(blackBishop & (1ULL << square)) piece = 'b';
            else if(blackKnight & (1ULL << square)) piece = 'n';
            else if(blackQueen & (1ULL << square)) piece = 'q';
            else if(blackKing & (1ULL << square)) piece = 'k';

            if(piece == '-') {
                emptyCount++;
            }
            else {
                if(emptyCount > 0) {
                    fen += to_string(emptyCount);
                    emptyCount = 0;
                }
                fen += piece;
            }
        }
        if(emptyCount > 0) {
            fen += to_string(emptyCount);
        }
        if(rank != 7) {
            fen += '/';
        }
    }

    // // 2. Active Color
    // fen += ' ';
    // fen += (turn) ? 'w' : 'b';

    // // 3. Castling Availability
    // fen += ' ';
    // string castling = "";
    // if(whiteCanCastleKingSide) castling += 'K';
    // if(whiteCanCastleQueenSide) castling += 'Q';
    // if(blackCanCastleKingSide) castling += 'k';
    // if(blackCanCastleQueenSide) castling += 'q';
    // fen += (castling.empty()) ? "-" : castling;

    // // 4. En Passant Target Square
    // fen += ' ';
    // fen += (enPassantSquare == -1) ? '-' : indexToAlgebraic(enPassantSquare)[0];

    // // 5. Halfmove Clock
    // fen += ' ';
    // fen += to_string(halfmoveClock);

    // // 6. Fullmove Number
    // fen += ' ';
    // fen += to_string(fullmoveNumber);

    // // Output the FEN string
    cout << fen << "\n";
}

void Board::generatePieceMoves(bool isWhite, Move moveArray[], int &moveCount) {

    moveCount = 0;

    uint64_t ownPieces = isWhite ? whitePieces : blackPieces;
    uint64_t enemyPieces = isWhite ? blackPieces : whitePieces;

    uint64_t knights = isWhite ? whiteKnight : blackKnight;
    while(knights) {
        int single_knight = __builtin_ctzll(knights);
        knights &= knights - 1;
        uint64_t knight_attack_bb = knight_attacks[single_knight];
        while(knight_attack_bb) {
            int target = __builtin_ctzll(knight_attack_bb);
            knight_attack_bb &= knight_attack_bb - 1;
            uint64_t targetMask = (1ULL << target);
            if(ownPieces & targetMask) continue;
            if(allPieces[target] != PIECE_NONE && ((allPieces[target] & COLOR_MASK) != (isWhite ? COLOR_WHITE : COLOR_BLACK))) {
                moveArray[moveCount++] = Move(isWhite, PIECE_KNIGHT, single_knight, target, (allPieces[target] & PIECE_MASK), '-');
            } else {
                moveArray[moveCount++] = Move(isWhite, PIECE_KNIGHT, single_knight, target, PIECE_NONE, '-');
            }
        }
    }

    uint64_t rooks = isWhite ? whiteRook : blackRook;
    while(rooks && moveCount < 256) {
        int single_rook = __builtin_ctzll(rooks);
        rooks &= rooks-1;
        for(int dir = 0; dir < 4; dir++) {
            int target = single_rook;
            while(true) {
                target += DIRECTIONS_STRAIGHT[dir];
                if(target < 0 || target > 63) break;
                if(DIRECTIONS_STRAIGHT[dir] == 1 && (target % 8 == 0)) break; // right out-of-board
                if(DIRECTIONS_STRAIGHT[dir] == -1 && ((target+1) % 8 == 0)) break; // left out-of-board
                uint64_t targetMask = 1ULL << target;
                if(ownPieces & targetMask) {
                    break;
                } 
                if(allPieces[target] != PIECE_NONE && ((allPieces[target] & COLOR_MASK) != (isWhite ? COLOR_WHITE : COLOR_BLACK))) {
                    moveArray[moveCount++] = Move(isWhite, PIECE_ROOK, single_rook, target, (allPieces[target] & PIECE_MASK), '-');
                    break;
                } else {
                    moveArray[moveCount++] = Move(isWhite, PIECE_ROOK, single_rook, target, PIECE_NONE, '-');
                }
                if(moveCount >= 256) break;
            }
        }
    }

    uint64_t bishops = isWhite ? whiteBishop : blackBishop;
    while(bishops && moveCount < 256) {
        int single_bishop = __builtin_ctzll(bishops);
        bishops &= bishops - 1; 
        for(int dir = 0; dir < 4; dir++) {
            int target = single_bishop;
            while(true) {
                target += DIRECTIONS_DIAGONAL[dir];
                if(target < 0 || target > 63) break;
                if((DIRECTIONS_DIAGONAL[dir] == 9 || DIRECTIONS_DIAGONAL[dir] == -7) && (target % 8 == 0)) break; // top-right wrapping
                if((DIRECTIONS_DIAGONAL[dir] == -9 || DIRECTIONS_DIAGONAL[dir] == 7) && ((target + 1) % 8 == 0)) break; // top-left wrapping
                uint64_t targetMask = 1ULL << target;
                if(ownPieces & targetMask) {
                    break;
                } 
                else if(allPieces[target] != PIECE_NONE && ((allPieces[target] & COLOR_MASK) != (isWhite ? COLOR_WHITE: COLOR_BLACK))) {
                    moveArray[moveCount++] = Move(isWhite, PIECE_BISHOP, single_bishop, target, (allPieces[target] & PIECE_MASK), '-');
                    break;
                } else {
                    moveArray[moveCount++] = Move(isWhite, PIECE_BISHOP, single_bishop, target, PIECE_NONE, '-');
                }
                if(moveCount >= 256) break;
            }
        }
    }

    const int DIRECTIONS_QUEEN[8] = {8, -8, 1, -1, 9, -7, -9, 7}; 

    uint64_t queens = isWhite ? whiteQueen : blackQueen;
    while(queens && moveCount < 256) {
        int queenSquare = __builtin_ctzll(queens);
        queens &= queens - 1;
        for(int dir = 0; dir < 8; dir++) {
            int target = queenSquare;
            while(true) {
                target += DIRECTIONS_QUEEN[dir];
                if(target < 0 || target > 63) break;
                if((DIRECTIONS_QUEEN[dir] == 1 && (target % 8 == 0)) || // right wrapping
                   (DIRECTIONS_QUEEN[dir] == -1 && ((target + 1) % 8 == 0)) || // left wrapping
                   (DIRECTIONS_QUEEN[dir] == 9 && (target % 8 == 0)) ||  // NE wrapping
                   (DIRECTIONS_QUEEN[dir] == -7 && (target % 8 == 0)) || // NE wrapping
                   (DIRECTIONS_QUEEN[dir] == -9 && ((target + 1) % 8 == 0)) || // NW wrapping
                   (DIRECTIONS_QUEEN[dir] == 7 && ((target + 1) % 8 == 0))) { // SW wrapping
                    break;
                }
                uint64_t targetMask = 1ULL << target;
                if(ownPieces & targetMask) {
                    break;
                } 
                else if(allPieces[target] != PIECE_NONE && ((allPieces[target] & COLOR_MASK) != (isWhite ? COLOR_WHITE : COLOR_BLACK))) {
                    moveArray[moveCount++] = Move(isWhite, PIECE_QUEEN, queenSquare, target, (allPieces[target] & PIECE_MASK), '-');
                    break;
                } else {
                    moveArray[moveCount++] = Move(isWhite, PIECE_QUEEN, queenSquare, target, PIECE_NONE, '-');
                }
                if(moveCount >= 256) break;
            }
        }
    }
    // king 
    const int DIRECTIONS_KING[8] = {8, 9, 1, -7, -8, -9, -1, 7}; // N, NE, E, SE, S, SW, W, NW
    uint64_t kings = isWhite ? whiteKing : blackKing;
    while(kings && moveCount < 256) {
        int kingSquare = __builtin_ctzll(kings);
        kings &= kings-1;
        for(int dir = 0; dir < 8; dir++) {
            int target = kingSquare + DIRECTIONS_KING[dir];
            if(target < 0 || target > 63) continue;
            if((DIRECTIONS_KING[dir] == 1 || DIRECTIONS_KING[dir] == -7 || DIRECTIONS_KING[dir] == 9)&& (kingSquare % 8 == 7)) continue;   // East wrapping
            if((DIRECTIONS_KING[dir] == -1 || DIRECTIONS_KING[dir] == -9 || DIRECTIONS_KING[dir] == +7) && (kingSquare % 8 == 0)) continue; // West wrapping
            if((DIRECTIONS_KING[dir] == -9 || DIRECTIONS_KING[dir] == -7 || DIRECTIONS_KING[dir] == -8) && (kingSquare < 8)) continue; // North wrapping
            if((DIRECTIONS_KING[dir] == 9 || DIRECTIONS_KING[dir] == 8 || DIRECTIONS_KING[dir] == 7) && (kingSquare > 55)) continue; // South wrapping

            uint64_t targetMask = 1ULL << target;
            if(ownPieces & targetMask) {
                continue;
            } 
            else if(allPieces[target] != PIECE_NONE && ((allPieces[target] & COLOR_MASK) != (isWhite ? COLOR_WHITE : COLOR_BLACK))) {
                moveArray[moveCount++] = Move(isWhite, PIECE_KING, kingSquare, target, (allPieces[target] & PIECE_MASK), '-');
            } else {
                moveArray[moveCount++] = Move(isWhite, PIECE_KING, kingSquare, target, PIECE_NONE, '-');
            }

            if(moveCount >= 256) break;
        }
    }

    // pawn move gen

    uint64_t pawns = isWhite ? whitePawn : blackPawn;
    while(pawns && moveCount < 256) {
        int single_pawn = __builtin_ctzll(pawns);
        pawns &= pawns -1;
        if(isWhite) {
            // Single push
            int target = single_pawn - 8;
            if(target >= 0 && allPieces[target] == PIECE_NONE) {
                if(target < 8) { // Promotion
                    moveArray[moveCount++] = Move(isWhite, PIECE_PAWN, single_pawn, target, PIECE_NONE, 'Q');
                    // Add other promotion pieces
                } else {
                    moveArray[moveCount++] = Move(isWhite, PIECE_PAWN, single_pawn, target, PIECE_NONE, '-');
                }
            }
            
            if(((1ULL << single_pawn) & RANK_2) && target >= 24) {
                target = single_pawn - 16;
                if(allPieces[target] == PIECE_NONE && allPieces[single_pawn - 8] == PIECE_NONE) {
                    moveArray[moveCount++] = Move(isWhite, PIECE_PAWN, single_pawn, target, PIECE_NONE, '-');
                }
            }
            // capture left
            if((1ULL << single_pawn) & ~A_FILE) {
                target = single_pawn - 9;
                if((allPieces[target] & COLOR_MASK) != (isWhite ? COLOR_WHITE : COLOR_BLACK) && (allPieces[target] & PIECE_MASK) != PIECE_NONE){
                    // piece captured
                    moveArray[moveCount++] = Move(isWhite, PIECE_PAWN, single_pawn, target, (allPieces[target] & PIECE_MASK), '-');
                }
            }
            // capture right
            if((1ULL << single_pawn) & ~H_FILE) {
                target = single_pawn - 7;
                if((allPieces[target] & COLOR_MASK) != (isWhite ? COLOR_WHITE : COLOR_BLACK) && (allPieces[target] & PIECE_MASK) != PIECE_NONE){
                    // piece captured
                    moveArray[moveCount++] = Move(isWhite, PIECE_PAWN, single_pawn, target, (allPieces[target] & PIECE_MASK), '-');
                }
            }

            // En passant
            if(enPassantSquare != -1) {
                if((single_pawn - 7 == enPassantSquare && (1ULL << single_pawn) & ~H_FILE) ||
                (single_pawn - 9 == enPassantSquare && (1ULL << single_pawn) & ~A_FILE)) {
                    moveArray[moveCount++] = Move(isWhite, PIECE_PAWN, single_pawn, enPassantSquare, PIECE_PAWN, 'e');
                }
            }
        } else {
            // single push
            int target = single_pawn + 8;
            if(target >= 0 && allPieces[target] == PIECE_NONE) {
                moveArray[moveCount++] = Move(isWhite, PIECE_PAWN, single_pawn, target, PIECE_NONE, '-');
            }
            // double push
            if(((1ULL << single_pawn) & RANK_7) && target <= 39) {
                target = single_pawn + 16;
                if(allPieces[target] == PIECE_NONE && allPieces[single_pawn + 8] == PIECE_NONE) {
                    moveArray[moveCount++] = Move(isWhite, PIECE_PAWN, single_pawn, target, PIECE_NONE, '-');
                }
            }
 

            // capture left
            if((1ULL << single_pawn) & ~A_FILE) {
                target = single_pawn + 7;
                if((allPieces[target] & COLOR_MASK) != (isWhite ? COLOR_WHITE : COLOR_BLACK) && (allPieces[target] & PIECE_MASK) != PIECE_NONE){
                    // piece captured
                    moveArray[moveCount++] = Move(isWhite, PIECE_PAWN, single_pawn, target, (allPieces[target] & PIECE_MASK), '-');
                }
            }
            // capture right
            if((1ULL << single_pawn) & ~H_FILE) {
                target = single_pawn + 9;
                if((allPieces[target] & COLOR_MASK) != (isWhite ? COLOR_WHITE : COLOR_BLACK) && (allPieces[target] & PIECE_MASK) != PIECE_NONE){
                    // piece captured
                    moveArray[moveCount++] = Move(isWhite, PIECE_PAWN, single_pawn, target, (allPieces[target] & PIECE_MASK), '-');
                }
            }
        }
    }
    int legalMoveCount = 0;
    for (int i = 0; i < moveCount; i++)
    {
        if(isMoveLegal(moveArray[i])) {
            if(legalMoveCount != i) {
                moveArray[legalMoveCount] = moveArray[i];
            }
            legalMoveCount++;
        }
    }
    moveCount = legalMoveCount;
    
}

long Board::perft2(int depth, bool isWhite) {
    if (depth == 0)
        return 1;

    long nodes = 0;
    Move moves[256];
    int moveCount = 0;

    generatePieceMoves(isWhite, moves, moveCount);

    for (int i = 0; i < moveCount; i++) {
        make_move(moves[i]);
        nodes += perft2(depth - 1, !isWhite);
        unmake_move(moves[i]);
    }

    return nodes;
}

void Board::printMoves(Move moveArray[], int moveCount) {
    for(int i = 0; i < moveCount; i++) {
        cout << (moveArray[i].isWhite ? "White " : "Black ") 
                << "Move: " << indexToAlgebraic(moveArray[i].startSquare) << indexToAlgebraic(moveArray[i].endSquare) 
                << "\n";
    }
}

void Board::make_move(Move &mv) {
    bool side = mv.isWhite;

    // Remove captured piece if any
    if(mv.pieceCaptured != PIECE_NONE) {
        uint64_t* capBoard = nullptr;
        switch(mv.pieceCaptured) {
            case PIECE_PAWN:   capBoard = side ? &blackPawn   : &whitePawn;   break;
            case PIECE_ROOK:   capBoard = side ? &blackRook   : &whiteRook;   break;
            case PIECE_BISHOP: capBoard = side ? &blackBishop : &whiteBishop; break;
            case PIECE_KNIGHT: capBoard = side ? &blackKnight : &whiteKnight; break;
            case PIECE_QUEEN:  capBoard = side ? &blackQueen  : &whiteQueen;  break;
            case PIECE_KING:   capBoard = side ? &blackKing   : &whiteKing;   break;
        }
        if(capBoard) {
            *capBoard &= ~(1ULL << mv.endSquare);
            // Update combined pieces
            if(side) {
                blackPieces &= ~(1ULL << mv.endSquare);
            } else {
                whitePieces &= ~(1ULL << mv.endSquare);
            }
        }
    }

    // Move the piece
    uint64_t* movBoard = nullptr;
    switch(mv.pieceType) {
        case PIECE_PAWN:   movBoard = side ? &whitePawn   : &blackPawn;   break;
        case PIECE_ROOK:   movBoard = side ? &whiteRook   : &blackRook;   break;
        case PIECE_BISHOP: movBoard = side ? &whiteBishop : &blackBishop; break;
        case PIECE_KNIGHT: movBoard = side ? &whiteKnight : &blackKnight; break;
        case PIECE_QUEEN:  movBoard = side ? &whiteQueen  : &blackQueen;  break;
        case PIECE_KING:   movBoard = side ? &whiteKing   : &blackKing;   break;
    }
    if(movBoard) {
        *movBoard &= ~(1ULL << mv.startSquare);
        *movBoard |= (1ULL << mv.endSquare);
        // Update combined pieces
        if(side) {
            whitePieces &= ~(1ULL << mv.startSquare);
            whitePieces |= (1ULL << mv.endSquare);
        } else {
            blackPieces &= ~(1ULL << mv.startSquare);
            blackPieces |= (1ULL << mv.endSquare);
        }
    }

    allPieces[mv.startSquare] = PIECE_NONE;
    allPieces[mv.endSquare] = side ? (COLOR_WHITE | mv.pieceType) : (COLOR_BLACK | mv.pieceType);
}

void Board::unmake_move(Move &mv) {
    bool side = mv.isWhite;

    // Move piece back
    uint64_t* movBoard = nullptr;
    switch(mv.pieceType) {
        case PIECE_PAWN:   movBoard = side ? &whitePawn   : &blackPawn;   break;
        case PIECE_ROOK:   movBoard = side ? &whiteRook   : &blackRook;   break;
        case PIECE_BISHOP: movBoard = side ? &whiteBishop : &blackBishop; break;
        case PIECE_KNIGHT: movBoard = side ? &whiteKnight : &blackKnight; break;
        case PIECE_QUEEN:  movBoard = side ? &whiteQueen  : &blackQueen;  break;
        case PIECE_KING:   movBoard = side ? &whiteKing   : &blackKing;   break;
    }
    if(movBoard) {
        *movBoard &= ~(1ULL << mv.endSquare);
        *movBoard |= (1ULL << mv.startSquare);
        // Update combined pieces
        if(side) {
            whitePieces &= ~(1ULL << mv.endSquare);
            whitePieces |= (1ULL << mv.startSquare);
        } else {
            blackPieces &= ~(1ULL << mv.endSquare);
            blackPieces |= (1ULL << mv.startSquare);
        }
    }

    // Restore captured piece
    if(mv.pieceCaptured != PIECE_NONE) {
        uint64_t* capBoard = nullptr;
        switch(mv.pieceCaptured) {
            case PIECE_PAWN:   capBoard = side ? &blackPawn   : &whitePawn;   break;
            case PIECE_ROOK:   capBoard = side ? &blackRook   : &whiteRook;   break;
            case PIECE_BISHOP: capBoard = side ? &blackBishop : &whiteBishop; break;
            case PIECE_KNIGHT: capBoard = side ? &blackKnight : &whiteKnight; break;
            case PIECE_QUEEN:  capBoard = side ? &blackQueen  : &whiteQueen;  break;
            case PIECE_KING:   capBoard = side ? &blackKing   : &whiteKing;   break;
        }
        if(capBoard) {
            *capBoard |= (1ULL << mv.endSquare);
            // Update combined pieces
            if(side) {
                blackPieces |= (1ULL << mv.endSquare);
            } else {
                whitePieces |= (1ULL << mv.endSquare);
            }
        }
    }

    allPieces[mv.startSquare] = side ? (COLOR_WHITE | mv.pieceType) : (COLOR_BLACK | mv.pieceType);
    allPieces[mv.endSquare] = mv.pieceCaptured != PIECE_NONE ? 
        (side ? (COLOR_BLACK | mv.pieceCaptured) : (COLOR_WHITE | mv.pieceCaptured)) : PIECE_NONE;
}

long Board::perft(int depth, bool isWhite, int initialDepth = -1) {
    if (initialDepth == -1) {
        initialDepth = depth;
    }

    if (depth == 0) {
        return 1;
    }

    Move moveArray[256];
    int moveCount = 0;
    generatePieceMoves(isWhite, moveArray, moveCount);

    long nodes = 0;

    // Only print moves at root level
    if (depth == initialDepth) {
        for (int i = 0; i < moveCount; i++) {
            make_move(moveArray[i]);
            long childNodes = perft(depth - 1, !isWhite, initialDepth);
            string moveStr = indexToAlgebraic(moveArray[i].startSquare) + 
                           indexToAlgebraic(moveArray[i].endSquare);
            cout << moveStr << ": " << childNodes << "\n";
            nodes += childNodes;
            unmake_move(moveArray[i]);
        }
    } else {
        // Faster path for non-root nodes
        for (int i = 0; i < moveCount; i++) {
            make_move(moveArray[i]);
            nodes += perft(depth - 1, !isWhite, initialDepth);
            unmake_move(moveArray[i]);
        }
    }

    return nodes;
}

int Board::evaluateBoard() {
    const int PAWN_VALUE = 100;
    const int BISHOP_VALUE = 300;
    const int KNIGHT_VALUE = 300;
    const int ROOK_VALUE = 500;
    const int QUEEN_VALUE = 900;

    int score = 0;

    score += __builtin_popcount(whitePawn) * PAWN_VALUE;
    score += __builtin_popcount(whiteBishop) * BISHOP_VALUE;
    score += __builtin_popcount(whiteKnight) * KNIGHT_VALUE;
    score += __builtin_popcount(whiteRook) * ROOK_VALUE;
    score += __builtin_popcount(whiteQueen) * QUEEN_VALUE;

    score -= __builtin_popcount(blackPawn) * PAWN_VALUE;
    score -= __builtin_popcount(blackBishop) * BISHOP_VALUE;
    score -= __builtin_popcount(blackKnight) * KNIGHT_VALUE;
    score -= __builtin_popcount(blackRook) * ROOK_VALUE;
    score -= __builtin_popcount(blackQueen) * QUEEN_VALUE;


    // new stuff added

    if(isInCheck(true)) score -= 50;
    if(isInCheck(false)) score += 50;

    Move moves[256];
    int count = 0;

    generatePieceMoves(true, moves, count);
    if (count == 0 && isInCheck(true)) score = -99999;  // White is checkmated
    
    count = 0;
    generatePieceMoves(false, moves, count);
    if (count == 0 && isInCheck(false)) score = 99999;  // Black is checkmated

    return score;
}

void Board::order_moves(Move *moves, int moveCount) {
    // if move != capture -> send to last 
    for(int i = 0; i < moveCount; i++){
        for(int j = i; j < moveCount - i - 1; j++) {
            if(moves[i].pieceCaptured == -1){
                swap(moves[i], moves[j]);
            }
        }
    }
}

void print(Move *moves, int moveCount){
    for (int i = 0; i < moveCount; i++)
    {
        cout << moves[i].pieceCaptured << " " << moves[i].endSquare << " ";
    }
    cout << "\n";
}

int Board::alphaBeta(bool isWhite, int depth, int alpha, int beta) {

    if (depth == 0) {
        moveSearched++;
        return evaluateBoard();
    }
    
    int localBest = isWhite ? -100000 : 100000;
    Move bestLocalMove;
    
    Move moves[256];
    int moveCount = 0;
    
    generatePieceMoves(isWhite, moves, moveCount);

    for (int i = 0; i < moveCount; i++) {
        make_move(moves[i]);
        int score = alphaBeta(!isWhite, depth - 1, alpha, beta);
        unmake_move(moves[i]);
        
        if (isWhite) {
            if (score > localBest) {
                localBest = score;
                bestLocalMove = moves[i];
            }
            alpha = max(alpha, localBest);
        } else {
            if (score < localBest) {
                localBest = score;
                bestLocalMove = moves[i];
            }
            beta = min(beta, localBest);
        }
        
        if (beta <= alpha) {
            break; // Alpha-beta pruning
        }
    }
    
    // Store best move only at root
    if (depth == initialDepth) {
        bestMove = bestLocalMove;
    }
    
    return localBest;
}

// new stuff added, remove if the engine breaks


bool Board::isSquareAttacked(int square, bool byWhite) {
    // Pawn attacks
    uint64_t pawns = byWhite ? whitePawn : blackPawn;
    int pawnDirection = byWhite ? -8 : 8;
    if ((square + pawnDirection + 1) < 64 && (square + pawnDirection + 1) >= 0) {
        if (pawns & (1ULL << (square + pawnDirection + 1))) return true;
    }
    if ((square + pawnDirection - 1) < 64 && (square + pawnDirection - 1) >= 0) {
        if (pawns & (1ULL << (square + pawnDirection - 1))) return true;
    }

    // Knight attacks
    uint64_t knights = byWhite ? whiteKnight : blackKnight;
    uint64_t knightAttacks = knight_attacks[square];
    if (knights & knightAttacks) return true;

    // Bishop/Queen diagonal attacks
    uint64_t bishopsQueens = byWhite ? 
        (whiteBishop | whiteQueen) : (blackBishop | blackQueen);
    for (int dir = 0; dir < 4; dir++) {
        int target = square;
        while (true) {
            target += DIRECTIONS_DIAGONAL[dir];
            if (target < 0 || target > 63) break;
            if ((DIRECTIONS_DIAGONAL[dir] == 9 || DIRECTIONS_DIAGONAL[dir] == -7) 
                && (target % 8 == 0)) break;
            if ((DIRECTIONS_DIAGONAL[dir] == -9 || DIRECTIONS_DIAGONAL[dir] == 7) 
                && ((target + 1) % 8 == 0)) break;
            uint64_t targetBit = 1ULL << target;
            if (bishopsQueens & targetBit) return true;
            if (allPieces[target] != PIECE_NONE) break;
        }
    }

    // Rook/Queen straight attacks
    uint64_t rooksQueens = byWhite ? 
        (whiteRook | whiteQueen) : (blackRook | blackQueen);
    for (int dir = 0; dir < 4; dir++) {
        int target = square;
        while (true) {
            target += DIRECTIONS_STRAIGHT[dir];
            if (target < 0 || target > 63) break;
            if (DIRECTIONS_STRAIGHT[dir] == 1 && (target % 8 == 0)) break;
            if (DIRECTIONS_STRAIGHT[dir] == -1 && ((target + 1) % 8 == 0)) break;
            uint64_t targetBit = 1ULL << target;
            if (rooksQueens & targetBit) return true;
            if (allPieces[target] != PIECE_NONE) break;
        }
    }

    return false;
}

bool Board::isInCheck(bool isWhite) {
    uint64_t king = isWhite ? whiteKing : blackKing;
    int kingSquare = __builtin_ctzll(king);
    return isSquareAttacked(kingSquare, !isWhite);
}

bool Board::isMoveLegal(Move &mv) {
    make_move(mv);
    bool legal = !isInCheck(mv.isWhite);
    unmake_move(mv);
    return legal;
}





int main() {
    Board b;
    b.setupGameFromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    // bool isWhite = true;
    // for (int i = 0; i < 30; i++)
    // {
    //     b.alphaBeta(isWhite, initialDepth, -10000, 10000);
    //     b.make_move(b.bestMove);
    //     b.printGame();
    //     isWhite = !isWhite;
    // }
    


    int maxDepth = 7;
    for(int depth = 1; depth <= maxDepth; depth++){
        long totalNodes = b.perft2(depth, true);
        cout << totalNodes << "\n";
    }

    return 0;
}
