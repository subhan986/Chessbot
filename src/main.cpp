#include "core/zobrist.h"
#include "uci/uci_interface.h"
#include "evaluation/nnue_adapter.h"
#include <iostream>
#include <exception>

int main() {
    try {
        // Initialize Zobrist hashing
        Cerberus::Zobrist::init();
        Cerberus::Bitboards::init();

        // Initialize NNUE
        if (!Cerberus::NNUE_Adapter::init("models/nn-62ef826d1a6d.nnue")) {
            std::cerr << "Failed to initialize NNUE" << std::endl;
            return 1;
        }
        
        // Create and run UCI interface
        Cerberus::UCIInterface uci;
        uci.loop();
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown fatal error occurred" << std::endl;
        return 1;
    }
}
