#include "Board.h"
#include <sstream>

using namespace std;

class UCI {
private:
    bool isWhiteTurn;
    void applyMove(Board &board, const string& moveStr);
public:
    void uciLoop();
    
};