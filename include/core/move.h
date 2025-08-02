#pragma once
#include "types.h"
#include <string>
#include <vector>

namespace Cerberus {

class Position;
class MoveGenerator;

// Move parsing and string conversion
namespace MoveUtils {
    Move parse_move(const std::string& str);
    std::string to_string(Move move);
    std::string to_string(Move move, bool chess960);
}

// Forward declarations
struct SearchInfo;

} // namespace Cerberus
