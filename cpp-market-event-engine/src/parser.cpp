#include "parser.hpp"
#include "util.hpp"
#include <charconv>
#include <cstdlib>
#include <limits>

namespace mkt {

EventParser::EventParser(std::istream& in) : in_(in) {}

EventType EventParser::parse_type(const std::string& t) {
    if (t == "PRICE_UPDATE") return EventType::PriceUpdate;
    if (t == "ORDER_NEW")    return EventType::OrderNew;
    if (t == "ORDER_FILL")   return EventType::OrderFill;
    if (t == "ORDER_CANCEL") return EventType::OrderCancel;
    return EventType::Unknown;
}

static bool parse_i64(std::string_view sv, std::int64_t& out) {
    sv = std::string_view(sv.data(), sv.size());
    auto s = mkt::util::trim(sv);
    if (s.empty()) return false;
    const char* b = s.data();
    const char* e = s.data() + s.size();
    std::int64_t v = 0;
    auto res = std::from_chars(b, e, v);
    if (res.ec != std::errc() || res.ptr != e) return false;
    out = v;
    return true;
}

static bool parse_double(std::string_view sv, double& out) {
    auto s = mkt::util::trim(sv);
    if (s.empty()) return false;
    char* endp = nullptr;
    // std::strtod requires null-terminated string
    std::string tmp(s);
    double v = std::strtod(tmp.c_str(), &endp);
    if (!endp || *endp != '\0') return false;
    out = v;
    return true;
}

std::optional<Event> EventParser::next() {
    last_error_.clear();
    while (std::getline(in_, line_)) {
        line_no_++;

        // Skip comments / blank lines
        auto trimmed = util::trim(line_);
        if (trimmed.empty()) continue;
        if (!trimmed.empty() && trimmed[0] == '#') continue;

        // Header line skip (optional)
        if (trimmed.rfind("timestamp,", 0) == 0) continue;

        const auto parts = util::split_sv(trimmed, ',');
        if (parts.size() != 6) {
            last_error_ = "Expected 6 fields: timestamp,symbol,type,price,qty,side";
            Event bad;
            bad.line_no = line_no_;
            return bad;
        }

        Event e;
        e.line_no = line_no_;

        if (!parse_i64(parts[0], e.timestamp)) {
            last_error_ = "Invalid timestamp";
            return e;
        }

        e.symbol = util::trim(parts[1]);
        if (e.symbol.empty()) {
            last_error_ = "Empty symbol";
            return e;
        }

        auto type_s = util::trim(parts[2]);
        e.type = parse_type(type_s);
        if (e.type == EventType::Unknown) {
            last_error_ = "Unknown type: " + type_s;
            // We still return the event so engine can count unknowns deterministically.
        }

        if (!parse_double(parts[3], e.price)) {
            last_error_ = "Invalid price";
            return e;
        }

        if (!parse_i64(parts[4], e.qty)) {
            last_error_ = "Invalid qty";
            return e;
        }

        auto side_s = util::trim(parts[5]);
        if (side_s.size() != 1) {
            last_error_ = "Invalid side (expected B, S, or -)";
            return e;
        }
        e.side = side_s[0];
        if (!(e.side == 'B' || e.side == 'S' || e.side == '-')) {
            last_error_ = "Invalid side (expected B, S, or -)";
            return e;
        }

        return e;
    }
    return std::nullopt; // EOF
}

} // namespace mkt
