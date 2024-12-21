/*
    parts of a chess engine:
        -> Board representation
        focus on move generation and moving
        -> Move generation
            -> Color-wise move generation not working
        -> Move validation
        -> Make move
            -> A function to make/unmake move
            -> Create save/restore BB
        -> Board evaluation
            -> Checks aren't working
        -> Searching

    -> en passant not working

*/

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

int initialDepth = 4;

class Move {
public:
    int color, piece, startSquare, endSquare, capturedPiece;

    Move() {
    }
    Move(uint32_t m) {
        color = (m >> 15) & 1;
        piece = (m >> 12) & 0b111;
        startSquare = (m >> 6) & 0b111111;
        endSquare = (m) & 0b111111;
        capturedPiece = -1;
    }
    Move(int color, int piece, int startSquare, int endSquare, int capturedPiece) {
        this->color = color;
        this->piece = piece;
        this->startSquare = startSquare;
        this->endSquare = endSquare;
        this->capturedPiece = capturedPiece;
    }
    void printMove() {
        switch (piece) {
            case 0: // pawn
                // cout << "P";
            // else cout << "p";
            break;
            case 1: // rook
                cout << "R";
            // else cout << "r";
            break;
            case 2: // bishop
                cout << "B";
            // else cout << "b";
            break;
            case 3: // queen
                cout << "Q";
            // else cout << "q";
            break;
            case 4: // knight
                cout << "N";
            // else cout << "n";
            break;
            case 5: // king
                cout << "K";
            // else cout << "k";
            break;
        }
        // convert endSquare into File/Rank
        char file = 'a' + (endSquare % 8);
        char rank = '1' + 7-(endSquare / 8);
        cout << string(1,file) + string(1,rank) << "\n";

    }
};

class Board { 
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

    bool turn = true; // true = white, false = black;
    int color = 0, piece = 0, start = 0, end = 0;
    int tested = 0; 
    int bestScore = 0;

    uint64_t whiteLegalMoves = 0;
    uint64_t blackLegalMoves = 0;

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

    uint16_t encodeMove(bool isWhite, int pieceType, int startSquare, int endSquare){
        uint16_t move = 0;
        move |= (isWhite ? 1 : 0) << 15;           // 1 bit for color
        move |= (pieceType & 0b111) << 12;         // 3 bits for piece type
        move |= (startSquare & 0b111111) << 6;     // 6 bits for starting square
        move |= (endSquare & 0b111111);            // 6 bits for ending square
        return move;
    }

    void decodeMove(uint16_t mv) {
        color = (mv >> 15) & 1;
        piece = (mv >> 12) & 0b111;
        start = (mv >> 6) & 0b111111;
        end = (mv) & 0b111111;
        
        // cout << color << " " << piece << " " << start << " " << end << " ";
    }

    int evaluatePieces(uint64_t pieces, const int pst[64], bool isWhite) {
        int pieceScore = 0;
        while (pieces) {
            uint64_t singlePiece = pieces & -pieces;
            int square = __builtin_ctzll(singlePiece);
            pieceScore += pst[square];
            pieces &= pieces - 1;
        }
        return pieceScore;
    }
    
    // fix this
    bool isKingInCheck(bool isWhite) {
        uint64_t king = isWhite ? whiteKing : blackKing;
        int kingSq = __builtin_ctzll(king);
        vector<Move> enemyMoves = generateMoves(!isWhite);
        for(auto &mv : enemyMoves) {
            if(mv.endSquare == kingSq) {
                return true;
            }
        }
        return false;
    }


public:
    vector<Move> moveList;
    Move bestMove;
    int count = 0;

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
            if(c == ' ') break;
            int set = i*8 + j;
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

    // something is wrong with this

