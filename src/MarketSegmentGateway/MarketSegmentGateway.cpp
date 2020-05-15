//
// Created by pbeerkens on 4/23/20.
//

#include <sys/socket.h>
#include <string>
#include <cstring>
#include "MarketSegmentGateway.h"
#include <Logger/LogMacros.h>
#include <netinet/in.h>
#include <future>
#include <arpa/inet.h>
#include <Connection/FIXPConnection.h>

bool MarketSegmentGateway::initialize(const MSGWSettings &msgwSettings) {
    port_=msgwSettings.port_;

    socket_=::socket (AF_INET, SOCK_STREAM, 0);
    if (socket_==-1) {
        LOGERROR ("MarketSegmentGateway::initialize failed to create socket with error code "<<std::to_string(errno)<<" ("<<strerror (errno)<<")");
        port_=-1; //reset so that we could call start again possibly with a different port
        return false;
    }

    int enable = 1;
    if (setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
        LOGERROR ("setsockopt(SO_REUSEADDR) failed");
    }

    struct sockaddr_in sockAddr;
    bzero (&sockAddr, sizeof (sockAddr));
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_addr.s_addr = htonl (INADDR_ANY);
    sockAddr.sin_port = htons (port_);

    auto result=::bind (socket_, reinterpret_cast<const struct sockaddr*>(&sockAddr),sizeof (sockAddr));
    if (result!=0) {
        LOGERROR ("MarketSegmentGateway::initialize failed to bind socket on port "<<std::to_string (port_)<<" with error code "<<std::to_string(errno)<<" ("<<strerror (errno)<<")");
        port_=-1; //reset so that we could call start again possibly with a different port
        return false;
    }

    result = ::listen (socket_, SOMAXCONN); //allow the maximum number of connections to be queued up waiting for an accept call
    if (result!=0) {
        LOGERROR ("MarketSegmentGateway::initialize failed to listen socket on port "<<std::to_string (port_)<<" with error code "<<std::to_string(errno)<<" ("<<strerror (errno)<<")");
        port_=-1; //reset so that we could call start again possibly with a different port
        return false;
    }

    return true;
}

bool MarketSegmentGateway::start() {
    assert (threadPtr_==nullptr);

    std::promise <void> threadStartedPromise_;
    threadPtr_=std::make_unique <std::thread> ([this, &threadStartedPromise_] () {
        threadStartedPromise_.set_value();
        this->handleClientConnections ();});

    threadStartedPromise_.get_future().wait(); //wait till thread has been started;
    return true;
}

bool MarketSegmentGateway::stop() {
    return false;
}

void MarketSegmentGateway::handleClientConnections() { //accept loop
    LOGINFO ("MarketSegmentGateway listening on port "<<std::to_string (port_));

    while (!requestedToTerminate_.load(std::memory_order_relaxed)) {
        struct sockaddr_in clientAddr;
        socklen_t len=sizeof (clientAddr);

        auto newSocket=::accept (socket_, reinterpret_cast<struct sockaddr*>(&clientAddr), &len);
        //If accept fails it could be a temporary thing. For example there could be a handle leak somewhere in the app
        //and we can force increase the number of handles for the application or some connections disconnect which frees
        //up some more handles. Better to keep trying than to give up.
        if (newSocket==-1) {
            LOGERROR ("MarketSegmentGateway::handleClientConnections accept call failed on port "<<std::to_string (port_)
              <<" with error code "<<std::to_string(errno)<<" ("<<strerror (errno)<<"). Going to try again in one minute");
            for (std::int32_t loopCount=60;!requestedToTerminate_&&loopCount>0;--loopCount) std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }

        //Get the remote ip address. Remote port is less interesting as it is temporary in most cases.
        char remoteIP [20] = {0};
        if (::inet_ntop (AF_INET, reinterpret_cast <const void*>(&clientAddr.sin_addr), remoteIP, sizeof (remoteIP))==nullptr) {
            LOGWARN ("Failed to get remote ip address");
        }
        LOGINFO ("MarketSegmentGateway connected to a client running on "<<remoteIP);

        auto newConnection=std::make_shared<FIXPConnection <MarketSegmentGateway>> (newSocket, remoteIP,*this);
        {
            std::lock_guard<std::mutex> guard (mutex_);
            activeConnections_.insert(newConnection);
        }

        std::promise <void> threadStartedPromise;
        std::thread([newConnection,&threadStartedPromise]() {
            LOGINFO ("Starting connection thread for connection ID:"<<newConnection->getConnectionId ());
            threadStartedPromise.set_value();
            newConnection->processMessages ();
            }).detach();

        threadStartedPromise.get_future().wait(); //wait till thread has been started;

    }
}

