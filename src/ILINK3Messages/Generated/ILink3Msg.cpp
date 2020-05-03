//
// Created by pbeerkens on 4/27/20.
//

#include "ILink3Msg.h"

void IL3Msg::NegotiateMsg::readFromBuffer(MessageBuffer &msgBuffer) {
    blockData_=reinterpret_cast<const BlockData*>(msgBuffer.getRdPtr());
    msgBuffer.moveRdPtr (sizeof (BlockData));
}

void IL3Msg::NegotiationResponseMsgOut::writeToBuffer(MessageBuffer &msgBuffer) {
    msgBuffer.write(reinterpret_cast<void*>(&blockDataWrite), sizeof (blockDataWrite));
    data.writeToBuffer (msgBuffer);
}

void IL3Msg::Data::writeToBuffer(MessageBuffer &msgBuffer) {
    msgBuffer.write (reinterpret_cast<void*>(&length), sizeof (length));
    if (length>0) {
        msgBuffer.write (reinterpret_cast<void*>(varData), length);
    }
}
