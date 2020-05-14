//
// Created by pbeerkens on 5/7/20.
//

#ifndef CMESIMULATOR_ILINK3COMPOSITE_H
#define CMESIMULATOR_ILINK3COMPOSITE_H

#include <cstdint>
#include <ILINK3Messages/MessageBuffer.h>
#include <set>
#include <string>

namespace IL3Composite {

inline std::set<std::string> knownCompositeTypes {"Decimal32NULL","Decimal64NULL","MaturityMonthYear",
                                           "PRICE9","PRICENULL9","groupSize","messageHeader","ExecInst"};

struct Data {
    std::uint16_t length{0};
    char *varData;

    void writeToBuffer(MessageBuffer &msgBuffer);
};

//Optional floating point decimal with int32 mantissa and int8 exponent
struct Decimal32NULL {
    std::int32_t mantissa {2147483647};
    std::int8_t exponent {127};
};

//Optional floating point decimal
struct Decimal64NULL {
    std::int64_t mantissa {9223372036854775807};
    std::int8_t exponent {127};
};

//Year, Month and Date
struct MaturityMonthYear {
    std::uint16_t year {65535};
    std::uint8_t month {255};
    std::uint8_t day {255};
    std::uint8_t week {255};
};

//Price with constant exponent -9
struct PRICE9 {
    std::int64_t mantissa;
    static const std::int8_t exponent {-9};
};

//Optional Price with constant exponent -9
struct PRICENULL9 {
    std::int64_t mantissa {9223372036854775807};
    static const std::int8_t exponent {-9};
};

//Repeating group dimensions
struct groupSize {
    std::uint16_t blockLength;
    std::uint16_t numInGroup;
};

//Template ID and length of message root
struct messageHeader {
    std::uint16_t blockLength;
    std::uint16_t templateId;
    std::uint16_t schemaId;
    std::uint16_t version;
};

struct ExecInst {
    std::uint8_t bits_;
};


} //end of namespace IL3Composite

#endif //CMESIMULATOR_ILINK3COMPOSITE_H