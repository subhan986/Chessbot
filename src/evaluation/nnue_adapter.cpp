#include "evaluation/nnue_adapter.h"
#include "Stockfish-master/src/position.h"
#include "Stockfish-master/src/nnue/network.h"
#include "Stockfish-master/src/nnue/nnue_accumulator.h"
#include "Stockfish-master/src/nnue/nnue_feature_transformer.h"

#include <memory>

namespace Cerberus {
namespace NNUE_Adapter {

    // Global network object
    std::unique_ptr<Stockfish::Eval::NNUE::Networks> nnue_networks;

    // Function to convert Cerberus board to Stockfish position
    void convert_board_to_stockfish_position(const Board& cerberus_board, Stockfish::Position& sf_pos);

    bool init(const std::string& model_path) {
        try {
            Stockfish::Eval::NNUE::NetworkBig big_net(Stockfish::EvalFile::NNUE, Stockfish::Eval::NNUE::EmbeddedNNUEType::BIG);
            Stockfish::Eval::NNUE::NetworkSmall small_net(Stockfish::EvalFile::NNUE, Stockfish::Eval::NNUE::EmbeddedNNUEType::SMALL);

            big_net.load(model_path, "");
            small_net.load(model_path, "");

            nnue_networks = std::make_unique<Stockfish::Eval::NNUE::Networks>(std::move(big_net), std::move(small_net));
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Failed to load NNUE model: " << e.what() << std::endl;
            return false;
        }
    }

    Score evaluate(const Board& board) {
        if (!nnue_networks) {
            return 0; // Or some other default value
        }

        Stockfish::Position sf_pos;
        convert_board_to_stockfish_position(board, sf_pos);

        Stockfish::Eval::NNUE::AccumulatorStack accumulatorStack;
        Stockfish::Eval::NNUE::AccumulatorCaches::Cache<Stockfish::Eval::NNUE::NetworkBig::FTDimensions> cache;

        auto [score, _] = nnue_networks->big.evaluate(sf_pos, accumulatorStack, &cache);
        return static_cast<Score>(score);
    }

    void convert_board_to_stockfish_position(const Board& cerberus_board, Stockfish::Position& sf_pos) {
        Stockfish::StateInfo si;
        std::string fen = cerberus_board.get_fen();
        sf_pos.set(fen, false, &si);
    }

} // namespace NNUE_Adapter
} // namespace Cerberus
