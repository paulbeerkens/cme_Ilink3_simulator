//
// Created by pbeerkens on 4/24/20.
//

#ifndef CMESIMULATOR_MSGFACTORY_H
#define CMESIMULATOR_MSGFACTORY_H

#include <memory>
#include <unordered_map>
#include <libWebServer/webserver/WebServer.h>
#include "ILink3Msg.h"
#include "MessageBuffer.h"
#include "MsgFactorySettings.h"
#include <ILink3Common/ILink3FieldDefinition.h>
#include "ILink3MsgDefinition.h"

class MsgFactory {
public:
    MsgFactory () {
        webserver::WebServer::get ().registerURL("/ILink3MsgFormat", [this] (const webserver::UrlRequest& req, std::ostream& os) {
            this->WebRequestMsgFormat (req, os);
            return true;
        });
    }
    bool initialize (MsgFactorySettings& msgFactorySettings);

    std::unique_ptr<ILink3Msg> processMessage (MessageBuffer &msgBuffer);
protected:
    std::unordered_map <std::string, ILink3FieldDefinition> ilink3Fields_;
    std::unordered_map <ILink3MsgDefinition::ID, ILink3MsgDefinition> ilink3Msgs_;
    void WebRequestMsgFormat (const webserver::UrlRequest& req, std::ostream& os) const;
};


#endif //CMESIMULATOR_MSGFACTORY_H
