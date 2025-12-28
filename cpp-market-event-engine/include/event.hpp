#pragma once
#include <cstdint>
#include <string>

namespace mkt {

enum class EventType {
    PriceUpdate,
    OrderNew,
    OrderFill,
    OrderCancel,
    Unknown
};

struct Event {
    std::int64_t timestamp = 0;
    std::string symbol;
    EventType type = EventType::Unknown;
    double price = 0.0;
    std::int64_t qty = 0;
    char side = '-'; // 'B', 'S', or '-'
    std::size_t line_no = 0; // for diagnostics
};

inline const char* to_string(EventType t) {
    switch (t) {
        case EventType::PriceUpdate: return "PRICE_UPDATE";
        case EventType::OrderNew:    return "ORDER_NEW";
        case EventType::OrderFill:   return "ORDER_FILL";
        case EventType::OrderCancel: return "ORDER_CANCEL";
        default:                     return "UNKNOWN";
    }
}

} // namespace mkt