    uint64_t generatePieceMoves(bool isWhite, int pieceType, vector<Move>& moves) {
        uint64_t movesBits = 0;
        uint64_t ownPieces = isWhite ? whitePieces : blackPieces;
        uint64_t enemyPieces = isWhite ? blackPieces : whitePieces;
        
        switch(pieceType) {
            case 0: { // Pawn
                uint64_t pawns = isWhite ? whitePawn : blackPawn;
                uint64_t empty = ~(whitePieces | blackPieces);
                
                if (isWhite) {
                    // Single Push
                    uint64_t singlePush = (pawns >> 8) & empty;
                    while(singlePush) {
                        uint64_t target = singlePush & -singlePush;
                        int endSquare = __builtin_ctzll(target);
                        int startSquare = endSquare + 8;
                        // Promotion
                        moves.emplace_back(isWhite, 0, startSquare, endSquare, -1);
                        singlePush &= singlePush - 1;
                    }

                    // Double Push
                    uint64_t doublePush = ((pawns & RANK_7) >> 8) & empty;
                    doublePush = (doublePush >> 8) & empty;
                    while(doublePush) {
                        uint64_t target = doublePush & -doublePush;
                        int endSquare = __builtin_ctzll(target);
                        int startSquare = endSquare + 16;
                        moves.emplace_back(isWhite, 0, startSquare, endSquare, -1);
                        doublePush &= doublePush - 1;
                    }

                    // Captures
                    uint64_t leftCaptures = (pawns >> 9) & enemyPieces & ~H_FILE;
                    while(leftCaptures) {
                        uint64_t target = leftCaptures & -leftCaptures;
                        int endSquare = __builtin_ctzll(target);
                        int startSquare = endSquare + 9;
                        // Promotion
                        moves.emplace_back(isWhite, 0, startSquare, endSquare, -1);
                        leftCaptures &= leftCaptures - 1;
                    }

                    uint64_t rightCaptures = (pawns >> 7) & enemyPieces & ~A_FILE;
                    while(rightCaptures) {
                        uint64_t target = rightCaptures & -rightCaptures;
                        int endSquare = __builtin_ctzll(target);
                        int startSquare = endSquare + 7;
                        // Promotion
                        moves.emplace_back(isWhite, 0, startSquare, endSquare, -1);
                        rightCaptures &= rightCaptures - 1;
                    }
                } else {
                    // Single Push
                    uint64_t singlePush = (pawns << 8) & empty;
                    while(singlePush) {
                        uint64_t target = singlePush & -singlePush;
                        int endSquare = __builtin_ctzll(target);
                        int startSquare = endSquare - 8;
                        // Promotion
                        moves.emplace_back(isWhite, 0, startSquare, endSquare, -1);
                        singlePush &= singlePush - 1;
                    }

                    // Double Push
                    uint64_t doublePush = ((pawns & RANK_2) << 8) & empty;
                    doublePush = (doublePush << 8) & empty;
                    while(doublePush) {
                        uint64_t target = doublePush & -doublePush;
                        int endSquare = __builtin_ctzll(target);
                        int startSquare = endSquare - 16;
                        moves.emplace_back(isWhite, 0, startSquare, endSquare, -1);
                        doublePush &= doublePush - 1;
                    }

                    // Captures
                    uint64_t leftCaptures = (pawns << 9) & enemyPieces & ~A_FILE;
                    while(leftCaptures) {
                        uint64_t target = leftCaptures & -leftCaptures;
                        int endSquare = __builtin_ctzll(target);
                        int startSquare = endSquare - 9;
                        // Promotion
                        moves.emplace_back(isWhite, 0, startSquare, endSquare, -1);
                        leftCaptures &= leftCaptures - 1;
                    }

                    uint64_t rightCaptures = (pawns << 7) & enemyPieces & ~H_FILE;
                    while(rightCaptures) {
                        uint64_t target = rightCaptures & -rightCaptures;
                        int endSquare = __builtin_ctzll(target);
                        int startSquare = endSquare - 7;
                        // Promotion
                        moves.emplace_back(isWhite, 0, startSquare, endSquare, -1);
                        rightCaptures &= rightCaptures - 1;
                    }
                }

                break;
            }
            case 1: { // Rook
                uint64_t rooks = isWhite ? whiteRook : blackRook;
                while(rooks) {
                    uint64_t singleRook = rooks & -rooks;
                    uint64_t rookMoves = 0;
                    uint64_t shift;
                    
                    // North
                    shift = singleRook;
                    while((shift <<= 8) && !(shift & ownPieces)) {
                        rookMoves |= shift;
                        if(shift & enemyPieces) break;
                    }
                    // South
                    shift = singleRook;
                    while((shift >>= 8) && !(shift & ownPieces)) {
                        rookMoves |= shift;
                        if(shift & enemyPieces) break;
                    }
                    
                    // East
                    shift = singleRook;
                    while((shift = (shift & ~H_FILE) << 1) && !(shift & ownPieces)) {
                        rookMoves |= shift;
                        if(shift & enemyPieces) break;
                    }
                    
                    // West
                    shift = singleRook;
                    while((shift = (shift & ~A_FILE) >> 1) && !(shift & ownPieces)) {
                        rookMoves |= shift;
                        if(shift & enemyPieces) break;
                    }
                    movesBits |= rookMoves;

                    int startSquare = __builtin_ctzll(singleRook);

                    uint64_t targets = rookMoves;
                    while(targets) {
                        uint64_t target = targets & -targets;
                        int endSquare = __builtin_ctzll(target);
                        if(startSquare != endSquare) {
                            moves.emplace_back(isWhite, 1, startSquare, endSquare, -1);
                        }
                        targets &= targets - 1;
                    }

                    rooks &= rooks - 1;
                }
                break;
            }
            case 2: { // Bishop
                uint64_t bishops = isWhite ? whiteBishop : blackBishop;
                while(bishops) {
                    uint64_t singleBishop = bishops & -bishops;
                    uint64_t bishopMoves = 0;
                    uint64_t shift;
                    
                    // NorthEast
                    shift = singleBishop;
                    while((shift = (shift & ~H_FILE) << 9) && !(shift & ownPieces)) {
                        bishopMoves |= shift;
                        if(shift & enemyPieces) break;
                    }
                    
                    // NorthWest
                    shift = singleBishop;
                    while((shift = (shift & ~A_FILE) << 7) && !(shift & ownPieces)) {
                        bishopMoves |= shift;
                        if(shift & enemyPieces) break;
                    }
                    
                    // SouthEast
                    shift = singleBishop;
                    while((shift = (shift & ~H_FILE) >> 7) && !(shift & ownPieces)) {
                        bishopMoves |= shift;
                        if(shift & enemyPieces) break;
                    }
                    
                    // SouthWest
                    shift = singleBishop;
                    while((shift = (shift & ~A_FILE) >> 9) && !(shift & ownPieces)) {
                        bishopMoves |= shift;
                        if(shift & enemyPieces) break;
                    }
                    movesBits |= bishopMoves;

                    int startSquare = __builtin_ctzll(singleBishop);

                    uint64_t targets = bishopMoves;
                    while(targets) {
                        uint64_t target = targets & -targets;
                        int endSquare = __builtin_ctzll(target);
                        if(startSquare != endSquare) {
                            moves.emplace_back(isWhite, 2, startSquare, endSquare, -1);
                        }
                        targets &= targets - 1;
                    }

                    bishops &= bishops - 1;
                }
                break;
            }
            case 3: { // Queen
                uint64_t queens = isWhite ? whiteQueen : blackQueen;
                while(queens) {
                    uint64_t singleQueen = queens & -queens;
                    uint64_t queenMoves = 0;
                    uint64_t shift;
                    
                    // North
                    shift = singleQueen;
                    while((shift <<= 8) && !(shift & ownPieces)) {
                        queenMoves |= shift;
                        if(shift & enemyPieces) break;
                    }
                    // South
                    shift = singleQueen;
                    while((shift >>= 8) && !(shift & ownPieces)) {
                        queenMoves |= shift;
                        if(shift & enemyPieces) break;
                    }
                    // East
                    shift = singleQueen;
                    while((shift = (shift & ~H_FILE) << 1) && !(shift & ownPieces)) {
                        queenMoves |= shift;
                        if(shift & enemyPieces) break;
                    }
                    // West
                    shift = singleQueen;
                    while((shift = (shift & ~A_FILE) >> 1) && !(shift & ownPieces)) {
                        queenMoves |= shift;
                        if(shift & enemyPieces) break;
                    }
                    // NorthEast
                    shift = singleQueen;
                    while((shift = (shift & ~H_FILE) << 9) && !(shift & ownPieces)) {
                        queenMoves |= shift;
                        if(shift & enemyPieces) break;
                    }
                    // NorthWest
                    shift = singleQueen;
                    while((shift = (shift & ~A_FILE) << 7) && !(shift & ownPieces)) {
                        queenMoves |= shift;
                        if(shift & enemyPieces) break;
                    }
                    // SouthEast
                    shift = singleQueen;
                    while((shift = (shift & ~H_FILE) >> 7) && !(shift & ownPieces)) {
                        queenMoves |= shift;
                        if(shift & enemyPieces) break;
                    }
                    // SouthWest
                    shift = singleQueen;
                    while((shift = (shift & ~A_FILE) >> 9) && !(shift & ownPieces)) {
                        queenMoves |= shift;
                        if(shift & enemyPieces) break;
                    }
                    movesBits |= queenMoves;

                    int startSquare = __builtin_ctzll(singleQueen);

                    uint64_t targets = queenMoves;
                    while(targets) {
                        uint64_t target = targets & -targets;
                        int endSquare = __builtin_ctzll(target);
                        if(startSquare != endSquare) {
                            moves.emplace_back(isWhite, 3, startSquare, endSquare, -1);
                        }
                        targets &= targets - 1;
                    }

                    queens &= queens - 1;
                }
                break;
            }
            case 4: { // Knight
                uint64_t knights = isWhite ? whiteKnight : blackKnight;
                while(knights) {
                    uint64_t knight = knights & -knights;
                    uint64_t possible = 0;
                    
                    possible |= (knight << 17) & ~A_FILE;
                    possible |= (knight << 15) & ~H_FILE;
                    possible |= (knight << 10) & ~(A_FILE | B_FILE);
                    possible |= (knight << 6) & ~(G_FILE | H_FILE);
                    possible |= (knight >> 6) & ~(A_FILE | B_FILE);
                    possible |= (knight >> 10) & ~(G_FILE | H_FILE);
                    possible |= (knight >> 15) & ~A_FILE;
                    possible |= (knight >> 17) & ~H_FILE;
                    
                    uint64_t knightMoves = possible & ~ownPieces;
                    movesBits |= knightMoves;

                    int startSquare = __builtin_ctzll(knight);

                    uint64_t targets = knightMoves;
                    while(targets) {
                        uint64_t target = targets & -targets;
                        int endSquare = __builtin_ctzll(target);
                        if(startSquare != endSquare) {
                            moves.emplace_back(isWhite, 4, startSquare, endSquare, -1);
                        }
                        targets &= targets - 1;
                    }

                    knights &= knights - 1;
                }
                break;
            }
            case 5: { // King
                uint64_t king = isWhite ? whiteKing : blackKing;
                uint64_t possible = 0;
                // uint64_t enemyAttacks = getAttackedSquares(!isWhite);
    
                possible |= (king << 8) | (king >> 8);
                possible |= (king << 1) & ~A_FILE;
                possible |= (king >> 1) & ~H_FILE;
                possible |= (king << 9) & ~A_FILE;
                possible |= (king >> 7) & ~A_FILE;
                possible |= (king << 7) & ~H_FILE;
                possible |= (king >> 9) & ~H_FILE;

                vector<Move> temp;
                uint64_t enemyAttack = 0;
                for(int i = 0; i < 5; i++){
                    enemyAttack |= generatePieceMoves(!isWhite, i, temp);
                }

                uint64_t opponentKing = isWhite ? blackKing : whiteKing;
                uint64_t opponentKingMoves = 0;
                opponentKingMoves |= (opponentKing << 8) | (opponentKing >> 8);
                opponentKingMoves |= (opponentKing << 1) & ~A_FILE;
                opponentKingMoves |= (opponentKing >> 1) & ~H_FILE;
                opponentKingMoves |= (opponentKing << 9) & ~A_FILE;
                opponentKingMoves |= (opponentKing >> 7) & ~A_FILE;
                opponentKingMoves |= (opponentKing << 7) & ~H_FILE;
                opponentKingMoves |= (opponentKing >> 9) & ~H_FILE;
                enemyAttack |= opponentKingMoves;

                
                uint64_t kingMoves = possible & ~ownPieces & ~enemyAttack;
                movesBits |= kingMoves;

                int startSquare = __builtin_ctzll(king);
    
                uint64_t targets = kingMoves;
                while(targets) {
                    uint64_t target = targets & -targets;
                    int endSquare = __builtin_ctzll(target);
                    if(startSquare != endSquare) {
                        moves.emplace_back(isWhite, 5, startSquare, endSquare, -1);
                    }
                    targets &= targets - 1;
                }
                break;
            }
        }
        return movesBits;
    }

