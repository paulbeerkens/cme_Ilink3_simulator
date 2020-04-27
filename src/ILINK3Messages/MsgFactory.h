//
// Created by pbeerkens on 4/24/20.
//

#ifndef CMESIMULATOR_MSGFACTORY_H
#define CMESIMULATOR_MSGFACTORY_H

#include <memory>
#include <unordered_map>
#include "ILink3Msg.h"
#include "MessageBuffer.h"
#include "MsgFactorySettings.h"
#include "ILink3Field.h"

class MsgFactory {
public:
    bool initialize (MsgFactorySettings& msgFactorySettings);

    std::unique_ptr<ILink3Msg> processMessage (MessageBuffer &msgBuffer);
protected:
    std::unordered_map <std::string, ILink3Field> ilink3Fields_;
};


#endif //CMESIMULATOR_MSGFACTORY_H
