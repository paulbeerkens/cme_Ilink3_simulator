//
// Created by pbeerkens on 5/7/20.
//

#include "ILink3Composite.h"

void IL3Composite::Data::writeToBuffer(MessageBuffer &msgBuffer) {
    msgBuffer.write (reinterpret_cast<void*>(&length), sizeof (length));
    if (length>0) {
        msgBuffer.write (reinterpret_cast<void*>(varData), length);
    }
}
