#include "search/transposition_table.h"
#include <cstring>

namespace Cerberus {

// Transposition table constructor
TranspositionTable::TranspositionTable(size_t size_mb) : table(nullptr), size(0), mask(0), age(0) {
    resize(size_mb);
}

// Transposition table destructor
TranspositionTable::~TranspositionTable() {
    delete[] table;
}

// Clear transposition table
void TranspositionTable::clear() {
    if (table) {
        std::memset(table, 0, size * sizeof(TTEntry));
    }
    age = 0;
}

// Resize transposition table
void TranspositionTable::resize(size_t size_mb) {
    delete[] table;
    
    // Calculate number of entries (size_mb * 1024 * 1024 / sizeof(TTEntry))
    size = (size_mb * 1024 * 1024) / sizeof(TTEntry);
    
    // Round down to power of 2
    size_t power = 1;
    while (power * 2 <= size) {
        power *= 2;
    }
    size = power;
    
    // Calculate mask for indexing
    mask = size - 1;
    
    // Allocate table
    table = new TTEntry[size];
    clear();
}

// Probe transposition table
bool TranspositionTable::probe(uint64_t key, TTEntry& entry) {
    if (!table) return false;
    
    size_t idx = index(key);
    TTEntry& tt_entry = table[idx];
    
    if (tt_entry.key == key) {
        entry = tt_entry;
        return true;
    }
    
    return false;
}

// Store entry in transposition table
void TranspositionTable::store(uint64_t key, Move best_move, Score score, int depth, TTFlag flag) {
    if (!table) return;
    
    size_t idx = index(key);
    TTEntry& entry = table[idx];
    
    // Replace if empty, better depth, or older entry
    if (entry.key == 0 || depth >= entry.depth || age > entry.age) {
        entry.key = key;
        entry.best_move = best_move;
        entry.score = score;
        entry.depth = depth;
        entry.flag = flag;
        entry.age = age;
    }
}

} // namespace Cerberus 