#include <iostream> 
#include <vector>
using namespace std;

const uint64_t A_FILE = 0x0101010101010101ULL; // File A mask
const uint64_t B_FILE = 0x0202020202020202ULL; // File B mask
const uint64_t C_FILE = 0x0404040404040404ULL; // File C mask
const uint64_t D_FILE = 0x0808080808080808ULL;
const uint64_t E_FILE = 0x1010101010101010ULL;
const uint64_t F_FILE = 0x2020202020202020ULL;
const uint64_t G_FILE = 0x4040404040404040ULL;
const uint64_t H_FILE = 0x8080808080808080ULL;

const uint64_t RANK_1 = 0x00000000000000FF;
const uint64_t RANK_2 = 0x000000000000FF00;
const uint64_t RANK_3 = 0x0000000000FF0000;
const uint64_t RANK_4 = 0x00000000FF000000;
const uint64_t RANK_5 = 0x000000FF00000000;
const uint64_t RANK_6 = 0x0000FF0000000000;
const uint64_t RANK_7 = 0x00FF000000000000;
const uint64_t RANK_8 = 0xFF00000000000000;



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
    
    int white_knight[64] = {
        -50,-40,-30,-30,-30,-30,-40,-50,
        -40,-20,  0,  0,  0,  0,-20,-40,
        -30,  0, 10, 15, 15, 10,  0,-30,
        -30,  5, 15, 20, 20, 15,  5,-30,
        -30,  0, 15, 20, 20, 15,  0,-30,
        -30,  5, 10, 15, 15, 10,  5,-30,
        -40,-20,  0,  5,  5,  0,-20,-40,
        -50,-40,-30,-30,-30,-30,-40,-50
    };

    int white_pawn[64] = {
        0,  0,  0,  0,  0,  0,  0,  0,
        50, 50, 50, 50, 50, 50, 50, 50,
        10, 10, 20, 30, 30, 20, 10, 10,
        5,  5, 10, 25, 25, 10,  5,  5,
        0,  0,  0, 20, 20,  0,  0,  0,
        5, -5,-10,  0,  0,-10, -5,  5,
        5, 10, 10,-20,-20, 10, 10,  5,
        0,  0,  0,  0,  0,  0,  0,  0
    };

    int white_bishop[64] = {
        -20,-10,-10,-10,-10,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5, 10, 10,  5,  0,-10,
        -10,  5,  5, 10, 10,  5,  5,-10,
        -10,  0, 10, 10, 10, 10,  0,-10,
        -10, 10, 10, 10, 10, 10, 10,-10,
        -10,  5,  0,  0,  0,  0,  5,-10,
        -20,-10,-10,-10,-10,-10,-10,-20
    };

    int white_rook[64] = {
        0,  0,  0,  0,  0,  0,  0,  0,
        5, 10, 10, 10, 10, 10, 10,  5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        0,  0,  0,  5,  5,  0,  0,  0
    };
    
    int white_queen[64] = {
        -20,-10,-10, -5, -5,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5,  5,  5,  5,  0,-10,
        -5,  0,  5,  5,  5,  5,  0, -5,
        0,  0,  5,  5,  5,  5,  0, -5,
        -10,  5,  5,  5,  5,  5,  0,-10,
        -10,  0,  5,  0,  0,  0,  0,-10,
        -20,-10,-10, -5, -5,-10,-10,-20
    };
    
    int white_king[64] = {
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
    uint64_t whiteLegalMoves = 0;
    uint64_t blackLegalMoves = 0;

    string indexToAlgebraic(int index) {
        if(index < 0 || index > 63) return "??";
        char file = 'a' + (index % 8);
        char rank = '1' + (index / 8);
        return string(1, file) + string(1, rank);
    }



public:
    vector<uint32_t> moveList;

    void initializeGame() {
        blackQueen = (1LL << 3);
        whiteQueen = (1LL << 59);
        
        blackKing = (1LL << 4);
        whiteKing = (1LL << 60);

        blackKnight = (1LL << 1) | (1LL << 6);
        whiteKnight = (1LL << 57) | (1LL << 62);
        
        blackRook = (1LL << 0) | (1LL << 7);
        whiteRook = (1LL << 56) | (1LL << 63);

        blackBishop = (1LL << 2) | (1LL << 5);
        whiteBishop = (1LL << 58) | (1LL << 61);

        for(int i = 8; i <= 15; i++){
            blackPawn |= (1LL << i);
        }
        for(int i = 48; i <= 55; i++){
            whitePawn |= (1LL << i);
        }
        whitePieces = whiteBishop | whiteKing | whitePawn | whiteKnight | whiteQueen | whiteRook;
        blackPieces = blackBishop | blackKing | blackPawn | blackKnight | blackQueen | blackRook;

    }
    
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

                // if((whiteLegalMoves >> mask) & 1) {
                //     piece = "_";
                // };
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
                // Skip the specified number of empty squares
                j += (c - '0') - 1;
            }
            j++;
        }
        whitePieces = whiteBishop | whiteKing | whitePawn | whiteKnight | whiteQueen | whiteRook;
        blackPieces = blackBishop | blackKing | blackPawn | blackKnight | blackQueen | blackRook;
    }

    int evaluateBoard(){
        int score = 0;
        for(int file = 0; file < 8; file++){
            for(int rank = 0; rank < 8; rank++){
                int mask = file*8 + rank;
                if((blackQueen >> mask) & 1) score -= 9 + white_queen[63-mask];
                else if((blackPawn >> mask) & 1) score -= 1 + white_pawn[63-mask];
                else if((blackBishop >> mask) & 1) score -= 3 + white_bishop[63-mask];
                else if((blackKing >> mask) & 1) score -= white_king[63-mask];
                else if((blackRook >> mask) & 1) score -= 5 + white_rook[63-mask];
                else if((blackKnight >> mask) & 1) score -= 3 + white_knight[63-mask];

                else if((whiteQueen >> mask) & 1) score += 9 + white_queen[mask];
                else if((whitePawn >> mask) & 1) score += 1 + white_pawn[mask];
                else if((whiteBishop >> mask) & 1) score += 3 + white_bishop[mask];
                else if((whiteKing >> mask) & 1) score += white_king[mask];
                else if((whiteRook >> mask) & 1) score += 5 + white_rook[mask];
                else if((whiteKnight >> mask) & 1) score += 3 + white_knight[mask];

                // if(whiteKing & blackLegalMoves) score -= 1000;
                // if(blackKing & whiteLegalMoves) score += 1000;
            }
        }
        // cout << score << "\n";
        return score;
    }

    void printGeneratedMoves(uint64_t moveList){
            for(int i = 1; i <= 64; i++){
                cout << (((moveList) & (1LL << (i-1))) != 0) << " ";
                if(i % 8 == 0) cout << "\n";
            }
            cout << "\n";
    }
    
    uint64_t getAttackedSquares(bool byWhite) {
        uint64_t attacked = 0;
        for(int piece = 0; piece < 5; piece++) {
            attacked |= generatePieceMoves(byWhite, piece);
        }
        return attacked;
    }

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

    void generateMoves() {
        for(int piece = 0; piece < 6; piece++) {
            whiteLegalMoves |= generatePieceMoves(true, piece);
            blackLegalMoves |= generatePieceMoves(false, piece);
        }
        // printGeneratedMoves((turn) ? whiteLegalMoves : blackLegalMoves);
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
    
    void getAllMoves(){
        for(auto it: moveList) {
            decodeMove(it);
        }
    }

    int makeMove() {
        int temp;
        uint64_t oldPiece;
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
        oldPiece = *currentPiece;

        *currentPiece &= ~(1ULL << start);
        *currentPiece |= (1ULL << end);

        whitePieces = whitePawn | whiteRook | whiteBishop | whiteKnight | whiteQueen | whiteKing;
        blackPieces = blackPawn | blackRook | blackBishop | blackKnight | blackQueen | blackKing;

        temp = evaluateBoard();
        printGame();

        *currentPiece = oldPiece;

        return temp;

    }
    
    void test() {
        int mx = -100000;
        uint32_t bestMv = 0;
        for(auto it: moveList) {

            decodeMove(it);
            if(color) {
                int temp = makeMove();
                if(temp > mx) {
                    bestMv = it;
                    mx = temp;
                }
            }
        }
        printGame();
        decodeMove(bestMv);
        cout << color << " " << piece << " " << start << " " << end << " " << mx << "\n";
    }
    
};

int main() {
    Board b;
    // b.initializeGame();
    // b.setupGame("8/5k2/3p4/1p1Pp2p/pP2Pp1P/P4P1K/8/8");
    b.setupGame("5R/7K/8/8/8/8/8/5q");
    // b.setupGame("2p5/8/8/8/8/2P5/3B4/3K4");
    // b.setupGame("r3kb1r/ppp2p1p/5p2/3qp3/3N4/7P/PPPPKPP1/R1B4R");
    // b.setupGame("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
    b.generateMoves();
    b.printGame();
    b.evaluateBoard();
    b.test();
    // b.printGame();

    return 0;
}