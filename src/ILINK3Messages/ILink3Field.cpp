//
// Created by pbeerkens on 4/27/20.
//

#include "ILink3Field.h"

PrimitiveType PrimitiveTypeFromString(const std::string &type) {

    static std::unordered_map<std::string, PrimitiveType> lookup{
            {"char",   PrimitiveType::char_t},
            {"uint8",  PrimitiveType::uint8_t},
            {"uint16", PrimitiveType::uint16_t},
            {"uint32", PrimitiveType::uint32_t},
            {"uint64", PrimitiveType::uint64_t},
            {"int32",  PrimitiveType::int32_t},
            {"int64",  PrimitiveType::int64_t}
    };

    auto itr = lookup.find(type);
    if (itr != lookup.end()) return itr->second;

    return PrimitiveType::none_t;

}