void MarketSegmentGateway::connectionEnd(std::shared_ptr<FIXPConnection <MarketSegmentGateway>> connection) {
    size_t itemsRemoved;
    {
        std::lock_guard <std::mutex> guard (mutex_);
        assert (activeConnections_.count (connection)>0&&"Trying to delete an unknown connection.");
        itemsRemoved=activeConnections_.erase (connection);
    }
    if (itemsRemoved>0) {
        LOGINFO ("[ConId: "<<connection->getConnectionId()<<"] Removing connection from list of active connections.");
    } else {
        LOGERROR ("[ConId: "<<connection->getConnectionId()<<"] Tried to remove unkown connection with id. This should never happen.");
    }
}

void MarketSegmentGateway::onMessage(const IL3Msg::NegotiateMsg &msg, FIXPConnection<MarketSegmentGateway> &connection) {

    IL3Msg::NegotiationResponseMsgOut outMsg;

    outMsg.setUUID (msg.getUUID());
    outMsg.setRequestTimestamp(msg.getRequestTimestamp());
    outMsg.setFaultToleranceIndicator(IL3Enum::FTI::Primary);
    outMsg.setPreviousSeqNo(1);
    outMsg.setPreviousUUID(msg.getUUID());

    connection.sendMsg (outMsg);
}

void MarketSegmentGateway::onMessage(const IL3Msg::EstablishMsg &msg, FIXPConnection<MarketSegmentGateway> &connection) {

    //TODO manage UUIDs
    connection.setUuid(msg.getUUID());
    std::size_t nextFreeSeqNo=1;
    connection.setNextSeqNo(nextFreeSeqNo);

    IL3Msg::EstablishmentAckMsgOut outMsg;
    outMsg.setUUID(msg.getUUID());
    outMsg.setRequestTimestamp (msg.getRequestTimestamp());
    outMsg.setNextSeqNo (nextFreeSeqNo);
    outMsg.setPreviousSeqNo (1);
    outMsg.setPreviousUUID(msg.getUUID ());
    //outMsg.setPreviousUUID(2);
    outMsg.setKeepAliveInterval(msg.getKeepAliveInterval ());

    connection.sendMsg(outMsg);
}

void MarketSegmentGateway::onMessage(const IL3Msg::SequenceMsg  &msg, FIXPConnection<MarketSegmentGateway> &) {
    LOGINFO("Recieved SequenceMsg UUID:"<<msg.getUUID()
      <<" nextSeqNo:"<<msg.getNextSeqNo()
      <<" faultToleranceInd:"<<msg.getFaultToleranceIndicator()
      <<" keepAliveIntervalElapsed: "<<msg.getKeepAliveIntervalLapsed());
}

void MarketSegmentGateway::onMessage(const IL3Msg::RetransmitRequestMsg &msg,
                                     FIXPConnection<MarketSegmentGateway> &) {
    LOGINFO ("Recieved RetransmitRequestMsg "
      <<" UUID:"<<msg.getUUID()
      <<" requestTimestamp:"<<msg.getRequestTimestamp()
      <<" lastUUID:"<<msg.getLastUUID()
      <<" msgCount:"<<msg.getMsgCount()
      <<" fromSeqNo: "<<msg.getFromSeqNo());

}

void MarketSegmentGateway::onMessage(const IL3Msg::NewOrderSingleMsg &msg,
                                     [[maybe_unused]] FIXPConnection<MarketSegmentGateway> &connection) {
    std::cout<<msg.getSeqNum()<<" price: "<<msg.getPrice().mantissa<<std::endl;

    auto now=std::chrono::system_clock::now ();

#define COPY_FROM_MSG(F) replyMsg.set##F (msg.get##F());

    IL3Msg::ExecutionReportNewMsgOut replyMsg;
    replyMsg.setSeqNum (connection.getNextSeqNoAndIncrement()); //todo this needs to be synchronized as other threads might be trying to send
    replyMsg.setUUID (connection.getUuid());
    replyMsg.setExecID("ExecutionId");
    COPY_FROM_MSG(SenderID);
    COPY_FROM_MSG(ClOrdID);
    COPY_FROM_MSG(PartyDetailsListReqID);
    replyMsg.setOrderID(100); //TODO need to get this from an to be build order management system
    IL3Composite::PRICE9 price {msg.getPrice ().mantissa};

    replyMsg.setPrice( price);
    COPY_FROM_MSG(StopPx);
    replyMsg.setTransactTime(now.time_since_epoch().count());
    replyMsg.setSendingTimeEpoch(now.time_since_epoch().count());
    COPY_FROM_MSG(OrderRequestID);

    COPY_FROM_MSG(Location);
    COPY_FROM_MSG(SecurityID);
    COPY_FROM_MSG(OrderQty)
    COPY_FROM_MSG(MinQty);

    connection.sendMsg(replyMsg);


}



/*
bool MarketSegmentGateway::processMessage(MessageBuffer &msgBuffer) {
    auto newMsg=msgFactory_.processMessage (msgBuffer);
    return false;
}
 */

