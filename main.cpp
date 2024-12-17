#include <iostream> 
using namespace std;

const uint64_t A_FILE = 0x0101010101010101LL;
const uint64_t B_FILE = 0x0202020202020202LL;
const uint64_t C_FILE = 0x0303030303030303LL;
const uint64_t D_FILE = 0x0404040404040404LL;
const uint64_t E_FILE = 0x0505050505050505LL;
const uint64_t F_FILE = 0x0606060606060606LL;
const uint64_t G_FILE = 0x0707070707070707LL;
const uint64_t H_FILE = 0x8080808080808080LL;

const uint64_t RANK_1 = 0x00000000000000FFLL;
const uint64_t RANK_2 = 0x000000000000FF00LL;
const uint64_t RANK_3 = 0x0000000000FF0000LL;
const uint64_t RANK_4 = 0x00000000FF000000LL;
const uint64_t RANK_5 = 0x000000FF00000000LL;
const uint64_t RANK_6 = 0x0000FF0000000000LL;
const uint64_t RANK_7 = 0x00FF000000000000LL;
const uint64_t RANK_8 = 0xFF00000000000000LL;



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


public:

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
                char piece = '.';
                if((blackQueen >> mask) & 1) piece = 'q';
                else if((blackPawn >> mask) & 1) piece = 'p';
                else if((blackBishop >> mask) & 1) piece = 'b';
                else if((blackKing >> mask) & 1) piece = 'k';
                else if((blackRook >> mask) & 1) piece = 'r';
                else if((blackKnight >> mask) & 1) piece = 'n';

                else if((whiteQueen >> mask) & 1) piece = 'Q';
                else if((whitePawn >> mask) & 1) piece = 'P';
                else if((whiteBishop >> mask) & 1) piece = 'B';
                else if((whiteKing >> mask) & 1) piece = 'K';
                else if((whiteRook >> mask) & 1) piece = 'R';
                else if((whiteKnight >> mask) & 1) piece = 'N';
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
            else if(isdigit(c)) j += (c - '0' - 1);
            j++;
        }
        whitePieces = whiteBishop | whiteKing | whitePawn | whiteKnight | whiteQueen | whiteRook;
        blackPieces = blackBishop | blackKing | blackPawn | blackKnight | blackQueen | blackRook;

    }

    void evaluateBoard(){
        int score = 0;
        for(int file = 0; file < 8; file++){
            for(int rank = 0; rank < 8; rank++){
                int mask = file*8 + rank;
                if((blackQueen >> mask) & 1) score -= 9 + white_queen[64-mask];
                else if((blackPawn >> mask) & 1) score -= 1 + white_pawn[64-mask];
                else if((blackBishop >> mask) & 1) score -= 3 + white_bishop[64-mask];
                // else if((blackKing >> mask) & 1) score -= ;
                else if((blackRook >> mask) & 1) score -= 5 + white_rook[64-mask];
                else if((blackKnight >> mask) & 1) score -= 3 + white_knight[64-mask];

                else if((whiteQueen >> mask) & 1) score += 9 + white_queen[mask];
                else if((whitePawn >> mask) & 1) score += 1 + white_pawn[mask];
                else if((whiteBishop >> mask) & 1) score += 3 + white_bishop[mask];
                // else if((whiteKing >> mask) & 1) score += 0;
                else if((whiteRook >> mask) & 1) score += 5 + white_rook[mask];
                else if((whiteKnight >> mask) & 1) score += 3 + white_knight[mask];
            }
        }
        cout << score << "\n";
    }

    uint64_t generateMovesWhiteKing(){
        uint64_t temp = (
                    (whiteKing >> 8) | (whiteKing << 8) | // North, south
                    ((whiteKing << 1) & ~A_FILE) | // left
                    ((whiteKing >> 1) & ~H_FILE) | // right
                    ((whiteKing << 9) & ~A_FILE) | // north-east
                    ((whiteKing >> 7) & ~A_FILE) | // south-east
                    ((whiteKing << 7) & ~H_FILE) | // north-west
                    ((whiteKing >> 9) & ~H_FILE)  // south-west
                );
        temp &= ~whitePieces;
        return temp;
    }

    uint64_t generateMovesWhiteRook(){
        uint64_t movesWhiteRook = 0;
        uint64_t rook = whiteRook;
            while(rook){
                uint64_t singleRook = rook & -rook;
                uint64_t rookMoves = 0;
                uint64_t shift = singleRook;
                while(true) {
                    shift <<= 8; // north
                    if(shift == 0) break;
                    if(shift & whitePieces) break;
                    rookMoves |= shift;
                    if(shift & blackPieces) break;
                }
                shift = singleRook;
                while(true) {
                    shift >>= 8; // south
                    if(shift == 0) break;
                    if(shift & whitePieces) break;
                    rookMoves |= shift;
                    if(shift & blackPieces) break;
                }
                shift = singleRook;
                while(true) {
                    shift = (shift & ~H_FILE) << 1; // east 
                    if(shift == 0) break;
                    if(shift & whitePieces) break;
                    rookMoves |= shift;
                    if(shift & blackPieces) break;
                }  

                shift = singleRook;
                while(true) {
                    shift = (shift & ~A_FILE) >> 1; // west 
                    if(shift == 0) break;
                    if(shift & whitePieces) break;
                    rookMoves |= shift;
                    if(shift & blackPieces) break;
                }  
                movesWhiteRook |= rookMoves;
                rook &= rook - 1;
            }
        return movesWhiteRook;
    }
    
    uint64_t generateMovesWhiteQueen(){
        uint64_t movesWhiteQueen = 0;
            
            uint64_t queen = whiteQueen;
            while(true){
                queen >>= 8; // north
                if(queen == 0) break;
                if(queen & whitePieces) break;
                movesWhiteQueen |= queen;
                if(queen & blackPieces) break;
            }
            queen = whiteQueen;
            while(true){
                queen <<= 8; // south
                if(queen == 0) break;
                if(queen & whitePieces) break;
                movesWhiteQueen |= queen;
                if(queen & blackPieces) break;
            }
            queen = whiteQueen;
            while(true){
                queen = (queen & ~A_FILE) >> 1; // west
                if(queen == 0) break;
                if(queen & whitePieces) break;
                movesWhiteQueen |= queen;
                if(queen & blackPieces) break;
            }
            queen = whiteQueen;
            while(true){
                queen = (queen & ~H_FILE) << 1; // east
                if(queen == 0) break;
                if(queen & whitePieces) break;
                movesWhiteQueen |= queen;
                if(queen & blackPieces) break;
            }
            // diagonals
            queen = whiteQueen;
            while(true){
                // queen <<= 7; // south-west
                queen = (queen & ~A_FILE) << 7;
                if(queen == 0) break;
                if(queen & whitePieces) break;
                movesWhiteQueen |= queen;
                if(queen & blackPieces) break;
            }
            queen = whiteQueen;
            while(true){
                // queen <<= 9; // south-east
                queen = (queen & ~H_FILE) << 9;
                if(queen == 0) break;
                if(queen & whitePieces) break;
                movesWhiteQueen |= queen;
                if(queen & blackPieces) break;
            }
            queen = whiteQueen;
            while(true){
                // queen >>= 7; // north-east
                queen = (queen & ~H_FILE) >> 7;
                if(queen == 0) break;
                if(queen & whitePieces) break;
                movesWhiteQueen |= queen;
                if(queen & blackPieces) break;
            }
            queen = whiteQueen;
            while(true){
                queen = (queen & ~A_FILE) >> 9; // north-west
                if(queen == 0) break;
                if(queen & whitePieces) break;
                movesWhiteQueen |= queen;
                if(queen & blackPieces) break;
            }
        return movesWhiteQueen;
    }

    uint64_t generateMovesWhiteBishop(){
        uint64_t movesWhiteBishop = 0;
            uint64_t bishop = 0;

            bishop = whiteBishop;
            while(true){
                // bishop <<= 7; // south-west
                bishop = (bishop & ~A_FILE) << 7;
                if(bishop == 0) break;
                if(bishop & whitePieces) break;
                movesWhiteBishop |= bishop;
                if(bishop & blackPieces) break;
            }
            bishop = whiteBishop;
            while(true){
                // bishop <<= 9; // south-east
                bishop = (bishop & ~H_FILE) << 9;
                if(bishop == 0) break;
                if(bishop & whitePieces) break;
                movesWhiteBishop |= bishop;
                if(bishop & blackPieces) break;
            }
            bishop = whiteBishop;
            while(true){
                // bishop >>= 7; // north-east
                bishop = (bishop & ~H_FILE) >> 7;
                if(bishop == 0) break;
                if(bishop & whitePieces) break;
                movesWhiteBishop |= bishop;
                if(bishop & blackPieces) break;
            }
            bishop = whiteBishop;
            while(true){
                bishop = (bishop & ~A_FILE) >> 9; // north-west
                if(bishop == 0) break;
                if(bishop & whitePieces) break;
                movesWhiteBishop |= bishop;
                if(bishop & blackPieces) break;
            }
        return movesWhiteBishop;
    }

    uint64_t generateMovesWhiteKnight() {
        uint64_t moves = 0;
        uint64_t knights = whiteKnight;
        
        while(knights) {
            uint64_t knight = knights & -knights;
            uint64_t possible = 0;
            
           
            possible |= (knight << 17) & ~A_FILE;        // North-North-East
            possible |= (knight << 15) & ~H_FILE;        // North-North-West
            possible |= (knight << 10) & ~(A_FILE | B_FILE);  // North-East-East
            possible |= (knight << 6) & ~(G_FILE | H_FILE);   // North-West-West
            possible |= (knight >> 6) & ~(A_FILE | B_FILE);   // South-East-East
            possible |= (knight >> 10) & ~(G_FILE | H_FILE);  // South-West-West
            possible |= (knight >> 15) & ~A_FILE;        // South-South-East
            possible |= (knight >> 17) & ~H_FILE;        // South-South-West
            
            possible &= ~whitePieces;
            
            moves |= possible;
            knights &= knights - 1; 
        }
        return moves;
    }

    uint64_t generateMovesWhitePawn() {
        uint64_t moves = 0;
        uint64_t pawns = whitePawn;
        
        uint64_t singlePush = (pawns << 8) & ~(whitePieces | blackPieces);
        
        uint64_t doublePush = ((singlePush & RANK_3) << 8) & ~(whitePieces | blackPieces);
        
        uint64_t leftCapture = (pawns << 7) & blackPieces & ~H_FILE;
        uint64_t rightCapture = (pawns << 9) & blackPieces & ~A_FILE;
        
        moves = singlePush | doublePush | leftCapture | rightCapture;
        return moves;
    }

    void printGeneratedMoves(uint64_t moveList){
            for(int i = 1; i <= 64; i++){
                cout << (((moveList) & (1LL << (i-1))) != 0) << " ";
                if(i % 8 == 0) cout << "\n";
            }
            cout << "\n";
    }

    void generateMoves(){
        if(turn){ // generate for white
            // psuedo-moves for white king
            uint64_t movesWhiteKing = generateMovesWhiteKing();
            uint64_t movesWhiteRook = generateMovesWhiteRook();
            uint64_t movesWhiteQueen = generateMovesWhiteQueen();
            uint64_t movesWhiteBishop = generateMovesWhiteBishop();
            uint64_t movesWhiteKnight = generateMovesWhiteKnight();
            uint64_t movesWhitePawn = generateMovesWhitePawn();

        } else { // generate for black

        }
    }

    void move(uint32_t mv){
        /*
            last bit -> Black or white
            next 3 bits -> piece info
            next 6 bits -> start square
            next 6 bits -> end square
        */
        decodeMove(mv);
        if(color) { // move white
            switch (piece){
                case 0:
                    // move pawn
                    whitePawn &= ~(1LL << start);
                    whitePawn |= (1LL << end);
                    break;
                case 1:
                    // move rook
                    whiteRook &= ~(1LL << start);
                    whiteRook |= (1LL << end);
                    break;
                case 2:
                    // move knight
                    whiteKnight &= ~(1LL << start);
                    whiteKnight |= (1LL << end);
                    break;
                case 3:
                    // move bishop
                    whiteBishop &= ~(1LL << start);
                    whiteBishop |= (1LL << end);
                    break;
                case 4: 
                    // move queen
                    whiteQueen &= ~(1LL << start);
                    whiteQueen |= (1LL << end);
                    break;
                case 5:
                    // move king
                    whiteKing &= ~(1LL << start);
                    whiteKing |= (1LL << end);
                    break;
            }
        } else {
            switch (piece){
                case 0:
                    // move pawn
                    blackPawn &= ~(1LL << start);
                    blackPawn |= (1LL << end);
                    break;
                case 1:
                    // move rook
                    blackRook &= ~(1LL << start);
                    blackRook |= (1LL << end);
                    break;
                case 2:
                    // move knight
                    blackKnight &= ~(1LL << start);
                    blackKnight |= (1LL << end);
                    break;
                case 3:
                    // move bishop
                    blackBishop &= ~(1LL << start);
                    blackBishop |= (1LL << end);
                    break;
                case 4: 
                    // move queen
                    blackQueen &= ~(1LL << start);
                    blackQueen |= (1LL << end);
                    break;
                case 5:
                    // move king
                    blackKing &= ~(1LL << start);
                    blackKing |= (1LL << end);
                    break;
            }

        }
    }

    uint32_t encodeMove(int color, int piece, int start, int end) {
        uint32_t mv = 0;
        mv |= (end & 63);         
        mv <<= 6;                 
        mv |= (start & 63);       
        mv <<= 3;                 
        mv |= (piece & 7);        
        mv <<= 1;                 
        mv |= (color & 1);        
        return mv;
    }
    // uint32_t encodeMove(string mv){
    //     uint32_t mv = 0;

    // }
    void decodeMove(uint32_t mv) {
        color = mv & 1;           
        mv >>= 1;                 
        piece = mv & 7;           
        mv >>= 3;                 
        start = mv & 63;          
        mv >>= 6;                 
        end = mv & 63;            
        cout << "color: " << color << " piece: " << piece << " start: " << start << " end: " << end << "\n";
    }
};

int main() {
    Board b;
    // b.initializeGame();
    // b.setupGame("8/5k2/3p4/1p1Pp2p/pP2Pp1P/P4P1K/8/8");
    b.setupGame("8/8/4p1Q1/8/3B4/8/4R3/4K3");
    // b.setupGame("8/8/8/4K3/8/8/8/4Q3");
    // b.setupGame("r3kb1r/ppp2p1p/5p2/3qp3/3N4/7P/PPPPKPP1/R1B4R");
    b.generateMoves();
    b.printGame();
    b.evaluateBoard();
    // uint32_t mv = b.encodeMove(1, 0, 55, 39);
    // b.move(mv);
    // b.printGame();

    return 0;
}