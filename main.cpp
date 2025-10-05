#include "Board.h"
#include "uci.h"
#include <string>
#include <cstring>
using namespace std;


int main(int argc, char* argv[]) {
    // Board b;
    // b.setupGameFromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    // for(int i = 0; i < argc; i++) {
    //     if(strcmp(argv[i], "--perft") == 0) {
    //         if(i == argc-1) {
    //             cout << "--perft expects a numbers.\n";
    //             return 12;
    //         }
    //         int maxDepth = atoi(argv[i+1]);
    //         for(int depth = 1; depth <= maxDepth; depth++) {
    //             cout << b.perft2(depth, true) << "\n";
    //         }
    //     } else if(strcmp(argv[i], "--play") == 0) {
    //         string move;
    //         while(true) {
    //             b.printGame();
    //             cout << "Enter move: ";
    //             cin >> move;
    //             if(!b.play(move)) {
    //                 cout << "Invalid Move\n";
    //             }
    //         }
    //     } 
    // }
    UCI uci;
    uci.uciLoop();
    return 0;
}
