#include <iostream> 
#include <vector>
using namespace std;

const uint64_t A_FILE = 0x0101010101010101;
const uint64_t B_FILE = 0x0202020202020202;
const uint64_t C_FILE = 0x0404040404040404;
const uint64_t D_FILE = 0x0808080808080808;
const uint64_t E_FILE = 0x1010101010101010;
const uint64_t F_FILE = 0x2020202020202020;
const uint64_t G_FILE = 0x4040404040404040;
const uint64_t H_FILE = 0x8080808080808080;

const uint64_t RANK_1 = 0x00000000000000FF;
const uint64_t RANK_2 = 0x000000000000FF00;
const uint64_t RANK_3 = 0x0000000000FF0000;
const uint64_t RANK_4 = 0x00000000FF000000;
const uint64_t RANK_5 = 0x000000FF00000000;
const uint64_t RANK_6 = 0x0000FF0000000000;
const uint64_t RANK_7 = 0x00FF000000000000;
const uint64_t RANK_8 = 0xFF00000000000000;

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

int initialDepth = 3;

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

    bool turn = false; // true = white, false = black;

    bool whiteCanCastleKingSide;
    bool whiteCanCastleQueenSide;
    bool blackCanCastleKingSide;
    bool blackCanCastleQueenSide;
    int enPassantSquare; 
    int halfmoveClock;
    int fullmoveNumber;

    string indexToAlgebraic(int index) {
        if(index < 0 || index > 63) return "??";
        char file = 'a' + (index % 8);
        char rank = '1' + 7-(index / 8);
        return string(1, file) + string(1, rank);
    }

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


