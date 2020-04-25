//
// Created by pbeerkens on 4/23/20.
//

#ifndef CMESIMULATOR_MARKETSEGMENTGATEWAY_H
#define CMESIMULATOR_MARKETSEGMENTGATEWAY_H


#include <cstdint>
#include <atomic>
#include "MSGWSettings.h"
#include <memory>
#include <thread>
#include <unordered_set>
#include <Connection/FIXPConnection.h>
#include <mutex>
#include <Connection/IConnectionCB.h>

class MarketSegmentGateway: public IConnectionCB  {
public:
    bool initialize (const MSGWSettings& msgwSettings);
    bool start ();
    bool stop ();

    //IConnectionCB
void connectionEnd (std::shared_ptr<FIXPConnection> connection) override;
protected:
    std::int32_t port_ {-1};
    int socket_ {-1};

    void handleClientConnections (); //runs in the *threadPtr thread.
    std::unique_ptr <std::thread> threadPtr_;
    std::atomic <bool> requestedToTerminate_ {false};

    std::mutex mutex_;
    std::unordered_set <std::shared_ptr <FIXPConnection>> activeConnections_; //protected by mutex_;


};


#endif //CMESIMULATOR_MARKETSEGMENTGATEWAY_H
