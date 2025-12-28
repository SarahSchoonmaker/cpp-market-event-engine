#pragma once
#include <string>
#include <string_view>
#include <vector>

namespace mkt::util {

// Split a string_view by a delimiter, returning views into the original string.
// Safe as long as the original string outlives the returned views.
inline std::vector<std::string_view> split_sv(std::string_view s, char delim) {
    std::vector<std::string_view> parts;
    std::size_t start = 0;
    while (start <= s.size()) {
        auto pos = s.find(delim, start);
        if (pos == std::string_view::npos) {
            parts.push_back(s.substr(start));
            break;
        }
        parts.push_back(s.substr(start, pos - start));
        start = pos + 1;
    }
    return parts;
}

inline std::string trim(std::string_view sv) {
    std::size_t b = 0;
    while (b < sv.size() && (sv[b] == ' ' || sv[b] == '\t' || sv[b] == '\r' || sv[b] == '\n')) b++;
    std::size_t e = sv.size();
    while (e > b && (sv[e-1] == ' ' || sv[e-1] == '\t' || sv[e-1] == '\r' || sv[e-1] == '\n')) e--;
    return std::string(sv.substr(b, e-b));
}

} // namespace mkt::util
