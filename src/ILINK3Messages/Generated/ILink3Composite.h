//
// Created by pbeerkens on 5/7/20.
//

#ifndef CMESIMULATOR_ILINK3COMPOSITE_H
#define CMESIMULATOR_ILINK3COMPOSITE_H

#include <cstdint>
#include <ILINK3Messages/MessageBuffer.h>

namespace IL3Composite {

struct Data {
    std::uint16_t length{0};
    char *varData;

    void writeToBuffer(MessageBuffer &msgBuffer);
};


} //end of namespace IL3Composite

#endif //CMESIMULATOR_ILINK3COMPOSITE_H