    vector<Move> generateMoves(bool isWhite) {
        vector<Move> localMoveList; // Local move list for this invocation

        // Iterate over all piece types (0: Pawn, 1: Rook, 2: Bishop, 3: Queen, 4: Knight, 5: King)
        for(int pieceType = 0; pieceType < 6; pieceType++) {
            generatePieceMoves(isWhite, pieceType, localMoveList);
        }

        return localMoveList;
    }

        
    int evaluateBoard() {
        int score = 0;
        const int PAWN_VALUE = 100;
        const int KNIGHT_VALUE = 280;
        const int BISHOP_VALUE = 320;
        const int ROOK_VALUE = 479;
        const int QUEEN_VALUE = 929;
        const int KING_VALUE = 60000;
        // Material counting
        int whiteMaterial = 0;
        int blackMaterial = 0;
        
        // Count white material
        whiteMaterial += __builtin_popcountll(whitePawn) * PAWN_VALUE;
        whiteMaterial += __builtin_popcountll(whiteKnight) * KNIGHT_VALUE;
        whiteMaterial += __builtin_popcountll(whiteBishop) * BISHOP_VALUE;
        whiteMaterial += __builtin_popcountll(whiteRook) * ROOK_VALUE;
        whiteMaterial += __builtin_popcountll(whiteQueen) * QUEEN_VALUE;
        
        // Count black material
        blackMaterial += __builtin_popcountll(blackPawn) * PAWN_VALUE;
        blackMaterial += __builtin_popcountll(blackKnight) * KNIGHT_VALUE;
        blackMaterial += __builtin_popcountll(blackBishop) * BISHOP_VALUE;
        blackMaterial += __builtin_popcountll(blackRook) * ROOK_VALUE;
        blackMaterial += __builtin_popcountll(blackQueen) * QUEEN_VALUE;

        
        score += (whiteMaterial - blackMaterial);
        
        // Positional evaluation using piece-square tables
        score += evaluatePieces(whitePawn, pawn_pref, true);
        score += evaluatePieces(whiteRook, rook_pref, true);
        score += evaluatePieces(whiteKnight, knight_pref, true);
        score += evaluatePieces(whiteBishop, bishop_pref, true);
        score += evaluatePieces(whiteQueen, queen_pref, true);
        score += evaluatePieces(whiteKing, king_pref, true);
        
        score -= evaluatePieces(blackPawn, pawn_pref, false);
        score -= evaluatePieces(blackRook, rook_pref, false);
        score -= evaluatePieces(blackKnight, knight_pref, false);
        score -= evaluatePieces(blackBishop, bishop_pref, false);
        score -= evaluatePieces(blackQueen, queen_pref, false);
        score -= evaluatePieces(blackKing, king_pref, false);
        
        
        // King safety
        if(isKingInCheck(true)) score -= 50000;
        if(isKingInCheck(false)) score += 50000;
        
        return score;
    }

