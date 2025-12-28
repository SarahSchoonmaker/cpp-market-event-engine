#pragma once
#include "event.hpp"
#include <cstdint>
#include <string>
#include <unordered_map>

namespace mkt {

struct SymbolState {
    std::string symbol;
    std::int64_t last_ts = 0;
    double last_price = 0.0;
    bool has_price = false;

    std::uint64_t price_updates = 0;
    std::uint64_t order_new = 0;
    std::uint64_t order_fill = 0;
    std::uint64_t order_cancel = 0;
};

class StateStore {
public:
    SymbolState& get_or_create(const std::string& symbol);
    const std::unordered_map<std::string, SymbolState>& all() const { return states_; }

private:
    std::unordered_map<std::string, SymbolState> states_;
};

} // namespace mkt
