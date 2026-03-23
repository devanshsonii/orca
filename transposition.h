#ifndef TRANSPOSITION_H
#define TRANSPOSITION_H
#include <cstdint>
#include <vector>
#include "move.h"

enum Flags {
    EXACT,
    ALPHA,
    BETA
};

struct TTEntry {
    uint64_t key;
    int score;
    int depth;
    Move mv;
    Flags flag;
};

class Transposition {
private:
    size_t n = 1 << 20;

public:
    std::vector<TTEntry> entries;
    Transposition() {
        entries.resize(n, {0, 0, 0, {}, EXACT});
    };
    TTEntry* probe(uint64_t search);
    void store(uint64_t hash, int score, int depth, Move &mv, Flags flag);
};

#endif