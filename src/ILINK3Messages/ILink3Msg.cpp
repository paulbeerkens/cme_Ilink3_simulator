//
// Created by pbeerkens on 4/27/20.
//

#include "ILink3Msg.h"

void NegotiateMsg::readFromBuffer(MessageBuffer &msgBuffer) {
    blockData_=reinterpret_cast<const BlockData*>(msgBuffer.getRdPtr());
    msgBuffer.moveRdPtr (sizeof (BlockData));
}

void NegotiationResponseMsgOut::writeToBuffer(MessageBuffer &msgBuffer) {
    msgBuffer.write(reinterpret_cast<void*>(&blockDataWrite), sizeof (blockDataWrite));
}
