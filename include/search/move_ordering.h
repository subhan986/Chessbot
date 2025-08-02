#pragma once

#include "../core/board.h"
#include "../core/move.h"
#include "../core/types.h"
#include <vector>

namespace Cerberus {

class MovePicker {
public:
    MovePicker(const Board& b, Move tt_move, Move killer1, Move killer2, const int (*history)[64][64]);
    Move next_move();

private:
    void score_moves(Move tt_move, Move killer1, Move killer2);
    Score get_move_score(Move move, Move tt_move, Move killer1, Move killer2);

    const Board& board;
    std::vector<Move> moves;
    size_t current_index;
    const int (*history)[64][64];
};


} // namespace Cerberus
