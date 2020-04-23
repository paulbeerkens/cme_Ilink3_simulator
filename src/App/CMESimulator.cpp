//
// Created by pbeerkens on 4/22/20.
//

#include "CMESimulator.h"
#include <Logger/LogMacros.h>
#include <MarketSegmentGateway/MarketSegmentGateway.h>

bool CMESimulator::setup() {
    msgwSettings_.port_=25000;

    return true;
}

bool CMESimulator::run() {
    MarketSegmentGateway msgw;

    if (!msgw.initialize(msgwSettings_)) {
        LOGERROR ("Failed to initialize Market Segment Gateway")
        return false;
    }

    if (!msgw.start()) {
        LOGERROR ("Failed to start Market Segment Gateway")
        return false;
    }

    std::this_thread::sleep_for (std::chrono::seconds(60));

    msgw.stop ();

    return true;

}

