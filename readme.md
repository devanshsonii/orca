# Ocra - Chess Engine
A chess engine implemented from scratch using C++.

## Current Capabilities 
- Able to beat 1400 rated Stockfish (at least in terms of pieces)
- Depth-6 Perft in 7 seconds
- Alpha-Beta Search with Move Ordering

## Installation 

```shell
git clone https://github.com/devanshsonii/orca
cd orca/
make && ./chess_engine
```

## TODO
- Make the move generation faster and more robust
- Implementing Checkmates
- Opening moves book to introduce variety in the starting moves
- Implementing En Passant and Castling
