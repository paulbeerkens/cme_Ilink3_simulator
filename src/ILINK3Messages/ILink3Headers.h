#ifndef CMESIMULATOR_ILINK3HEADERS_H
#define CMESIMULATOR_ILINK3HEADERS_H


#include <cstdint>

struct SOFH { //Simple Open Framing Header
    std::uint16_t msgSize_;
    std::uint16_t enodingType_;
};

#endif //CMESIMULATOR_ILINK3HEADERS_H
