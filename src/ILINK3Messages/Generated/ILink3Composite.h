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

//Optional Price with constant exponent -9
struct PRICENULL9 {
    std::int64_t mantissa {9223372036854775807};
    static const std::int8_t exponent {-9};
};

struct ExecInst {
    std::uint8_t bits_;
};


} //end of namespace IL3Composite

#endif //CMESIMULATOR_ILINK3COMPOSITE_H