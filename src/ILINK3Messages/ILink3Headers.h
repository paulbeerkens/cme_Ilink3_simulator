#ifndef CMESIMULATOR_ILINK3HEADERS_H
#define CMESIMULATOR_ILINK3HEADERS_H


#include <cstdint>

struct SOFH { //Simple Open Framing Header
    std::uint16_t msgSize_;
    std::uint16_t enodingType_;
};

struct SBEHeader {//Simple Binary Encoding Header
    std::uint16_t blockLength_;
    std::uint16_t templateId_;
    std::uint16_t schemaId_;
    std::uint16_t version_;
};

#endif //CMESIMULATOR_ILINK3HEADERS_H
