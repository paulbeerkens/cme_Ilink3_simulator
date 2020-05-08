//
// Created by pbeerkens on 4/24/20.
//

#ifndef CMESIMULATOR_MSGFACTORY_H
#define CMESIMULATOR_MSGFACTORY_H

#include <memory>
#include <unordered_map>
#include <libWebServer/webserver/WebServer.h>
#include "Generated/ILink3Msg.h"
#include "MessageBuffer.h"
#include "MsgFactorySettings.h"
#include <ILink3Common/ILink3FieldDefinition.h>
#include "ILink3MsgDefinition.h"
#include "ILink3Headers.h"
#include <Logger/LogMacros.h>

#include <Generated/ILink3MsgsGen.h>

template <class CallBack> class FIXPConnection;

template <class CallBack>
class MsgFactory {
public:
    MsgFactory (CallBack& cb)
    :cb_ (cb) {};
    /*
    MsgFactory () {
        webserver::WebServer::get ().registerURL("/ILink3MsgFormat", [this] (const webserver::UrlRequest& req, std::ostream& os) {
            this->WebRequestMsgFormat (req, os);
            return true;
        });
    }
    bool initialize (MsgFactorySettings& msgFactorySettings);

    std::unique_ptr<ILink3Msg> processMessage (MessageBuffer &msgBuffer);
     */
    bool processMessage (MessageBuffer &msgBuffer, FIXPConnection<CallBack>& connection);
protected:
    /*
    std::unordered_map <std::string, ILink3FieldDefinition> ilink3Fields_;
    std::unordered_map <ILink3MsgDefinition::ID, ILink3MsgDefinition> ilink3Msgs_;
    void WebRequestMsgFormat (const webserver::UrlRequest& req, std::ostream& os) const;
     */

    CallBack& cb_; //Typical MarketSegmentGateway or OrderEntryServiceGateway. We call onMessage on this callback whenever a message is received.
};

template<class CallBack>
bool MsgFactory<CallBack>::processMessage(MessageBuffer &msgBuffer, FIXPConnection<CallBack> &connection) {
    const SBEHeader* sbeHeader= reinterpret_cast<const SBEHeader*>(msgBuffer.getRdPtr());
    msgBuffer.moveRdPtr (sizeof (SBEHeader));

    switch (sbeHeader->templateId_) {
        case IL3Msg::Negotiate::id: {
            IL3Msg::Negotiate newMsg;
            newMsg.readFromBuffer(msgBuffer);
            if (msgBuffer.bad()) {
                LOGERROR ("Failed to read msg with id " << IL3Msg::Negotiate::id);
                return false;
            }
            cb_.onMessage(newMsg, connection);
            break;
        }
        default:
            LOGERROR ("Unhandled message received. Template id: "<<sbeHeader->templateId_);
    }

    return true;
}


#endif //CMESIMULATOR_MSGFACTORY_H
