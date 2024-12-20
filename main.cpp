/*
    parts of a chess engine:
        -> Board representation
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

int initialDepth;

class Move {
public:
    int color, piece, startSquare, endSquare;
    Move(uint32_t m) {
        color = (m >> 15) & 1;
        piece = (m >> 12) & 0b111;
        startSquare = (m >> 6) & 0b111111;
        endSquare = (m) & 0b111111;
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

    uint64_t whiteLegalMoves = 0;
    uint64_t blackLegalMoves = 0;

    string indexToAlgebraic(int index) {
        if(index < 0 || index > 63) return "??";
        char file = 'a' + (index % 8);
        char rank = '1' + 7-(index / 8);
        return string(1, file) + string(1, rank);
    }

    // rewrite this
    uint64_t getAttackedSquares(bool byWhite) {
        uint64_t attacked = 0;
        for(int piece = 0; piece < 5; piece++) {
            attacked |= generatePieceMoves(byWhite, piece);
        }
        return attacked;
    }

public:
    vector<uint32_t> moveList;
    uint32_t bestMove;


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

    void setupGame(string moveSet) {
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

    // rewrite this

    int evaluateBoard() {
        int score = 0;
        for (int rank = 0; rank < 8; rank++) {
            for (int file = 0; file < 8; file++) {
                int square = (7 - rank) * 8 + file;
                int mirrorSquare = square ^ 56;
    
                if (whitePawn & (1ULL << square))
                    score += 1 + pawn_pref[square];
                else if (whiteKnight & (1ULL << square))
                    score += 3 + knight_pref[square];
                else if (whiteBishop & (1ULL << square))
                    score += 3 + bishop_pref[square];
                else if (whiteRook & (1ULL << square))
                    score += 5 + rook_pref[square];
                else if (whiteQueen & (1ULL << square))
                    score += 9 + queen_pref[square];
                else if (whiteKing & (1ULL << square))
                    score += king_pref[square];
                else if (blackPawn & (1ULL << square))
                    score -= 1 + pawn_pref[mirrorSquare];
                else if (blackKnight & (1ULL << square))
                    score -= 3 + knight_pref[mirrorSquare];
                else if (blackBishop & (1ULL << square))
                    score -= 3 + bishop_pref[mirrorSquare];
                else if (blackRook & (1ULL << square))
                    score -= 5 + rook_pref[mirrorSquare];
                else if (blackQueen & (1ULL << square))
                    score -= 9 + queen_pref[mirrorSquare];
                else if (blackKing & (1ULL << square))
                    score -= king_pref[mirrorSquare];
            }
        }
        return score;
    }    
    
    // something is wrong with this

    uint64_t generatePieceMoves(bool isWhite, int pieceType) {
        uint64_t moves = 0;
        uint64_t ownPieces = isWhite ? whitePieces : blackPieces;
        uint64_t enemyPieces = isWhite ? blackPieces : whitePieces;
        
        switch(pieceType) {
            case 0: { // Pawn
                uint64_t pawns = isWhite ? whitePawn : blackPawn;
                uint64_t empty = ~(whitePieces | blackPieces);
            
                if (isWhite) {
                    uint64_t singlePush = (pawns >> 8) & empty;
                    uint64_t doublePush = ((pawns & RANK_7) >> 8) & empty;
                    doublePush = (doublePush >> 8) & empty;
                    uint64_t leftCaptures = (pawns >> 9) & blackPieces & ~H_FILE; 
                    uint64_t rightCaptures = (pawns >> 7) & blackPieces & ~A_FILE;
                    moves = singlePush | doublePush | leftCaptures | rightCaptures;
                    uint64_t promotions = (singlePush & RANK_8) | (leftCaptures & RANK_8) | (rightCaptures & RANK_8);
                    moves |= promotions;
                } else {
                    uint64_t singlePush = (pawns << 8) & empty;
                    uint64_t doublePush = ((pawns & RANK_2) << 8) & empty;
                    doublePush = (doublePush << 8) & empty;
                    uint64_t leftCaptures = (pawns << 9) & whitePieces & ~A_FILE; 
                    uint64_t rightCaptures = (pawns << 7) & whitePieces & ~H_FILE; 
                    moves = singlePush | doublePush | leftCaptures | rightCaptures;
                    uint64_t promotions = (singlePush & RANK_1) | (leftCaptures & RANK_1) | (rightCaptures & RANK_1);
                    moves |= promotions;
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
                    moves = 0;
                    moves |= rookMoves;

                    int startSquare = __builtin_ctzll(singleRook);

                    for(int i = 0; i < 64; i++){
                        if(moves & (1ULL << i)) {
                            if(startSquare == i) continue;
                            moveList.push_back(encodeMove(isWhite, 1, startSquare, i));
                        }
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
                    moves = 0;
                    moves |= bishopMoves;

                    int startSquare = __builtin_ctzll(singleBishop);

                    for(int i = 0; i < 64; i++){
                        if(moves & (1ULL << i)) {
                            if(startSquare == i) continue;
                            moveList.push_back(encodeMove(isWhite, 2, startSquare, i));
                        }
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

                    moves = 0;
                    moves |= queenMoves;

                    int startSquare = __builtin_ctzll(singleQueen);

                    for(int i = 0; i < 64; i++){
                        if(moves & (1ULL << i)) {
                            if(startSquare == i) continue;
                            moveList.push_back(encodeMove(isWhite, 3, startSquare, i));
                        }
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
                    
                    moves = 0;
                    
                    possible &= ~ownPieces;
                    moves |= possible;

                    int startSquare = __builtin_ctzll(knight);

                    for(int i = 0; i < 64; i++){
                        if(moves & (1ULL << i)) {
                            if(startSquare == i) continue;
                            moveList.push_back(encodeMove(isWhite, 4, startSquare, i));
                        }
                    }


                    knights &= knights - 1;
                }
                break;
            }
            case 5: { // King
                uint64_t king = isWhite ? whiteKing : blackKing;
                uint64_t possible = 0;
                uint64_t enemyAttacks = getAttackedSquares(!isWhite);

                possible |= (king << 8) | (king >> 8);
                possible |= (king << 1 & ~A_FILE) | (king >> 1 & ~H_FILE);
                possible |= (king << 9 & ~A_FILE) | (king >> 7 & ~A_FILE);
                possible |= (king << 7 & ~H_FILE) | (king >> 9 & ~H_FILE);
                
                moves = 0;

                possible &= ~ownPieces;
                possible &= ~enemyAttacks;
                moves |= possible;
                
                int startSquare = __builtin_ctzll(king);

                for(int i = 0; i < 64; i++){
                    if(moves & (1ULL << i)) {
                        if(startSquare == i) continue;
                        moveList.push_back(encodeMove(isWhite, 5, startSquare, i));
                    }
                }
                break;
            }
        }
        return moves;
    }

    void generateMoves(bool isWhite) {
        if(isWhite) {
            for(int piece = 0; piece < 6; piece++) {
                whiteLegalMoves |= generatePieceMoves(true, piece);
            }
        } else {
            for(int piece = 0; piece < 6; piece++) {
                blackLegalMoves |= generatePieceMoves(false, piece);
            }
        }
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
    

    void moveTest(uint32_t m) {
        Move mv(m);
        
    }
    // rewrite this

    void makeMove() {
        uint64_t *currentPiece = nullptr;
    
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
        }
    
        *currentPiece &= ~(1ULL << start); 
        *currentPiece |= (1ULL << end);    
    
        // Captures
        if(color) { 
            if(blackPieces & (1ULL << end)) { // If there's a black piece at the destination
                if(blackPawn & (1ULL << end)) blackPawn &= ~(1ULL << end);
                else if(blackRook & (1ULL << end)) blackRook &= ~(1ULL << end);
                else if(blackBishop & (1ULL << end)) blackBishop &= ~(1ULL << end);
                else if(blackKnight & (1ULL << end)) blackKnight &= ~(1ULL << end);
                else if(blackQueen & (1ULL << end)) blackQueen &= ~(1ULL << end);
                else if(blackKing & (1ULL << end)) blackKing &= ~(1ULL << end);
            }
        } else { 
            if(whitePieces & (1ULL << end)) { // If there's a white piece at the destination
                if(whitePawn & (1ULL << end)) whitePawn &= ~(1ULL << end);
                else if(whiteRook & (1ULL << end)) whiteRook &= ~(1ULL << end);
                else if(whiteBishop & (1ULL << end)) whiteBishop &= ~(1ULL << end);
                else if(whiteKnight & (1ULL << end)) whiteKnight &= ~(1ULL << end);
                else if(whiteQueen & (1ULL << end)) whiteQueen &= ~(1ULL << end);
                else if(whiteKing & (1ULL << end)) whiteKing &= ~(1ULL << end);
            }
        }
    
        whitePieces = whitePawn | whiteRook | whiteBishop | whiteKnight | whiteQueen | whiteKing;
        blackPieces = blackPawn | blackRook | blackBishop | blackKnight | blackQueen | blackKing;
    }
    
    // void test() {
    //     int mx = -100000;
    //     int count = 0;
    //     uint32_t bestMv = 0;
    //     for(auto it: moveList) {
    //         decodeMove(it);
    //         if(color) {
    //             int temp = makeMove();
    //             count++;
    //             if(temp > mx) {
    //                 bestMv = it;
    //                 mx = temp;
    //             }
    //         }
    //     }
    //     cout << "Moves tested: " << count << "\n";
    //     cout << "----Best  Move----\n";
    //     printGame();
    //     decodeMove(bestMv);
    //     cout << color << " " << piece << " " << indexToAlgebraic(start) << "->" << indexToAlgebraic(end) << "\n";
    // }

    int testEvaluate() {
        int score = 0;
        // Piece values
        const int pawnValue = 100;
        const int knightValue = 300;
        const int bishopValue = 300;
        const int rookValue = 500;
        const int queenValue = 900;

        // Helper lambda to calculate piece-square table index
        auto pieceSquareIndex = [](bool isWhite, int square) {
            return isWhite ? square : 63 - square;
        };

        // Material and positional evaluation
        uint64_t bitboard;

        // Pawns
        bitboard = whitePawn;
        while (bitboard) {
            int square = __builtin_ctzll(bitboard);
            score += pawnValue + pawn_pref[pieceSquareIndex(true, square)];
            bitboard &= bitboard - 1;
        }
        bitboard = blackPawn;
        while (bitboard) {
            int square = __builtin_ctzll(bitboard);
            score -= pawnValue + pawn_pref[pieceSquareIndex(false, square)];
            bitboard &= bitboard - 1;
        }

        // Knights
        bitboard = whiteKnight;
        while (bitboard) {
            int square = __builtin_ctzll(bitboard);
            score += knightValue + knight_pref[pieceSquareIndex(true, square)];
            bitboard &= bitboard - 1;
        }
        bitboard = blackKnight;
        while (bitboard) {
            int square = __builtin_ctzll(bitboard);
            score -= knightValue + knight_pref[pieceSquareIndex(false, square)];
            bitboard &= bitboard - 1;
        }

        // Bishops
        bitboard = whiteBishop;
        while (bitboard) {
            int square = __builtin_ctzll(bitboard);
            score += bishopValue + bishop_pref[pieceSquareIndex(true, square)];
            bitboard &= bitboard - 1;
        }
        bitboard = blackBishop;
        while (bitboard) {
            int square = __builtin_ctzll(bitboard);
            score -= bishopValue + bishop_pref[pieceSquareIndex(false, square)];
            bitboard &= bitboard - 1;
        }

        // Rooks
        bitboard = whiteRook;
        while (bitboard) {
            int square = __builtin_ctzll(bitboard);
            score += rookValue + rook_pref[pieceSquareIndex(true, square)];
            bitboard &= bitboard - 1;
        }
        bitboard = blackRook;
        while (bitboard) {
            int square = __builtin_ctzll(bitboard);
            score -= rookValue + rook_pref[pieceSquareIndex(false, square)];
            bitboard &= bitboard - 1;
        }

        // Queens
        bitboard = whiteQueen;
        while (bitboard) {
            int square = __builtin_ctzll(bitboard);
            score += queenValue + queen_pref[pieceSquareIndex(true, square)];
            bitboard &= bitboard - 1;
        }
        bitboard = blackQueen;
        while (bitboard) {
            int square = __builtin_ctzll(bitboard);
            score -= queenValue + queen_pref[pieceSquareIndex(false, square)];
            bitboard &= bitboard - 1;
        }

        // Kings
        bitboard = whiteKing;
        while (bitboard) {
            int square = __builtin_ctzll(bitboard);
            score += king_pref[pieceSquareIndex(true, square)];
            bitboard &= bitboard - 1;
        }
        bitboard = blackKing;
        while (bitboard) {
            int square = __builtin_ctzll(bitboard);
            score -= king_pref[pieceSquareIndex(false, square)];
            bitboard &= bitboard - 1;
        }
        uint64_t attacked = getAttackedSquares(false);
        if(attacked & whiteKing) score -= 1000000;
        attacked = getAttackedSquares(true);
        if(attacked & blackKing) score += 1000000;
        return score;
    }


    // rewrite this

    int search(int depth, bool isWhite) {
        if (depth == 0) {
            tested++;
            return testEvaluate();
        }

        moveList.clear();
        generateMoves(isWhite);

        if (moveList.empty()) {
            return isWhite ? -99999 : 99999;  // No moves available
        }

        int bestScore = isWhite ? -100000 : 100000;
        vector<uint32_t> currentMoves = moveList;  // Save current moves

        for (auto mv : currentMoves) {
            // Save board state
            uint64_t oldWhitePawn = whitePawn;
            uint64_t oldWhiteKnight = whiteKnight;
            uint64_t oldWhiteBishop = whiteBishop;
            uint64_t oldWhiteRook = whiteRook;
            uint64_t oldWhiteQueen = whiteQueen;
            uint64_t oldWhiteKing = whiteKing;
            uint64_t oldBlackPawn = blackPawn;
            uint64_t oldBlackKnight = blackKnight;
            uint64_t oldBlackBishop = blackBishop;
            uint64_t oldBlackRook = blackRook;
            uint64_t oldBlackQueen = blackQueen;
            uint64_t oldBlackKing = blackKing;
            uint64_t oldWhitePieces = whitePieces;
            uint64_t oldBlackPieces = blackPieces;

            decodeMove(mv);
            makeMove();
            
            int score = search(depth - 1, !isWhite);

            // Restore board state
            whitePawn = oldWhitePawn;
            whiteKnight = oldWhiteKnight;
            whiteBishop = oldWhiteBishop;
            whiteRook = oldWhiteRook;
            whiteQueen = oldWhiteQueen;
            whiteKing = oldWhiteKing;
            blackPawn = oldBlackPawn;
            blackKnight = oldBlackKnight;
            blackBishop = oldBlackBishop;
            blackRook = oldBlackRook;
            blackQueen = oldBlackQueen;
            blackKing = oldBlackKing;
            whitePieces = oldWhitePieces;
            blackPieces = oldBlackPieces;

            if (isWhite) {
                if (score > bestScore) {
                    bestScore = score;
                    if (depth == initialDepth) bestMove = mv;
                }
            } else {
                if (score < bestScore) {
                    bestScore = score;
                    if (depth == initialDepth) bestMove = mv;
                }
            }
        }

        moveList = currentMoves;  
        return bestScore;
    }

    void testSearch() {
        moveList.clear();
        initialDepth = 1;
        int score = search(initialDepth, false);
        cout << "Search completed\n";
        cout << "Best move score: " << score << "\n";
        if (bestMove != 0) {
            decodeMove(bestMove);
            cout << "Best move: " << indexToAlgebraic(start) << "->" << indexToAlgebraic(end) << "\n";
        } else {
            cout << "No valid move found\n";
        }
        cout << "Positions evaluated: " << tested << "\n";
    }


};

int main() {
    Board b;
    cout << "\n";
    // b.setupGame("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
    // b.setupGame("8/5k2/3p4/1p1Pp2p/pP2Pp1P/P4P1K/8/8");
    // b.setupGame("6r/8/8/8/8/8/7K/6q");
    // b.setupGame("r3RB1n/p5K/3Q/1k/3pP3/8/1P3P1q/8");
    // b.setupGame("r3kb1r/2p1pppp/p4nb1/3p2B1/1q4P1/2NP1N1P/PPP1QP2/R3K2R");
    // b.setupGame("2p5/8/8/8/8/2P5/3B4/3K4");
    // b.setupGame("r3kb1r/ppp2p1p/5p2/3qp3/3N4/7P/PPPPKPP1/R1B4R");
    b.setupGame("8/8/8/8/8/1q/8/5K");
    // b.generateMoves();
    b.printGame();
    // b.test();
    // cout << b.evaluateBoard() << "\n"; 
    // cout << b.testEvaluate() << "\n";
    // b.printGame();
    // b.testSearch();

    return 0;
}
