#ifndef CONSTANTS_H
#define CONSTANTS_H

const uint64_t A_FILE = 0x0101010101010101;
const uint64_t B_FILE = 0x0202020202020202;
const uint64_t C_FILE = 0x0404040404040404;
const uint64_t D_FILE = 0x0808080808080808;
const uint64_t E_FILE = 0x1010101010101010;
const uint64_t F_FILE = 0x2020202020202020;
const uint64_t G_FILE = 0x4040404040404040;
const uint64_t H_FILE = 0x8080808080808080;

const uint64_t RANK_8 = 0x00000000000000FF;
const uint64_t RANK_7 = 0x000000000000FF00;
const uint64_t RANK_6 = 0x0000000000FF0000;
const uint64_t RANK_5 = 0x00000000FF000000;
const uint64_t RANK_4 = 0x000000FF00000000;
const uint64_t RANK_3 = 0x0000FF0000000000;
const uint64_t RANK_2 = 0x00FF000000000000;
const uint64_t RANK_1 = 0xFF00000000000000;

const int DIRECTIONS_STRAIGHT[4] = {8, -8, 1, -1}; // down, up, right, left
const int DIRECTIONS_DIAGONAL[4] = {9, -9, 7, -7}; // bottom-right, top-left, bottom-left, top-right
const int DIRECTIONS_KING[8] = {8, 9, 1, -7, -8, -9, -1, 7}; // N, NE, E, SE, S, SW, W, NW

// const int PIECE_NONE = 0;
// const int PIECE_PAWN = 1;
// const int PIECE_ROOK = 2;
// const int PIECE_BISHOP = 3;
// const int PIECE_QUEEN = 4;
// const int PIECE_KNIGHT = 5;
// const int PIECE_KING = 6;

// const uint32_t COLOR_MASK          = (1u << 31);
// const uint32_t PIECE_MASK          = (7u << 28);
// const uint32_t START_SQUARE_MASK   = (0xFFu << 20);
// const uint32_t END_SQUARE_MASK     = (0xFFu << 12);
// const uint32_t CAPTURED_PIECE_MASK = (7u << 9);
// const uint32_t PROMOTION_MASK      = (7u << 6);
// const uint32_t RESERVED_MASK       = (0x3Fu);

// static constexpr uint32_t create_move(
//     bool color, int piece, int start, int end,
//     int captured, int promotion)
// {
//     return (uint32_t(color) << 31)
//          | (uint32_t(piece)     << 28)
//          | (uint32_t(start)     << 20)
//          | (uint32_t(end)       << 12)
//          | (uint32_t(captured)  << 9)
//          | (uint32_t(promotion) << 6);
// }


const int PIECE_PAWN = 0;
const int PIECE_ROOK = 1;
const int PIECE_BISHOP = 2; 
const int PIECE_QUEEN = 3;
const int PIECE_KNIGHT = 4;
const int PIECE_KING = 5;
const int PIECE_NONE = 6;

const uint8_t COLOR_WHITE = 0x8;
const uint8_t COLOR_BLACK = 0;
const uint8_t PIECE_MASK = 0x7;
const uint8_t COLOR_MASK = 0x8;

#endif