public:
    Board() {}

    void printGame(){
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

    void setupGameFromFEN(string moveSet) {
        // Reset all bitboards
        whitePawn = whiteRook = whiteBishop = whiteKnight = whiteQueen = whiteKing = 0;
        blackPawn = blackRook = blackBishop = blackKnight = blackQueen = blackKing = 0;
        
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
            else if(c == 'r') blackRook |= (1LL << set);
            else if(c == 'n') blackKnight |= (1LL << set);
            else if(c == 'b') blackBishop |= (1LL << set);
            else if(c == 'q') blackQueen |= (1LL << set);
            else if(c == 'k') blackKing |= (1LL << set);
            else if(c == 'p') blackPawn |= (1LL << set);
            else if(c == 'R') whiteRook |= (1LL << set);
            else if(c == 'N') whiteKnight |= (1LL << set);
            else if(c == 'B') whiteBishop |= (1LL << set);
            else if(c == 'Q') whiteQueen |= (1LL << set);
            else if(c == 'K') whiteKing |= (1LL << set);
            else if(c == 'P') whitePawn |= (1LL << set);
            else if(isdigit(c)) {
                j += (c - '0') - 1;
            }
            j++;
        }
        whitePieces = whiteBishop | whiteKing | whitePawn | whiteKnight | whiteQueen | whiteRook;
        blackPieces = blackBishop | blackKing | blackPawn | blackKnight | blackQueen | blackRook;
        
        turn = (moveSet[index] == 'w');
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

    void outputFen() {
        std::string fen = "";

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
                        fen += std::to_string(emptyCount);
                        emptyCount = 0;
                    }
                    fen += piece;
                }
            }
            if(emptyCount > 0) {
                fen += std::to_string(emptyCount);
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
        // std::string castling = "";
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
        // fen += std::to_string(halfmoveClock);

        // // 6. Fullmove Number
        // fen += ' ';
        // fen += std::to_string(fullmoveNumber);

        // // Output the FEN string
        std::cout << fen << "\n";
    }

    void generatePieceMoves(bool isWhite, Move moveArray[], int &moveCount);

    void printMoves(Move moveArray[], int moveCount);

    long perft(int depth, bool isWhite);
    
    void make_move(Move& mv);

    void unmake_move(Move & mv);
};

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
            else if(enemyPieces & targetMask) {
                moveArray[moveCount++] = Move(isWhite, PIECE_KNIGHT, single_knight, target, 1, '-');
            } else {
                moveArray[moveCount++] = Move(isWhite, PIECE_KNIGHT, single_knight, target, -1, '-');
            }
        }
    }
    const int DIRECTIONS_STRAIGHT[4] = {8, -8, 1, -1}; // down, up, right, left
    const int DIRECTIONS_DIAGONAL[4] = {9, -9, 7, -7}; // bottom-right, top-left, bottom-left, top-right

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
                } else if(enemyPieces & targetMask) {
                    moveArray[moveCount++] = Move(isWhite, PIECE_ROOK, single_rook, target, 1, '-');
                    break;
                } else {
                    moveArray[moveCount++] = Move(isWhite, PIECE_ROOK, single_rook, target, -1, '-');
                }
                if(moveCount >= 256) break;
            }
        }
    }

    uint64_t bishops = isWhite ? whiteBishop : blackBishop;
    while(bishops && moveCount < 256) {
        int bishopSquare = __builtin_ctzll(bishops);
        bishops &= bishops - 1; 
        for(int dir = 0; dir < 4; dir++) {
            int targetSquare = bishopSquare;
            while(true) {
                targetSquare += DIRECTIONS_DIAGONAL[dir];
                if(targetSquare < 0 || targetSquare > 63) break;
                if((DIRECTIONS_DIAGONAL[dir] == 9 || DIRECTIONS_DIAGONAL[dir] == -7) && (targetSquare % 8 == 0)) break; // top-right wrapping
                if((DIRECTIONS_DIAGONAL[dir] == -9 || DIRECTIONS_DIAGONAL[dir] == 7) && ((targetSquare + 1) % 8 == 0)) break; // top-left wrapping
                uint64_t targetMask = 1ULL << targetSquare;
                if(ownPieces & targetMask) {
                    break;
                } else if(enemyPieces & targetMask) {
                    moveArray[moveCount++] = Move(isWhite, PIECE_BISHOP, bishopSquare, targetSquare, 1, '-');
                    break; 
                } else {
                    moveArray[moveCount++] = Move(isWhite, PIECE_BISHOP, bishopSquare, targetSquare, -1, '-');
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
            int targetSquare = queenSquare;
            while(true) {
                targetSquare += DIRECTIONS_QUEEN[dir];
                if(targetSquare < 0 || targetSquare > 63) break;
                if((DIRECTIONS_QUEEN[dir] == 1 && (targetSquare % 8 == 0)) || // right wrapping
                   (DIRECTIONS_QUEEN[dir] == -1 && ((targetSquare + 1) % 8 == 0)) || // left wrapping
                   (DIRECTIONS_QUEEN[dir] == 9 && (targetSquare % 8 == 0)) ||  // NE wrapping
                   (DIRECTIONS_QUEEN[dir] == -7 && (targetSquare % 8 == 0)) || // NE wrapping
                   (DIRECTIONS_QUEEN[dir] == -9 && ((targetSquare + 1) % 8 == 0)) || // NW wrapping
                   (DIRECTIONS_QUEEN[dir] == 7 && ((targetSquare + 1) % 8 == 0))) { // SW wrapping
                    break;
                }
                uint64_t targetMask = 1ULL << targetSquare;
                if(ownPieces & targetMask) {
                    break;
                } else if(enemyPieces & targetMask) {
                    moveArray[moveCount++] = Move(isWhite, PIECE_QUEEN, queenSquare, targetSquare, 1, '-');
                    break; 
                } else {
                    moveArray[moveCount++] = Move(isWhite, PIECE_QUEEN, queenSquare, targetSquare, -1, '-');
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
            int targetSquare = kingSquare + DIRECTIONS_KING[dir];
            if(targetSquare < 0 || targetSquare > 63) continue;
            if((DIRECTIONS_KING[dir] == 1 || DIRECTIONS_KING[dir] == -7 || DIRECTIONS_KING[dir] == 9)&& (kingSquare % 8 == 7)) continue;   // East wrapping
            if((DIRECTIONS_KING[dir] == -1 || DIRECTIONS_KING[dir] == -9 || DIRECTIONS_KING[dir] == +7) && (kingSquare % 8 == 0)) continue; // West wrapping
            if((DIRECTIONS_KING[dir] == -9 || DIRECTIONS_KING[dir] == -7 || DIRECTIONS_KING[dir] == -8) && (kingSquare < 8)) continue; // North wrapping
            if((DIRECTIONS_KING[dir] == 9 || DIRECTIONS_KING[dir] == 8 || DIRECTIONS_KING[dir] == 7) && (kingSquare > 55)) continue; // South wrapping

            uint64_t targetMask = 1ULL << targetSquare;
            if(ownPieces & targetMask) {
                continue;
            } else if(enemyPieces & targetMask) {
                moveArray[moveCount++] = Move(isWhite, PIECE_KING, kingSquare, targetSquare, 1, '-');
            } else {
                moveArray[moveCount++] = Move(isWhite, PIECE_KING, kingSquare, targetSquare, -1, '-');
            }

            if(moveCount >= 256) break;
        }
    }

    // pawn move gen

    uint64_t pawns = isWhite ? whitePawn : blackPawn;
    while(pawns && moveCount < 256) {
        int pawnSquare = __builtin_ctzll(pawns);
        pawns &= pawns - 1;

        // Single forward move
        int forward = isWhite ? pawnSquare - 8 : pawnSquare + 8;
        if(forward >= 0 && forward < 64 && !((whitePieces | blackPieces) & (1ULL << forward))) {
            // Promotion
            if((isWhite && forward <= 7) || (!isWhite && forward >= 65)) {
                moveArray[moveCount++] = Move(isWhite, PIECE_PAWN, pawnSquare, forward, -1, 'Q');
            }
            else {
                moveArray[moveCount++] = Move(isWhite, PIECE_PAWN, pawnSquare, forward, -1, '-');
            }

            // Double forward 
            if((isWhite && pawnSquare >= 48 && pawnSquare < 56) || (!isWhite && pawnSquare >= 8 && pawnSquare < 16)) {
                int doubleForward = isWhite ? pawnSquare - 16 : pawnSquare + 16;
                if(!((whitePieces | blackPieces) & (1ULL << doubleForward))) {
                    moveArray[moveCount++] = Move(isWhite, PIECE_PAWN, pawnSquare, doubleForward, -1, '-');
                }
            }
        }

        // Capture left
        int captureLeft = isWhite ? pawnSquare - 9 : pawnSquare + 7;
        if((isWhite && (pawnSquare % 8 != 0)) || (!isWhite && (pawnSquare % 8 != 7))) {
            if(captureLeft >= 0 && captureLeft < 64 && (isWhite ? blackPieces : whitePieces) & (1ULL << captureLeft)) {
                // Promotion
                if((isWhite && captureLeft >= 56) || (!isWhite && captureLeft < 8)) {
                    moveArray[moveCount++] = Move(isWhite, PIECE_PAWN, pawnSquare, captureLeft, -1, 'Q');
                }
                else {
                    moveArray[moveCount++] = Move(isWhite, PIECE_PAWN, pawnSquare, captureLeft, -1, '-');
                }
            }
        }

        // Capture right
        int captureRight = isWhite ? pawnSquare - 7 : pawnSquare + 9;
        if((isWhite && (pawnSquare % 8 != 7)) || (!isWhite && (pawnSquare % 8 != 0))) {
            if(captureRight >= 0 && captureRight < 64 && (isWhite ? blackPieces : whitePieces) & (1ULL << captureRight)) {
                // Promotion
                if((isWhite && captureRight >= 56) || (!isWhite && captureRight < 8)) {
                    moveArray[moveCount++] = Move(isWhite, PIECE_PAWN, pawnSquare, captureRight, -1, 'Q');
                }
                else {
                    moveArray[moveCount++] = Move(isWhite, PIECE_PAWN, pawnSquare, captureRight, -1, '-');
                }
            }
        }

        if(moveCount >= 256) break;
    }
}

void Board::printMoves(Move moveArray[], int moveCount) {
    for(int i = 0; i < moveCount; i++) {
        cout << (moveArray[i].isWhite ? "White " : "Black ") 
                << "Move: " << indexToAlgebraic(moveArray[i].startSquare) << indexToAlgebraic(moveArray[i].endSquare) 
                << "\n";
    }
}

long Board::perft(int depth, bool isWhite) {
    if (depth == 0) {
        // printGame();
        return 1;
    }

    Move moveArray[256];
    int moveCount = 0;
    generatePieceMoves(isWhite, moveArray, moveCount);

    long nodes = 0;
    for (int i = 0; i < moveCount; i++) {
        make_move(moveArray[i]);
        nodes += perft(depth - 1, !isWhite);
        unmake_move(moveArray[i]);
    }

    return nodes;
}

void Board::make_move(Move &mv) {
    // check which piece to move
    bool isWhite = mv.isWhite;
    uint64_t *currentPiece;
    switch(mv.pieceType) {
        case PIECE_PAWN:
            currentPiece = isWhite ? &whitePawn : &blackPawn;
            break;
        case PIECE_ROOK:
            currentPiece = isWhite ? &whiteRook : &blackRook;
            break;
        case PIECE_BISHOP:
            currentPiece = isWhite ? &whiteBishop : &blackBishop;
            break;
        case PIECE_KNIGHT:
            currentPiece = isWhite ? &whiteKnight : &blackKnight;
            break;
        case PIECE_QUEEN:
            currentPiece = isWhite ? &whiteQueen : &blackQueen;
            break;
        case PIECE_KING:
            currentPiece = isWhite ? &whiteKing : &blackKing;
            break;
    }
    // move it
    *currentPiece &= ~(1ULL << mv.startSquare);
    *currentPiece |= (1ULL << mv.endSquare);


    int capturedPiece = -1;
    if (isWhite) {
        if (blackPawn & (1ULL << mv.endSquare)) {
            capturedPiece = PIECE_PAWN;
            blackPawn &= ~(1ULL << mv.endSquare);
        } else if (blackRook & (1ULL << mv.endSquare)) {
            capturedPiece = PIECE_ROOK;
            blackRook &= ~(1ULL << mv.endSquare);
        } else if (blackBishop & (1ULL << mv.endSquare)) {
            capturedPiece = PIECE_BISHOP;
            blackBishop &= ~(1ULL << mv.endSquare);
        } else if (blackKnight & (1ULL << mv.endSquare)) {
            capturedPiece = PIECE_KNIGHT;
            blackKnight &= ~(1ULL << mv.endSquare);
        } else if (blackQueen & (1ULL << mv.endSquare)) {
            capturedPiece = PIECE_QUEEN;
            blackQueen &= ~(1ULL << mv.endSquare);
        } else if (blackKing & (1ULL << mv.endSquare)) {
            capturedPiece = PIECE_KING;
            blackKing &= ~(1ULL << mv.endSquare);
        }
    } else {
        if (whitePawn & (1ULL << mv.endSquare)) {
            capturedPiece = PIECE_PAWN;
            whitePawn &= ~(1ULL << mv.endSquare);
        } else if (whiteRook & (1ULL << mv.endSquare)) {
            capturedPiece = PIECE_ROOK;
            whiteRook &= ~(1ULL << mv.endSquare);
        } else if (whiteBishop & (1ULL << mv.endSquare)) {
            capturedPiece = PIECE_BISHOP;
            whiteBishop &= ~(1ULL << mv.endSquare);
        } else if (whiteKnight & (1ULL << mv.endSquare)) {
            capturedPiece = PIECE_KNIGHT;
            whiteKnight &= ~(1ULL << mv.endSquare);
        } else if (whiteQueen & (1ULL << mv.endSquare)) {
            capturedPiece = PIECE_QUEEN;
            whiteQueen &= ~(1ULL << mv.endSquare);
        } else if (whiteKing & (1ULL << mv.endSquare)) {
            capturedPiece = PIECE_KING;
            whiteKing &= ~(1ULL << mv.endSquare);
        }
    }
    if(capturedPiece != -1) mv.pieceCaptured = capturedPiece;
    // pawn promotions
    if(mv.promotion != '-') {
        if(mv.pieceType == PIECE_PAWN) {
            *currentPiece &= ~(1ULL << mv.endSquare);
            switch(mv.promotion) {
                case 'Q':
                    if(isWhite) {
                        whiteQueen |= (1ULL << mv.endSquare);
                    } else {
                        blackQueen |= (1ULL << mv.endSquare);
                    }
                    break;
                case 'R':
                    if(isWhite) {
                        whiteRook |= (1ULL << mv.endSquare);
                    } else {
                        blackRook |= (1ULL << mv.endSquare);
                    }
                    break;
                case 'B':
                    if(isWhite) {
                        whiteBishop |= (1ULL << mv.endSquare);
                    } else {
                        blackBishop |= (1ULL << mv.endSquare);
                    }
                    break;
                case 'N':
                    if(isWhite) {
                        whiteKnight |= (1ULL << mv.endSquare);
                    } else {
                        blackKnight |= (1ULL << mv.endSquare);
                    }
                    break;
            }
        }
    }

    whitePieces = whitePawn | whiteKnight | whiteBishop | whiteRook | whiteQueen | whiteKing;
    blackPieces = blackPawn | blackKnight | blackBishop | blackRook | blackQueen | blackKing;

    turn = !isWhite;
}

void Board::unmake_move(Move &mv) {
    bool isWhite = mv.isWhite;
    uint64_t *currentPiece;
    switch(mv.pieceType) {
        case PIECE_PAWN:
            currentPiece = isWhite ? &whitePawn : &blackPawn;
            break;
        case PIECE_ROOK:
            currentPiece = isWhite ? &whiteRook : &blackRook;
            break;
        case PIECE_BISHOP:
            currentPiece = isWhite ? &whiteBishop : &blackBishop;
            break;
        case PIECE_KNIGHT:
            currentPiece = isWhite ? &whiteKnight : &blackKnight;
            break;
        case PIECE_QUEEN:
            currentPiece = isWhite ? &whiteQueen : &blackQueen;
            break;
        case PIECE_KING:
            currentPiece = isWhite ? &whiteKing : &blackKing;
            break;
    }

    if(mv.promotion != '-' && mv.pieceType == PIECE_PAWN) {
        switch(mv.promotion) {
            case 'Q':
                if(isWhite) {
                    whiteQueen &= ~(1ULL << mv.endSquare);
                } else {
                    blackQueen &= ~(1ULL << mv.endSquare);
                }
                break;
            case 'R':
                if(isWhite) {
                    whiteRook &= ~(1ULL << mv.endSquare);
                } else {
                    blackRook &= ~(1ULL << mv.endSquare);
                }
                break;
            case 'B':
                if(isWhite) {
                    whiteBishop &= ~(1ULL << mv.endSquare);
                } else {
                    blackBishop &= ~(1ULL << mv.endSquare);
                }
                break;
            case 'N':
                if(isWhite) {
                    whiteKnight &= ~(1ULL << mv.endSquare);
                } else {
                    blackKnight &= ~(1ULL << mv.endSquare);
                }
                break;
        }

        *currentPiece |= (1ULL << mv.startSquare); // Place pawn back on startSquare
    } else {
        *currentPiece &= ~(1ULL << mv.endSquare);    
        *currentPiece |= (1ULL << mv.startSquare);  
    }

    if(mv.pieceCaptured != -1) {
        uint64_t *capturedPieceBitboard = nullptr;

        switch(mv.pieceCaptured) {
            case PIECE_PAWN:
                capturedPieceBitboard = isWhite ? &blackPawn : &whitePawn;
                break;
            case PIECE_ROOK:
                capturedPieceBitboard = isWhite ? &blackRook : &whiteRook;
                break;
            case PIECE_BISHOP:
                capturedPieceBitboard = isWhite ? &blackBishop : &whiteBishop;
                break;
            case PIECE_KNIGHT:
                capturedPieceBitboard = isWhite ? &blackKnight : &whiteKnight;
                break;
            case PIECE_QUEEN:
                capturedPieceBitboard = isWhite ? &blackQueen : &whiteQueen;
                break;
            case PIECE_KING:
                capturedPieceBitboard = isWhite ? &blackKing : &whiteKing;
                break;

        }
        if(capturedPieceBitboard != nullptr) {
            *capturedPieceBitboard |= (1ULL << mv.endSquare);
        }
    }
    whitePieces = whitePawn | whiteKnight | whiteBishop | whiteRook | whiteQueen | whiteKing;
    blackPieces = blackPawn | blackKnight | blackBishop | blackRook | blackQueen | blackKing;
    turn = !isWhite;

}



int main() {
    Board b;
    // cout << "\n";
    b.setupGameFromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    Move moves[256];
    int moveCount = 0;
    for(int depth = 1; depth <= 7; depth++){
        cout << b.perft(depth, true) << "\n";
    }
    return 0;
}
