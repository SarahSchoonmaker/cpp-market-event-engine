#include "engine.hpp"
#include <cassert>

int main() {
    mkt::EngineConfig cfg;
    cfg.emit_alerts = true;
    cfg.price_jump_bps = 10; // easy to trigger in test

    mkt::MarketEventEngine eng(cfg);

    mkt::Event e1;
    e1.timestamp = 1;
    e1.symbol = "AAPL";
    e1.type = mkt::EventType::PriceUpdate;
    e1.price = 100.0;

    mkt::Event e2 = e1;
    e2.timestamp = 2;
    e2.price = 100.2; // 20 bps

    eng.process(e1);
    eng.process(e2);

    assert(eng.stats().parsed_events == 2);
    assert(!eng.alerts().empty());
    assert(eng.state().all().at("AAPL").price_updates == 2);

    return 0;
}
