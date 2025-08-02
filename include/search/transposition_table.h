#pragma once

#include "../core/board.h"
#include "../core/move.h"
#include "../core/types.h"
#include <cstdint>

namespace Cerberus {

enum class TTFlag : uint8_t {
    NONE,
    EXACT,
    LOWER_BOUND,
    UPPER_BOUND
};

struct TTEntry {
    uint64_t key;
    Move best_move;
    Score score;
    int depth;
    TTFlag flag;
    int age;
};

class TranspositionTable {
private:
    TTEntry* table;
    size_t size;
    size_t mask;
    int age;

public:
    TranspositionTable(size_t size_mb = 16);
    ~TranspositionTable();
    
    void clear();
    void resize(size_t size_mb);
    
    bool probe(uint64_t key, TTEntry& entry);
    void store(uint64_t key, Move best_move, Score score, int depth, TTFlag flag);
    
    void set_age(int new_age) { age = new_age; }
    int get_age() const { return age; }
    
private:
    size_t index(uint64_t key) const { return key & mask; }
};

} // namespace Cerberus
