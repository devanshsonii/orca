#include "Board.h"
using namespace std;


int main() {
    Board b;
    b.setupGameFromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    // b.setupGameFromFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ");
    // b.setupGameFromFEN("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
    
    // Move moves[256];
    // int count = 0;
    // b.printGame();
    // b.generatePieceMoves(true, moves, count);
    // for (int i = 0; i < count; i++)
    // {
    //     cout << moves[i].startSquare << " " << moves[i].endSquare << "\n";
    // }
    
    string move;
    while(true) {
        b.printGame();
        cout << "Enter move: ";
        cin >> move;
        if(!b.play(move)) {
            cout << "Invalid Move\n";
        }
    }
    
    // int maxDepth = 5;
    // for(int depth = 1; depth <= maxDepth; depth++){
    //     long totalNodes = b.perft2(depth, true);
    //     cout << totalNodes << "\n";
    // }

    return 0;
}
