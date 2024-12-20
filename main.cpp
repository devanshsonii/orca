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

int initialDepth = 5;

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
        -50,-40,-30,-30,-30,-30,-40,-50,
        -40,-20,  0,  0,  0,  0,-20,-40,
        -30,  0, 10, 15, 15, 10,  0,-30,
        -30,  5, 15, 20, 20, 15,  5,-30,
        -30,  0, 15, 20, 20, 15,  0,-30,
        -30,  5, 10, 15, 15, 10,  5,-30,
        -40,-20,  0,  5,  5,  0,-20,-40,
        -50,-40,-30,-30,-30,-30,-40,-50
    };

    int pawn_pref[64] = {
        0,  0,  0,  0,  0,  0,  0,  0,
        50, 50, 50, 50, 50, 50, 50, 50,
        10, 10, 20, 30, 30, 20, 10, 10,
        5,  5, 10, 25, 25, 10,  5,  5,
        0,  0,  0, 20, 20,  0,  0,  0,
        5, -5,-10,  0,  0,-10, -5,  5,
        5, 10, 10,-20,-20, 10, 10,  5,
        0,  0,  0,  0,  0,  0,  0,  0
    };

    int bishop_pref[64] = {
        -20,-10,-10,-10,-10,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5, 10, 10,  5,  0,-10,
        -10,  5,  5, 10, 10,  5,  5,-10,
        -10,  0, 10, 10, 10, 10,  0,-10,
        -10, 10, 10, 10, 10, 10, 10,-10,
        -10,  5,  0,  0,  0,  0,  5,-10,
        -20,-10,-10,-10,-10,-10,-10,-20
    };

    int rook_pref[64] = {
        0,  0,  0,  0,  0,  0,  0,  0,
        5, 10, 10, 10, 10, 10, 10,  5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        0,  0,  0,  5,  5,  0,  0,  0
    };
    
    int queen_pref[64] = {
        -20,-10,-10, -5, -5,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5,  5,  5,  5,  0,-10,
        -5,  0,  5,  5,  5,  5,  0, -5,
        0,  0,  5,  5,  5,  5,  0, -5,
        -10,  5,  5,  5,  5,  5,  0,-10,
        -10,  0,  5,  0,  0,  0,  0,-10,
        -20,-10,-10, -5, -5,-10,-10,-20
    };
    
    int king_pref[64] = {
        -20,-10,-10, -5, -5,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5,  5,  5,  5,  0,-10,
        -5,  0,  5,  5,  5,  5,  0, -5,
        0,  0,  5,  5,  5,  5,  0, -5,
        -10,  5,  5,  5,  5,  5,  0,-10,
        -10,  0,  5,  0,  0,  0,  0,-10,
        -20,-10,-10, -5, -5,-10,-10,-20
    };

    bool turn = true; // true = white, false = black;
    int color = 0, piece = 0, start = 0, end = 0;
    int tested = 0; 
    int bestScore = 0;

    uint64_t whiteLegalMoves = 0;
    uint64_t blackLegalMoves = 0;


    string indexToAlgebraic(int index) {
        if(index < 0 || index > 63) return "??";
        char file = 'a' + (index % 8);
        char rank = '1' + 7-(index / 8);
        return string(1, file) + string(1, rank);
    }

    // rewrite this
    // uint64_t getAttackedSquares(bool byWhite) {
    //     uint64_t attacked = 0;
    //     for(int piece = 0; piece < 5; piece++) {
    //         attacked |= generatePieceMoves(byWhite, piece);
    //     }
    //     return attacked;
    // }

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
        int i = 0, j = 0;
        for(char c: moveSet){
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
                        if (endSquare / 8 == 7) {
                            moves.emplace_back(isWhite, 0, startSquare, endSquare, -1);
                        } else {
                            moves.emplace_back(isWhite, 0, startSquare, endSquare, -1);
                        }
                        singlePush &= singlePush - 1;
                    }

                    // Double Push
                    uint64_t doublePush = ((pawns & RANK_2) >> 8) & empty;
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
                        if (endSquare / 8 == 7) {
                            moves.emplace_back(isWhite, 0, startSquare, endSquare, -1);
                        } else {
                            moves.emplace_back(isWhite, 0, startSquare, endSquare, -1);
                        }
                        leftCaptures &= leftCaptures - 1;
                    }

                    uint64_t rightCaptures = (pawns >> 7) & enemyPieces & ~A_FILE;
                    while(rightCaptures) {
                        uint64_t target = rightCaptures & -rightCaptures;
                        int endSquare = __builtin_ctzll(target);
                        int startSquare = endSquare + 7;
                        // Promotion
                        if (endSquare / 8 == 7) {
                            moves.emplace_back(isWhite, 0, startSquare, endSquare, -1);
                        } else {
                            moves.emplace_back(isWhite, 0, startSquare, endSquare, -1);
                        }
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
                        if (endSquare / 8 == 0) {
                            moves.emplace_back(isWhite, 0, startSquare, endSquare, -1);
                        } else {
                            moves.emplace_back(isWhite, 0, startSquare, endSquare, -1);
                        }
                        singlePush &= singlePush - 1;
                    }

                    // Double Push
                    uint64_t doublePush = ((pawns & RANK_7) << 8) & empty;
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
                        if (endSquare / 8 == 0) {
                            moves.emplace_back(isWhite, 0, startSquare, endSquare, -1);
                        } else {
                            moves.emplace_back(isWhite, 0, startSquare, endSquare, -1);
                        }
                        leftCaptures &= leftCaptures - 1;
                    }

                    uint64_t rightCaptures = (pawns << 7) & enemyPieces & ~H_FILE;
                    while(rightCaptures) {
                        uint64_t target = rightCaptures & -rightCaptures;
                        int endSquare = __builtin_ctzll(target);
                        int startSquare = endSquare - 7;
                        // Promotion
                        if (endSquare / 8 == 0) {
                            moves.emplace_back(isWhite, 0, startSquare, endSquare, -1);
                        } else {
                            moves.emplace_back(isWhite, 0, startSquare, endSquare, -1);
                        }
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
                
                uint64_t kingMoves = possible & ~ownPieces ;
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
        const int KNIGHT_VALUE = 320;
        const int BISHOP_VALUE = 330;
        const int ROOK_VALUE = 500;
        const int QUEEN_VALUE = 900;
        
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


};

int main() {
    Board b;
    cout << "\n";
    b.setupGameFromFEN("r6r/ppp1k1pp/2npb3/3p4/6n1/8/2PKN1RP/5q2");
    // b.setupGameFromFEN("r2qkb1r/2p2ppp/4pn2/3p4/p7/5P1P/1PPPQP2/R1B1K2R");
    // // b.generateMoves();
    b.printGame();
    bool color = false;
    for (int i = 0; i < 1; i++)
    {
        b.testMoving(color, initialDepth, -100000, 100000);
        b.makeMove(b.bestMove);
        b.printGame();
        color = !color;
    }
    
    // // cout << b.bestMove.startSquare << " " << b.bestMove.endSquare << "\n";
    // // b.printGame();

    // cout << b.count << "\n";
    // b.printGame();
    // while(true){
    //     int a, s, e;
    //     cin >> a >> s >> e;
    //     Move mv(1, a, s, e, -1);
    //     b.makeMove(mv);
    //     b.testMoving(0, initialDepth, -100000, 100000);
    //     b.makeMove(b.bestMove);
    //     b.printGame();
    // }
    return 0;
}

