//
// Created by pbeerkens on 4/23/20.
//

#ifndef CMESIMULATOR_FIXPCONNECTION_H
#define CMESIMULATOR_FIXPCONNECTION_H

#include <cstdint>
#include <string>
#include "IConnectionCB.h"
#include <ILINK3Messages/MessageBuffer.h>
#include <ILINK3Messages/ILink3Headers.h>
#include <ILINK3Messages/MsgFactory.h>
#include <Generated/ILink3ConstGen.h>
#include <Logger/LogMacros.h>
#include <sys/socket.h>


#define LOGID "[ConId: "<<connectionId_<<"] "

template <class CallbackType>
class FIXPConnection: public std::enable_shared_from_this<FIXPConnection <CallbackType> > {
public:
    FIXPConnection (std::int32_t socket, const std::string& remoteHost, CallbackType& cb);

    ~FIXPConnection() {
        stop ();

    }

    void stop ();

    void processMessages ();

    uint32_t getConnectionId() const {return connectionId_;}

    template <class MsgType> void sendMsg (MsgType& msg);

protected:

    std::int32_t socket_;
    const std::string remoteHost_;

    inline static std::atomic <std::uint32_t> nextFreeConnectionId_ {1};
    std::uint32_t connectionId_;

    //std::unique_ptr <std::thread> threadPtr_;
    std::atomic <bool> requestedToTerminate_ {false};

    CallbackType& cb_;
    MsgFactory<CallbackType> msgFactory_;

    //bool readN (char* buf, std::size_t bytesToRead);
};

template<class CallbackType>
FIXPConnection<CallbackType>::FIXPConnection(std::int32_t socket, const std::string &remoteHost, CallbackType &cb)
        :socket_ (socket)
        ,remoteHost_ (remoteHost)
        ,cb_ (cb)
        ,msgFactory_(cb)
{
    connectionId_=nextFreeConnectionId_++;
}


template<class CallbackType>
void FIXPConnection <CallbackType>::processMessages() {
    MessageBuffer SOFHBuffer (sizeof(SOFH));
    MessageBuffer SBEHeaderAndMessage (1024);

    while (!requestedToTerminate_) {

        SOFHBuffer.reset();
        if (!SOFHBuffer.readFromSocket(socket_,sizeof (SOFH) )) {
            LOGERROR (LOGID<<"Failed to read Simple Open Framing Header.");
            break;
        }
        const SOFH* header= reinterpret_cast<const SOFH*> (SOFHBuffer.getRdPtr ());

        if (header->enodingType_!=0xcafe) {
            //TODO report this error to a central bad message repository which will be shown on a web page for debugging purposes
            LOGERROR (LOGID<<"Encoding type in header is not recognized. Not a valid message. Received "<<std::showbase<<std::hex<<header->enodingType_);
            break;
        }

        //Read the rest of the message

        //The size in the SOFH is inclusive of SOFH so we need to subtract that (SOFH=Simple Open Framing Header)
        std::size_t restOfMessageSize=header->msgSize_-sizeof (SOFH);
        //We are going to reuse the same message buffer but reset (and make sure it is big enough)
        SBEHeaderAndMessage.expandIfRequired(restOfMessageSize);
        SBEHeaderAndMessage.reset();
        if (!SBEHeaderAndMessage.readFromSocket(socket_,restOfMessageSize )) {
            //TODO report this error to a central bad message repository which will be shown on a web page for debugging purposes
            LOGERROR (LOGID<<"Failed to read message after SOFH.")
            break;
        }

        if (!msgFactory_.processMessage(SBEHeaderAndMessage, *this)) {
            //TODO report this error to a central bad message repository
            continue;
        }
    }

    cb_.connectionEnd(this->shared_from_this());
}

template<class CallbackType>
void FIXPConnection <CallbackType>::stop() {
    requestedToTerminate_=true;
    if (socket_!=-1) {
        ::shutdown (socket_, SHUT_RDWR); //send the FIN. This makes out code come out of recv calls
        ::close (socket_);
        socket_=-1;
    }
}

template<class CallbackType>
template<class MsgType>
void FIXPConnection<CallbackType>::sendMsg(MsgType &msg) {
    MessageBuffer msgBuffer (2000);
    msg.writeToBuffer (msgBuffer);
    if (msgBuffer.bad ()) {
        LOGERROR("Failed to write Msg with id "<<MsgType::id<<" send message buffer too small.");
        return;
    }

    MessageBuffer headerBuffer (sizeof (SOFH)+sizeof (SBEHeader));
    SOFH* sofh= reinterpret_cast<SOFH*>(headerBuffer.getWrtPtr ());
    SBEHeader* sbeHeader= reinterpret_cast<SBEHeader*> ((char*)sofh+sizeof (SOFH));

    sofh->msgSize_=msgBuffer.getLeftToRead()+sizeof (SOFH)+sizeof (SBEHeader);
    sofh->enodingType_=0xCAFE;
    sbeHeader->templateId_=MsgType::id;
    sbeHeader->blockLength_=MsgType::blockLength;
    sbeHeader->schemaId_=IL3Const::SCHEMA_ID;
    sbeHeader->version_=IL3Const::VERSION;
    headerBuffer.moveWrtPtr (sizeof (SOFH)+sizeof (SBEHeader));

    //could combine the two buffers into one. Look at this after we got this working
    if (!headerBuffer.sendToSocket(socket_)) {
        return;
    }

    if (!msgBuffer.sendToSocket(socket_)) {
        return;
    }
}

#endif //CMESIMULATOR_FIXPCONNECTION_H
