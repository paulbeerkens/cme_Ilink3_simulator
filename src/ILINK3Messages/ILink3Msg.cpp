//
// Created by pbeerkens on 4/27/20.
//

#include "ILink3Msg.h"

void NegotiateMsg::readFromBuffer(MessageBuffer &msgBuffer) {
    blockData_=reinterpret_cast<const NegotiateData*>(msgBuffer.getRdPtr());
    msgBuffer.moveRdPtr (sizeof (NegotiateData));
}