    // rewrite this

    void makeMove(Move& mv) {
        uint64_t *currentPiece = nullptr;

        int color = mv.color;
        int piece = mv.piece;
        int start = mv.startSquare;
        int end = mv.endSquare;
        int capturedPiece = -1;
    
        switch (piece) {
            case 0: // Pawn
                currentPiece = (!color ? &blackPawn : &whitePawn);
                break;
            case 1: // Rook
                currentPiece = (!color ? &blackRook : &whiteRook);
                break;
            case 2: // Bishop
                currentPiece = (!color ? &blackBishop : &whiteBishop);
                break;
            case 3: // Queen
                currentPiece = (!color ? &blackQueen : &whiteQueen);
                break;
            case 4: // Knight
                currentPiece = (!color ? &blackKnight : &whiteKnight);
                break;
            case 5: // King
                currentPiece = (!color ? &blackKing : &whiteKing);
                break;
            default:
                cout << "Invalid piece type!\n";
                return;
        }
    
        // Check for captures
        if (color) { // White to move
            if (blackPieces & (1ULL << end)) {
                if (blackPawn & (1ULL << end)) { blackPawn &= ~(1ULL << end); capturedPiece = 0; }
                else if (blackRook & (1ULL << end)) { blackRook &= ~(1ULL << end); capturedPiece = 1; }
                else if (blackBishop & (1ULL << end)) { blackBishop &= ~(1ULL << end); capturedPiece = 2; }
                else if (blackKnight & (1ULL << end)) { blackKnight &= ~(1ULL << end); capturedPiece = 4; }
                else if (blackQueen & (1ULL << end)) { blackQueen &= ~(1ULL << end); capturedPiece = 3; }
                else if (blackKing & (1ULL << end)) { blackKing &= ~(1ULL << end); capturedPiece = 5; }
            }
        } else { // Black to move
            if (whitePieces & (1ULL << end)) {
                if (whitePawn & (1ULL << end)) { whitePawn &= ~(1ULL << end); capturedPiece = 0; }
                else if (whiteRook & (1ULL << end)) { whiteRook &= ~(1ULL << end); capturedPiece = 1; }
                else if (whiteBishop & (1ULL << end)) { whiteBishop &= ~(1ULL << end); capturedPiece = 2; }
                else if (whiteKnight & (1ULL << end)) { whiteKnight &= ~(1ULL << end); capturedPiece = 4; }
                else if (whiteQueen & (1ULL << end)) { whiteQueen &= ~(1ULL << end); capturedPiece = 3; }
                else if (whiteKing & (1ULL << end)) { whiteKing &= ~(1ULL << end); capturedPiece = 5; }
            }
        }
    
        // Move the piece
        *currentPiece &= ~(1ULL << start);
        *currentPiece |= (1ULL << end);
    
        // Handle promotions
        if (piece == 0) { // Pawn
            if ((color && (end / 8) == 7) || (!color && (end / 8) == 0)) {
                // Promote to Queen by default
                *currentPiece &= ~(1ULL << end); // Remove Pawn
                if (color) whiteQueen |= (1ULL << end); // Promote to White Queen
                else blackQueen |= (1ULL << end); // Promote to Black Queen
                piece = 3; // Update piece type to Queen
            }
        }
    
        // Update combined piece bitboards
        whitePieces = whitePawn | whiteRook | whiteBishop | whiteKnight | whiteQueen | whiteKing;
        blackPieces = blackPawn | blackRook | blackBishop | blackKnight | blackQueen | blackKing;
    
        // Update move with captured piece
        mv.capturedPiece = capturedPiece;
        turn = !turn;
    }
    
