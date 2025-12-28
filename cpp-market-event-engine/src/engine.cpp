#include "engine.hpp"
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>

namespace mkt {

MarketEventEngine::MarketEventEngine(EngineConfig cfg) : cfg_(cfg) {}

double MarketEventEngine::abs_percent_change(double a, double b) {
    // percent change from a -> b
    if (a == 0.0) return 0.0;
    return std::abs((b - a) / a);
}

void MarketEventEngine::maybe_emit_price_jump_alert(SymbolState& st, double prev_price, double new_price, std::int64_t ts) {
    if (!cfg_.emit_alerts) return;
    if (prev_price <= 0.0) return;

    const double pct = abs_percent_change(prev_price, new_price);
    const double bps = pct * 10000.0; // 1% = 100 bps

    if (bps >= static_cast<double>(cfg_.price_jump_bps)) {
        std::ostringstream msg;
        msg << "Price jump: " << st.symbol
            << " " << std::fixed << std::setprecision(4)
            << (bps) << " bps ("
            << prev_price << " -> " << new_price << ")";
        alerts_.push_back(Alert{ts, st.symbol, msg.str()});
    }
}

void MarketEventEngine::process(const Event& e) {
    stats_.total_events++;

    // Parse errors are represented as Unknown with empty symbol or invalid fields.
    // We keep determinism by counting them here.
    if (e.symbol.empty() || e.timestamp == 0) {
        stats_.parse_errors++;
        return;
    }

    stats_.parsed_events++;

    auto& st = store_.get_or_create(e.symbol);

    // Deterministic: reject out-of-order timestamps per symbol (count as unknown)
    if (st.last_ts != 0 && e.timestamp < st.last_ts) {
        stats_.unknown_events++;
        return;
    }

    st.last_ts = e.timestamp;

    switch (e.type) {
        case EventType::PriceUpdate: {
            st.price_updates++;
            if (st.has_price) {
                maybe_emit_price_jump_alert(st, st.last_price, e.price, e.timestamp);
            }
            st.last_price = e.price;
            st.has_price = true;
            break;
        }
        case EventType::OrderNew:
            st.order_new++;
            break;
        case EventType::OrderFill:
            st.order_fill++;
            break;
        case EventType::OrderCancel:
            st.order_cancel++;
            break;
        default:
            stats_.unknown_events++;
            break;
    }
}

std::string MarketEventEngine::render_summary() const {
    std::ostringstream out;
    out << "SUMMARY\n";
    out << "-------\n";
    out << "Total lines/events seen: " << stats_.total_events << "\n";
    out << "Parsed events: " << stats_.parsed_events << "\n";
    out << "Parse errors: " << stats_.parse_errors << "\n";
    out << "Unknown/rejected events: " << stats_.unknown_events << "\n\n";

    // Deterministic output ordering (unordered_map -> stable order by symbol)
    std::vector<std::string> symbols;
    symbols.reserve(store_.all().size());
    for (const auto& kv : store_.all()) symbols.push_back(kv.first);
    std::sort(symbols.begin(), symbols.end());

    for (const auto& sym : symbols) {
        const auto& st = store_.all().at(sym);
        out << sym << "\n";
        out << "  last_ts: " << st.last_ts << "\n";
        out << "  last_price: " << (st.has_price ? st.last_price : 0.0) << "\n";
        out << "  counts: PRICE_UPDATE=" << st.price_updates
            << " ORDER_NEW=" << st.order_new
            << " ORDER_FILL=" << st.order_fill
            << " ORDER_CANCEL=" << st.order_cancel
            << "\n\n";
    }
    return out.str();
}

} // namespace mkt
