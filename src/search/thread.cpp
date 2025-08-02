#include "search/thread.h"

Thread::~Thread() {
    stop();
}

void Thread::start_searching(const Position& pos, const SearchLimits& limits) {
    stop();
    root_pos = pos;
    search_limits = limits;
    searching = true;
    thread = std::thread(&Thread::search, this);
}

void Thread::stop() {
    if (searching) {
        searching = false;
        if (thread.joinable()) {
            thread.join();
        }
    }
}

void Thread::search() {
    Search::iterative_deepening(root_pos, search_limits, &search_info);
}