    void unMakeMove(Move& mv) {
        uint64_t *currentPiece = nullptr;
        int color = mv.color;
        int piece = mv.piece;
        int start = mv.startSquare;
        int end = mv.endSquare;
        int capturedPiece = mv.capturedPiece;
    
        switch (piece) {
            case 0: // Pawn
                currentPiece = (!color ? &blackPawn : &whitePawn);
                break;
            case 1: // Rook
                currentPiece = (!color ? &blackRook : &whiteRook);
                break;
            case 2: // Bishop
                currentPiece = (!color ? &blackBishop : &whiteBishop);
                break;
            case 3: // Queen
                currentPiece = (!color ? &blackQueen : &whiteQueen);
                break;
            case 4: // Knight
                currentPiece = (!color ? &blackKnight : &whiteKnight);
                break;
            case 5: // King
                currentPiece = (!color ? &blackKing : &whiteKing);
                break;
            default:
                cout << "Invalid piece type during unMakeMove!\n";
                return;
        }
    
        // Move the piece back
        *currentPiece &= ~(1ULL << end);
        *currentPiece |= (1ULL << start);
    
        // Handle captured pieces
        if (capturedPiece != -1) {
            if (color) { // White had captured
                switch (capturedPiece) {
                    case 0: blackPawn |= (1ULL << end); break;
                    case 1: blackRook |= (1ULL << end); break;
                    case 2: blackBishop |= (1ULL << end); break;
                    case 3: blackQueen |= (1ULL << end); break;
                    case 4: blackKnight |= (1ULL << end); break;
                    case 5: blackKing |= (1ULL << end); break;
                    default: break;
                }
            } else { // Black had captured
                switch (capturedPiece) {
                    case 0: whitePawn |= (1ULL << end); break;
                    case 1: whiteRook |= (1ULL << end); break;
                    case 2: whiteBishop |= (1ULL << end); break;
                    case 3: whiteQueen |= (1ULL << end); break;
                    case 4: whiteKnight |= (1ULL << end); break;
                    case 5: whiteKing |= (1ULL << end); break;
                    default: break;
                }
            }
        }
    
        // Handle promotions
        if (piece == 0) { // Pawn
            if ((color && (start / 8) == 6) || (!color && (start / 8) == 1)) {
                // Revert promotion to Pawn
                *currentPiece &= ~(1ULL << start); // Remove Queen
                if (color) whitePawn |= (1ULL << start); // Revert to White Pawn
                else blackPawn |= (1ULL << start); // Revert to Black Pawn
            }
        }
    
        // Update combined piece bitboards
        whitePieces = whitePawn | whiteRook | whiteBishop | whiteKnight | whiteQueen | whiteKing;
        blackPieces = blackPawn | blackRook | blackBishop | blackKnight | blackQueen | blackKing;
        turn = !turn;
    }


