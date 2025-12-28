#pragma once
#include "event.hpp"
#include <istream>
#include <optional>
#include <string>

namespace mkt {

// Streaming parser: reads one line at a time, converts to Event.
// Expected format:
// timestamp,symbol,type,price,qty,side
class EventParser {
public:
    explicit EventParser(std::istream& in);

    // Returns next parsed event; std::nullopt on EOF.
    // If a line is malformed, returns an Event with type Unknown, symbol empty,
    // and sets error_message().
    std::optional<Event> next();

    const std::string& error_message() const { return last_error_; }
    std::size_t line_no() const { return line_no_; }

private:
    std::istream& in_;
    std::string line_;
    std::string last_error_;
    std::size_t line_no_ = 0;

    static EventType parse_type(const std::string& t);
};

} // namespace mkt
