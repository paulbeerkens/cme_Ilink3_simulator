//
// Created by pbeerkens on 4/27/20.
//

#ifndef CMESIMULATOR_ILINK3FIELD_H
#define CMESIMULATOR_ILINK3FIELD_H

#include <string>
#include <unordered_map>

enum class PrimitiveType {none_t,char_t, uint8_t, uint16_t, uint32_t, uint64_t,int32_t, int64_t};

struct ILink3Field {
    std::string name_;
    std::string description_;
    std::size_t length_ {0};
    PrimitiveType primitiveType_ {PrimitiveType::none_t};
};

PrimitiveType PrimitiveTypeFromString (const std::string& type);

#endif //CMESIMULATOR_ILINK3FIELD_H