    // int testMoving(bool isWhite, int depth) {
    //     vector<Move> moves = generateMoves(isWhite);
    //     // Base case
    //     if(depth == 0) {
    //         count++;
    //         return evaluateBoard();
    //     }
    //     int localBest = isWhite ? -100000 : 100000;
    //     Move bestLocalMove;
    //     for(auto& mv: moves) {
    //         makeMove(mv);
    //         int score = testMoving(!isWhite, depth - 1);
    //         unMakeMove(mv);
    //         if(isWhite) {
    //             // if(score > localBest) {
    //             //     localBest = score;
    //             //     bestLocalMove = mv;
    //             // }
    //         } else {
    //             // if(score < localBest) {
    //             //     localBest = score;
    //             //     bestLocalMove = mv;
    //             // }
    //         }
    //     }
    //     if(depth == initialDepth) {
    //         bestMove = bestLocalMove;
    //     }
    //     return localBest;
    // }

    int testMoving(bool isWhite, int depth, int alpha, int beta) {
        vector<Move> moves = generateMoves(isWhite);
        
        // Base case
        if (depth == 0) {
            return evaluateBoard();
        }
        
        int localBest = isWhite ? -100000 : 100000;
        Move bestLocalMove;
        
        for (auto& mv : moves) {
            makeMove(mv);
            int score = testMoving(!isWhite, depth - 1, alpha, beta);
            unMakeMove(mv);
            
            if (isWhite) {
                if (score > localBest) {
                    localBest = score;
                    bestLocalMove = mv;
                }
                alpha = max(alpha, localBest);
            } else {
                if (score < localBest) {
                    localBest = score;
                    bestLocalMove = mv;
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

    void generatorTest() {
        vector<Move> testMoves = generateMoves(turn);
        vector<Move> temp = generateMoves(!turn);
        for(Move it: testMoves) {
            it.printMove();
            makeMove(it);
            outputFen();
            cout << "\n";
            unMakeMove(it);
        }
        for(Move it: temp) {
            it.printMove();
            makeMove(it);
            outputFen();
            cout << "\n";
            unMakeMove(it);
        }
    }

    void play() {
        while(true) {
            printGame();
            string mv;
            cout << "Enter your move: ";
            cin >> mv;
    
            char start_file = mv[0];
            char start_rank = mv[1];
            char end_file = mv[2];
            char end_rank = mv[3];
    
            int start = (7 - (start_rank - '1')) * 8 + (start_file - 'a');
            int end = (7-(end_rank - '1')) * 8 + (end_file - 'a');
    
            int piece = -1;
            if(whitePawn & (1ULL << start)) piece = 0;
            else if(whiteRook & (1ULL << start)) piece = 1;
            else if(whiteBishop & (1ULL << start)) piece = 2;
            else if(whiteQueen & (1ULL << start)) piece = 3;
            else if(whiteKnight & (1ULL << start)) piece = 4;
            else if(whiteKing & (1ULL << start)) piece = 5;
    
            Move userMove(true, piece, start, end, -1);
            makeMove(userMove);
    
            printGame();
    
            cout << "Engine is thinking...\n";
            testMoving(false, initialDepth, -100000, 100000);
    
            makeMove(bestMove);
    
            // printGame();
    
            // TODO: Implement game termination checks
        }
    }

    int perft(bool isWhite, int depth) {
        if (depth == 0) return 1; // Leaf node, count as 1 move.

        int nodes = 0;
        vector<Move> moves;
        moves = generateMoves(isWhite); // Generate all legal moves.
        for (Move mv : moves) {
            makeMove(mv);       // Apply the move.
            nodes += perft(!isWhite, depth - 1); // Recursive call.
            unMakeMove(mv);     // Undo the move.
        }

        return nodes;
    }

};

int main() {
    Board b;
    // cout << "\n";
    b.setupGameFromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w - - 0 1");
    // b.play();
    for(int depth = 1; depth <= 6; depth++){
        int sum = b.perft(true, depth);
        cout << sum << "\n";
    }
    // bool color = true;
    // b.testMoving(color, initialDepth, -100000, 100000);
    // for (int i = 0; i < 100; i++)
    // {
    //     b.makeMove(b.bestMove);
    //     b.printGame();
    //     cout << b.evaluateBoard() << "\n"; 
    //     color = !color;
    // }
    return 0;
}

