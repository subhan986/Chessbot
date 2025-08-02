#pragma once

#include "search.h"
#include "../core/position.h"
#include <thread>

namespace Cerberus {

class Thread {
public:
    Thread() = default;
    ~Thread();

    void start_searching(const Position& pos, const SearchLimits& limits);
    void stop();

private:
    void search();

    std::thread thread;
    Position root_pos;
    SearchLimits search_limits;
    SearchInfo search_info;
    bool searching = false;
};

}
