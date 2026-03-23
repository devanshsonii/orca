#include "transposition.h"
#include <vector> 
using namespace std;

TTEntry* Transposition::probe(uint64_t search) {
    size_t idx = search & (n-1);
    TTEntry* entry = &entries[idx];
    if (entry->key == search) {
        return entry;
    }
    return nullptr;
}

void Transposition::store(uint64_t hash, int score, int depth, Move &mv, Flags flag) {
    size_t idx = hash & (n-1);
    entries[idx] = {hash, score, depth, mv, flag};
}