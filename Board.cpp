#include "Board.h"
#include "Constants.h"

int initialDepth = 7;

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
    
    index ++; 
    
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
    while(rooks && moveCount < 1024) {
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
                if(moveCount >= 1024) break;
            }
        }
    }

    uint64_t bishops = isWhite ? whiteBishop : blackBishop;
    while(bishops && moveCount < 1024) {
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
                if(moveCount >= 1024) break;
            }
        }
    }

    const int DIRECTIONS_QUEEN[8] = {8, -8, 1, -1, 9, -7, -9, 7}; 

    uint64_t queens = isWhite ? whiteQueen : blackQueen;
    while(queens && moveCount < 1024) {
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
                if(moveCount >= 1024) break;
            }
        }
    }
    // king 
    uint64_t kings = isWhite ? whiteKing : blackKing;
    while(kings && moveCount < 1024) {
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
            else {
                // Create a temporary move
                Move temp(isWhite, PIECE_KING, kingSquare, target, 
                          (allPieces[target] & PIECE_MASK), '-');
                
                make_move(temp);
                bool underCheck = isInCheck(isWhite);
                unmake_move(temp);
                
                if(!underCheck) {
                    moveArray[moveCount++] = temp;
                }
            }

            if(moveCount >= 1024) break;
        }
    }

    // Castling Test

    if(isWhite) {
        // White King on e1 => square 60
        if((whiteKing & (1ULL << 60)) && !isInCheck(true)) {
            if(whiteCanCastleKingSide &&
               ((allPieces[61] & PIECE_MASK) == PIECE_NONE) && ((allPieces[62] & PIECE_MASK) == PIECE_NONE) && 
               !isSquareAttacked(61, false) && !isSquareAttacked(62, false)) {
                // Debug
                // cout << "Adding white king-side castle\n";
                moveArray[moveCount++] = Move(true, PIECE_KING, 60, 62, PIECE_NONE, 'C');
            }
            if(whiteCanCastleQueenSide &&
               ((allPieces[59] & PIECE_MASK)==PIECE_NONE) && 
               ((allPieces[58] & PIECE_MASK) == PIECE_NONE) &&
               ((allPieces[57] & PIECE_MASK) == PIECE_NONE) && 
               !isSquareAttacked(59, false) && !isSquareAttacked(58, false)) {
                // Debug
                // cout << "Adding white queen-side castle\n";
                moveArray[moveCount++] = Move(true, PIECE_KING, 60, 58, PIECE_NONE, 'C');
            }
        }
    } else {
        // Black King on e8 => square 4
        if((blackKing & (1ULL << 4)) && !isInCheck(false)) {
            if(blackCanCastleKingSide &&
               ((allPieces[5] & PIECE_MASK) == PIECE_NONE) && ((allPieces[6] & PIECE_MASK) == PIECE_NONE) &&
               !isSquareAttacked(5, true) && !isSquareAttacked(6, true)) {
                // cout << "Adding black king-side castle\n";
                moveArray[moveCount++] = Move(false, PIECE_KING, 4, 6, PIECE_NONE, 'C');
            }
            if(blackCanCastleQueenSide &&
               ((allPieces[3] & PIECE_MASK) == PIECE_NONE) && 
               ((allPieces[2] & PIECE_MASK) == PIECE_NONE) &&
               ((allPieces[1] & PIECE_MASK) == PIECE_NONE) &&
               !isSquareAttacked(3, true) && !isSquareAttacked(2, true)) {
                // cout << "Adding black queen-side castle\n";
                moveArray[moveCount++] = Move(false, PIECE_KING, 4, 2, PIECE_NONE, 'C');
            }
        }
    }


    // pawn move gen

    uint64_t pawns = isWhite ? whitePawn : blackPawn;
    while(pawns && moveCount < 1024) {
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
    Move moves[1024];
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

    // castling

    if(mv.promotion == 'C' && mv.pieceType == PIECE_KING) {
        // White
        if(mv.isWhite) {
            // King side castling e1->g1
            if(mv.startSquare == 60 && mv.endSquare == 62) {
                whiteRook &= ~(1ULL << 63); // remove rook from h1
                whiteRook |=  (1ULL << 61); // place rook on f1
                whitePieces &= ~(1ULL << 63);
                whitePieces |=  (1ULL << 61);
                whiteCanCastleKingSide = false;
                whiteCanCastleQueenSide = false;
            }
            // Queen side castling e1->c1
            else if(mv.startSquare == 60 && mv.endSquare == 58) {
                whiteRook &= ~(1ULL << 56); // remove rook from a1
                whiteRook |=  (1ULL << 59); // place rook on d1
                whitePieces &= ~(1ULL << 56);
                whitePieces |=  (1ULL << 59);
                whiteCanCastleKingSide = false;
                whiteCanCastleQueenSide = false;
            }
        } 
        // Black
        else {
            // King side castling e8->g8
            if(mv.startSquare == 4 && mv.endSquare == 6) {
                blackRook &= ~(1ULL << 7); // remove rook from h8
                blackRook |=  (1ULL << 5); // place rook on f8
                blackPieces &= ~(1ULL << 7);
                blackPieces |=  (1ULL << 5);
                blackCanCastleKingSide = false;
                blackCanCastleQueenSide = false;
            }
            // Queen side castling e8->c8
            else if(mv.startSquare == 4 && mv.endSquare == 2) {
                blackRook &= ~(1ULL << 0); // remove rook from a8
                blackRook |=  (1ULL << 3); // place rook on d8
                blackPieces &= ~(1ULL << 0);
                blackPieces |=  (1ULL << 3);
                blackCanCastleKingSide = false;
                blackCanCastleQueenSide = false;
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

    // castling

    if(mv.promotion == 'C' && mv.pieceType == PIECE_KING) {
        // White
        if(mv.isWhite) {
            // King side g1->e1
            if(mv.startSquare == 60 && mv.endSquare == 62) {
                whiteRook &= ~(1ULL << 61);
                whiteRook |=  (1ULL << 63);
                whitePieces &= ~(1ULL << 61);
                whitePieces |=  (1ULL << 63);
            }
            // Queen side c1->e1
            else if(mv.startSquare == 60 && mv.endSquare == 58) {
                whiteRook &= ~(1ULL << 59);
                whiteRook |=  (1ULL << 56);
                whitePieces &= ~(1ULL << 59);
                whitePieces |=  (1ULL << 56);
            }
        } 
        // Black
        else {
            // King side g8->e8
            if(mv.startSquare == 4 && mv.endSquare == 6) {
                blackRook &= ~(1ULL << 5);
                blackRook |=  (1ULL << 7);
                blackPieces &= ~(1ULL << 5);
                blackPieces |=  (1ULL << 7);
            }
            // Queen side c8->e8
            else if(mv.startSquare == 4 && mv.endSquare == 2) {
                blackRook &= ~(1ULL << 3);
                blackRook |=  (1ULL << 0);
                blackPieces &= ~(1ULL << 3);
                blackPieces |=  (1ULL << 0);
            }
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

    Move moveArray[1024];
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
    const int KNIGHT_VALUE = 320;
    const int BISHOP_VALUE = 330;
    const int ROOK_VALUE = 500;
    const int QUEEN_VALUE = 900;

    int score = 0;
    score += __builtin_popcountll(whitePawn) * PAWN_VALUE;
    score += __builtin_popcountll(whiteBishop) * BISHOP_VALUE;
    score += __builtin_popcountll(whiteKnight) * KNIGHT_VALUE;
    score += __builtin_popcountll(whiteRook) * ROOK_VALUE;
    score += __builtin_popcountll(whiteQueen) * QUEEN_VALUE;
    score -= __builtin_popcountll(blackPawn) * PAWN_VALUE;
    score -= __builtin_popcountll(blackBishop) * BISHOP_VALUE;
    score -= __builtin_popcountll(blackKnight) * KNIGHT_VALUE;
    score -= __builtin_popcountll(blackRook) * ROOK_VALUE;
    score -= __builtin_popcountll(blackQueen) * QUEEN_VALUE;

    // // new stuff added
    // for (int i = 0; i < 64; i++)
    // {
    //     if((allPieces[i] & PIECE_MASK) != PIECE_NONE) {

    //     }
    // }
    
    for (int i = 0; i < 64; i++)
    {
        uint8_t curr_piece = allPieces[i];
        int isWhite = (curr_piece & COLOR_MASK) == 8;
        int piece = (curr_piece & PIECE_MASK);
        if(piece == PIECE_NONE) continue;
        switch (piece) {
            case PIECE_PAWN:
                score += (isWhite ? pawn_pst[56^i] : -pawn_pst[i]);
                break;
            case PIECE_ROOK:
                score += (isWhite ? rook_pst[56^i] : -rook_pst[i]);
                break;
            case PIECE_BISHOP:
                score += (isWhite ? bishop_pst[56^i] : -bishop_pst[i]);
                break;
            case PIECE_KNIGHT:
                score += (isWhite ? knight_pst[56^i] : -knight_pst[i]);
                break;
            case PIECE_QUEEN:
                score += (isWhite ? queen_pst[56^i] : -queen_pst[i]);
                break;
            case PIECE_KING:
                score += (isWhite ? king_pst[56^i] : -king_pst[i]);
                break;
        }
    }

    if(isInCheck(true)) score -= 50;
    if(isInCheck(false)) score += 50;
    // // checkmates
    // Move moves[1024];
    // int moveCount = 0;
    // // white checkmated
    // generatePieceMoves(true, moves, moveCount);
    // if(moveCount == 0) score -= 99999;
    // moveCount = 0;
    // // black checkmated
    // generatePieceMoves(false, moves, moveCount);
    // if(moveCount == 0) score += 99999;

    return score;
}

int value(int piece) {
    switch (piece) {
        case PIECE_PAWN:
            return 100;
        case PIECE_KNIGHT:
            return 320;
        case PIECE_BISHOP:
            return 330;
        case PIECE_ROOK:
            return 500;
        case PIECE_QUEEN:
            return 900;
        default:
            return 0;
    }
}

int calculateScore(const Move &move) {
    return value(move.pieceCaptured) - value(move.pieceType);
}

bool compareMoves(const Move &a, const Move &b) {
    return calculateScore(a) > calculateScore(b);
}

void Board::order_moves(Move *moves, int moveCount) {
    sort(moves, moves+moveCount, compareMoves);
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
    
    Move moves[1024];
    int moveCount = 0;
    
    generatePieceMoves(isWhite, moves, moveCount);
    order_moves(moves, moveCount);
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
    if (byWhite) {
        int attack1 = square + 7;
        int attack2 = square + 9;

        if (attack1 >= 0 && (attack1 % 8 != 7)) { 
            if (pawns & (1ULL << attack1)) {
                return true;
            }
        }
        if (attack2 >= 0 && (attack2 % 8 != 0)) { 
            if (pawns & (1ULL << attack2)) {
                return true;
            }
        }
    } else {
        int attack1 = square - 7;
        int attack2 = square - 9;

        if (attack1 < 64 && (attack1 % 8 != 7)) {
            if (pawns & (1ULL << attack1)) {
                return true;
            }
        }
        if (attack2 < 64 && (attack2 % 8 != 0)) {
            if (pawns & (1ULL << attack2)) {
                return true;
            }
        }
    }

    // Knight attacks
    uint64_t knights = byWhite ? whiteKnight : blackKnight;
    uint64_t knightAttacks = knight_attacks[square];
    if (knights & knightAttacks) {
        return true;
    }

    // Bishop/Queen diagonal attacks
    uint64_t bishopsQueens = byWhite ? (whiteBishop | whiteQueen) : (blackBishop | blackQueen);
    for (int dir = 0; dir < 4; dir++) {
        int target = square;
        while (true) {
            target += DIRECTIONS_DIAGONAL[dir];
            if (target < 0 || target > 63) break;

            if ((DIRECTIONS_DIAGONAL[dir] == 9 || DIRECTIONS_DIAGONAL[dir] == -7) && (target % 8 == 0)) break;
            if ((DIRECTIONS_DIAGONAL[dir] == -9 || DIRECTIONS_DIAGONAL[dir] == 7) && ((target + 1) % 8 == 0)) break;

            uint64_t targetBit = 1ULL << target;
            if (bishopsQueens & targetBit) {
                return true;
            }
            if (allPieces[target] != PIECE_NONE) break;
        }
    }

    uint64_t rooksQueens = byWhite ? (whiteRook | whiteQueen) : (blackRook | blackQueen);
    for (int dir = 0; dir < 4; dir++) {
        int target = square;
        while (true) {
            target += DIRECTIONS_STRAIGHT[dir];
            if (target < 0 || target > 63) break;

            if (DIRECTIONS_STRAIGHT[dir] == 1 && (target % 8 == 0)) break;
            if (DIRECTIONS_STRAIGHT[dir] == -1 && ((target + 1) % 8 == 0)) break;

            uint64_t targetBit = 1ULL << target;
            if (rooksQueens & targetBit) {
                return true;
            }
            if (allPieces[target] != PIECE_NONE) break;
        }
    }

    uint64_t opponentKing = byWhite ? whiteKing : blackKing;
    for (int dir = 0; dir < 8; dir++) {
        int adjacentSquare = square + DIRECTIONS_KING[dir];
        if (adjacentSquare < 0 || adjacentSquare > 63) continue;

        if ((DIRECTIONS_KING[dir] == 1 || DIRECTIONS_KING[dir] == -7 || DIRECTIONS_KING[dir] == 9) && (square % 8 == 7)) continue;  
        if ((DIRECTIONS_KING[dir] == -1 || DIRECTIONS_KING[dir] == -9 || DIRECTIONS_KING[dir] == 7) && (square % 8 == 0)) continue;
        if ((DIRECTIONS_KING[dir] == -9 || DIRECTIONS_KING[dir] == -7 || DIRECTIONS_KING[dir] == -8) && (square < 8)) continue;
        if ((DIRECTIONS_KING[dir] == 9 || DIRECTIONS_KING[dir] == 8 || DIRECTIONS_KING[dir] == 7) && (square > 55)) continue;
        if (opponentKing & (1ULL << adjacentSquare)) {
            return true;
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


bool Board::play(string moveStr) {
    // Parse human move
    if(moveStr.length() < 4) return false;
    
    // Detect castling moves
    if(moveStr == "e1g1" || moveStr == "e1c1" || moveStr == "e8g8" || moveStr == "e8c8") {
        // Determine if White or Black is castling
        bool isWhiteCastling = (moveStr[1] == '1');
        bool isKingside = (moveStr[2] == 'g' || moveStr[2] == 'g'); // e1g1 or e8g8
        
        // Create castling move
        Move castlingMove;
        castlingMove.isWhite = isWhiteCastling;
        castlingMove.pieceType = PIECE_KING;
        castlingMove.startSquare = algebraicToIndex(moveStr.substr(0,2));
        castlingMove.endSquare = algebraicToIndex(moveStr.substr(2,2));
        castlingMove.promotion = 'C'; // Indicate castling
        
        // Validate and make the castling move
        if(isMoveLegal(castlingMove)) {
            // Make human castling move
            make_move(castlingMove);
            
            // Engine response
            alphaBeta(false, initialDepth, -100000, 100000);
            make_move(bestMove);
            printGame();
            
            return true;
        } else {
            return false;
        }
    }

    int startSquare = algebraicToIndex(moveStr.substr(0,2));
    int endSquare = algebraicToIndex(moveStr.substr(2,2));
    
    // Find and validate the move
    Move moves[1024];
    int moveCount = 0;
    generatePieceMoves(true, moves, moveCount);
    
    Move* selectedMove = nullptr;
    for(int i = 0; i < moveCount; i++) {
        if(moves[i].startSquare == startSquare && moves[i].endSquare == endSquare) {
            if(isMoveLegal(moves[i])) {
                selectedMove = &moves[i];
                break;
            }
        }
    }
    
    if(!selectedMove) return false;
    
    // Make human move
    make_move(*selectedMove);
    
    // Engine response
    alphaBeta(false, initialDepth, -100000, 100000);
    make_move(bestMove);
    printGame();

    return true;
}

Move Board::search(int depth) {
    int old_initialDepth = initialDepth;
    initialDepth = depth;

    alphaBeta(turn, depth, -100000, 100000);

    initialDepth = old_initialDepth;
    return bestMove;
}

int Board::algebraicToIndex(string square) {
    int file = square[0] - 'a';
    int rank = '8' - square[1];
    return rank * 8 + file;
}

int Board::getPieceAt(int square) {
    uint64_t mask = 1ULL << square;
    
    if (blackPawn & mask) return COLOR_BLACK | PIECE_PAWN;
    if (blackRook & mask) return COLOR_BLACK | PIECE_ROOK;
    if (blackBishop & mask) return COLOR_BLACK | PIECE_BISHOP;
    if (blackQueen & mask) return COLOR_BLACK | PIECE_QUEEN;
    if (blackKnight & mask) return COLOR_BLACK | PIECE_KNIGHT;
    if (blackKing & mask) return COLOR_BLACK | PIECE_KING;
    
    if (whitePawn & mask) return COLOR_WHITE | PIECE_PAWN;
    if (whiteRook & mask) return COLOR_WHITE | PIECE_ROOK;
    if (whiteBishop & mask) return COLOR_WHITE | PIECE_BISHOP;
    if (whiteQueen & mask) return COLOR_WHITE | PIECE_QUEEN;
    if (whiteKnight & mask) return COLOR_WHITE | PIECE_KNIGHT;
    if (whiteKing & mask) return COLOR_WHITE | PIECE_KING;
    return -1;

}


