#include "state_store.hpp"

namespace mkt {

SymbolState& StateStore::get_or_create(const std::string& symbol) {
    auto it = states_.find(symbol);
    if (it != states_.end()) return it->second;

    SymbolState st;
    st.symbol = symbol;
    auto [ins_it, _] = states_.emplace(symbol, std::move(st));
    return ins_it->second;
}

} // namespace mkt
