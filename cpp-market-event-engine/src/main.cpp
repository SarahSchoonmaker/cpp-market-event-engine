#include "engine.hpp"
#include "parser.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

static void print_usage(const char* prog) {
    std::cerr
        << "Usage: " << prog << " --input <file> [--summary] [--alerts] [--price-jump-bps <bps>]\n"
        << "  --input            Path to events file\n"
        << "  --summary          Print summary after processing\n"
        << "  --alerts           Print alerts (if any)\n"
        << "  --price-jump-bps   Threshold in basis points for price jump alerts (default 50)\n";
}

int main(int argc, char** argv) {
    std::string input;
    mkt::EngineConfig cfg;

    for (int i = 1; i < argc; i++) {
        std::string a = argv[i];
        if (a == "--input" && i + 1 < argc) {
            input = argv[++i];
        } else if (a == "--summary") {
            cfg.emit_summary = true;
        } else if (a == "--alerts") {
            cfg.emit_alerts = true;
        } else if (a == "--price-jump-bps" && i + 1 < argc) {
            cfg.price_jump_bps = std::stoll(argv[++i]);
        } else if (a == "--help" || a == "-h") {
            print_usage(argv[0]);
            return 0;
        } else {
            std::cerr << "Unknown arg: " << a << "\n";
            print_usage(argv[0]);
            return 2;
        }
    }

    if (input.empty()) {
        print_usage(argv[0]);
        return 2;
    }

    std::ifstream fin(input);
    if (!fin) {
        std::cerr << "Failed to open input: " << input << "\n";
        return 1;
    }

    mkt::MarketEventEngine engine(cfg);
    mkt::EventParser parser(fin);

    while (auto ev = parser.next()) {
        const auto& e = *ev;

        // If parser had an error, it returns an Event with details and sets error_message().
        if (!parser.error_message().empty()) {
            // Count parse error deterministically by sending a minimal bad event to engine (engine counts it).
            mkt::Event bad;
            bad.line_no = parser.line_no();
            // Leave symbol empty and timestamp 0 to trigger parse_errors count.
            engine.process(bad);

            std::cerr << "[parse-error] line " << parser.line_no() << ": " << parser.error_message() << "\n";
            continue;
        }

        engine.process(e);
    }

    if (cfg.emit_alerts) {
        for (const auto& al : engine.alerts()) {
            std::cout << "ALERT " << al.timestamp << " " << al.symbol << " " << al.message << "\n";
        }
        if (!engine.alerts().empty()) std::cout << "\n";
    }

    if (cfg.emit_summary) {
        std::cout << engine.render_summary();
    }

    return 0;
}
