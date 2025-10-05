#include "uci.h"

using namespace std;

bool isWhiteTurn = false;
void UCI::applyMove(Board& board, const string& moveStr) {

    int startSquare = board.algebraicToIndex(moveStr.substr(0, 2));
    int endSquare = board.algebraicToIndex(moveStr.substr(2, 2));

    int piece = board.getPieceAt(startSquare);
    int capturedPiece = board.getPieceAt(endSquare);
    int pieceType = piece & 7; 
    int capturedPieceType = (capturedPiece == 0) ? -1 : (capturedPiece & 7); 

    char promotion = '-';
    if (moveStr.length() > 4) {
        promotion = toupper(moveStr[4]);
    }
    
    Move move(isWhiteTurn, pieceType, startSquare, endSquare, capturedPieceType, promotion);
    board.make_move(move);
    isWhiteTurn = !isWhiteTurn;
}

void UCI::uciLoop() {
    Board board;
    string startpos_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    board.setupGameFromFEN(startpos_fen);

    string line;
    while (getline(cin, line)) {
        istringstream iss(line);
        string token;
        iss >> token;
        if (token == "uci") {
            cout << "id name Ocra" << endl;
            cout << "id author Devansh Soni" << endl;
            cout << "uciok" << endl;
        } else if (token == "isready") {
            cout << "readyok" << endl;
        } else if (token == "ucinewgame") {
            board.setupGameFromFEN(startpos_fen);
        } else if (token == "position") {
            string fen_string;
            iss >> token;
            if (token == "startpos") {
                fen_string = startpos_fen;
                board.setupGameFromFEN(fen_string);
            } 
            string moveStr;
            while (iss >> moveStr) {
                // cout << moveStr;
                applyMove(board, moveStr);
            }
        } else if (token == "go") {
            int depth = 6; 
            string go_token;
            while(iss >> go_token) {
                if (go_token == "depth") {
                    iss >> depth;
                }
            }
            Move bestMove = board.search(depth);
            cout << "bestmove " << board.indexToAlgebraic(bestMove.startSquare) 
                 << board.indexToAlgebraic(bestMove.endSquare) << endl;
            board.make_move(bestMove);
        } else if (token == "quit") {
            break;
        }
    }
}

