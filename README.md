# cpp-market-event-engine

A **C++17** market-style **event processing engine** that ingests structured events, maintains per-symbol state, and emits deterministic derived outputs.
the focus is on **clean design**, **state management**, **correctness**, and **performance-aware** data structures.

## Project Features

- Modern C++17 (RAII, STL, move semantics)
- Clean module boundaries (Parser → Engine → StateStore → Output)
- Deterministic event ordering and state updates
- Performance-conscious parsing (streaming file processing; no full-file reads)
- Extendable “rules” / derived outputs (e.g., price jump alerts)

## Event format (simple, no external JSON libs)

The engine reads **one event per line** (CSV-like):

```
timestamp,symbol,type,price,qty,side
```

- `timestamp`: integer epoch seconds (or any increasing integer)
- `symbol`: e.g. `AAPL`
- `type`: `PRICE_UPDATE` | `ORDER_NEW` | `ORDER_FILL` | `ORDER_CANCEL`
- `price`: decimal
- `qty`: integer (0 allowed for PRICE_UPDATE)
- `side`: `B` | `S` | `-` (use `-` for PRICE_UPDATE)

Example:

```
1700000001,AAPL,PRICE_UPDATE,192.15,0,-
1700000002,AAPL,ORDER_NEW,192.10,100,B
1700000003,AAPL,ORDER_FILL,192.12,50,B
```

## Build

### Linux / macOS

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

Run:

```bash
./build/market_event_engine --input data/sample_events.csv --summary
```

### Windows (Visual Studio Community)

1. Open **Developer PowerShell for VS** (or use VS “Open a Developer Command Prompt”).
2. From the repo root:

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

Run:

```powershell
.\build\Release\market_event_engine.exe --input data\sample_events.csv --summary
```

## Usage

```bash
market_event_engine --input <file> [--summary] [--alerts] [--price-jump-bps <bps>]
```

Examples:

```bash
# Summaries only
./build/market_event_engine --input data/sample_events.csv --summary

# Summaries + alerts for >= 25 bps move (0.25%)
./build/market_event_engine --input data/sample_events.csv --summary --alerts --price-jump-bps 25
```

## Output

### Summary example

- Per symbol: last price, last timestamp, counts per event type
- Aggregate: total events processed, parse errors

### Alerts example

A “price jump” alert triggers when the absolute percent change from the previous price update
exceeds `--price-jump-bps` (basis points). Default is 50 bps (0.50%).

## Project layout

```
cpp-market-event-engine/
  CMakeLists.txt
  include/
    engine.hpp
    event.hpp
    parser.hpp
    state_store.hpp
    util.hpp
  src/
    main.cpp
    engine.cpp
    parser.cpp
    state_store.cpp
  data/
    sample_events.csv
  tests/
    test_engine.cpp
```

## Tests

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON
cmake --build build -j
ctest --test-dir build
```

## Extensions (easy wins)

- Add a bounded queue + worker thread(s) for parsing vs processing (preserving deterministic ordering per symbol)
- Add a simple “top movers” report
- Add a binary input format (faster than CSV) while keeping the same engine interface

## License

MIT
