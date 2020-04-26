//
// Created by pbeerkens on 4/22/20.
//

#include "CMESimulator.h"
#include <Logger/LogMacros.h>
#include <MarketSegmentGateway/MarketSegmentGateway.h>
#include <ILINK3Messages/MsgFactory.h>

bool CMESimulator::setup() {
    msgwSettings_.port_=25000;

    return true;
}

bool CMESimulator::run() {
    MsgFactory msgFactory;
    MarketSegmentGateway msgw (msgFactory);

    if (!msgFactory.initialize ()) {
        LOGERROR ("Failed to initialze Message Factory. This is fatal.")
        return false;
    }

    if (!msgw.initialize(msgwSettings_)) {
        LOGERROR ("Failed to initialize Market Segment Gateway. This is fatal.")
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

