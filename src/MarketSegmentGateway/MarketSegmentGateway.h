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
//#include <Connection/IConnectionCB.h>
#include <ILINK3Messages/MsgFactory.h>

class MarketSegmentGateway {
public:
    /*MarketSegmentGateway ( MsgFactory& msgFactory)
    :msgFactory_ (msgFactory) {};*/

    bool initialize (const MSGWSettings& msgwSettings);
    bool start ();
    bool stop ();

    //IConnectionCB
    void connectionEnd (std::shared_ptr<FIXPConnection <MarketSegmentGateway>> connection);
    //bool processMessage(MessageBuffer &msgBuffer) override;


    //Callback from MsgFactory
    void onMessage ([[maybe_unused]] const IL3Msg::NegotiateMsg& msg, [[maybe_unused]] FIXPConnection<MarketSegmentGateway>& connection);
protected:
    std::int32_t port_ {-1};
    std::int32_t socket_ {-1};

    void handleClientConnections (); //runs in the *threadPtr thread.
    std::unique_ptr <std::thread> threadPtr_;
    std::atomic <bool> requestedToTerminate_ {false};

    std::mutex mutex_;
    std::unordered_set <std::shared_ptr <FIXPConnection <MarketSegmentGateway>>> activeConnections_; //protected by mutex_;

    //MsgFactory& msgFactory_;
};


#endif //CMESIMULATOR_MARKETSEGMENTGATEWAY_H
