#ifndef CMESIMULATOR_CMESIMULATOR_H
#define CMESIMULATOR_CMESIMULATOR_H


#include <thread>
#include <MarketSegmentGateway/MSGWSettings.h>
#include <ILINK3Messages/MsgFactorySettings.h>

class CMESimulator {
public:
    bool setup ();
    bool run ();

protected:
    MSGWSettings msgwSettings_;
    MsgFactorySettings msgFactorySettings_;
};


#endif //CMESIMULATOR_CMESIMULATOR_H
