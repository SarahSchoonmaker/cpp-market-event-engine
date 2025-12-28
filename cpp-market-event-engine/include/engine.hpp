#pragma once
#include "event.hpp"
#include "state_store.hpp"
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace mkt {

struct Alert {
    std::int64_t timestamp;
    std::string symbol;
    std::string message;
};

struct EngineConfig {
    bool emit_summary = false;
    bool emit_alerts = false;
    // Price jump threshold in basis points (bps). 50 bps = 0.50%
    std::int64_t price_jump_bps = 50;
};

struct EngineStats {
    std::uint64_t total_events = 0;
    std::uint64_t parsed_events = 0;
    std::uint64_t parse_errors = 0;
    std::uint64_t unknown_events = 0;
};

class MarketEventEngine {
public:
    explicit MarketEventEngine(EngineConfig cfg);

    // Apply an event to the engine state; may generate alerts.
    void process(const Event& e);

    // Render deterministic outputs.
    std::string render_summary() const;

    const std::vector<Alert>& alerts() const { return alerts_; }
    const EngineStats& stats() const { return stats_; }
    const StateStore& state() const { return store_; }

private:
    EngineConfig cfg_;
    StateStore store_;
    EngineStats stats_;
    std::vector<Alert> alerts_;

    void maybe_emit_price_jump_alert(SymbolState& st, double prev_price, double new_price, std::int64_t ts);
    static double abs_percent_change(double a, double b);
};

} // namespace mkt
