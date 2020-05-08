//
// Created by pbeerkens on 4/27/20.
//

#include <sstream>
#include "ILink3FieldDefinition.h"

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

std::ostream &operator<<(std::ostream &os, PrimitiveType type) {
    {
        switch (type) {
            #define TYPEOUTPUT(t) case PrimitiveType::t: os<<"std::"<<#t; break;
            #define TYPEOUTPUT2(t,s) case PrimitiveType::t: os<<#s; break;

            TYPEOUTPUT(none_t)
            TYPEOUTPUT2(char_t,char)
            TYPEOUTPUT(uint8_t)
            TYPEOUTPUT(uint16_t)
            TYPEOUTPUT(uint32_t)
            TYPEOUTPUT(uint64_t)
            TYPEOUTPUT(int32_t)
            TYPEOUTPUT(int64_t)
        }
        return os;
    }
}

bool primitiveTypeIsUnsigned(PrimitiveType pt) {
    switch (pt) {
        case PrimitiveType::uint8_t:
        case PrimitiveType::uint16_t:
        case PrimitiveType::uint32_t:
        case PrimitiveType::uint64_t:
            return true;
            break;
        case PrimitiveType::int32_t:
        case PrimitiveType::int64_t:
        case PrimitiveType::char_t:
        case PrimitiveType::none_t:
            return false;
            break;
    }
    return false;
}

std::string ILink3FieldDefinition::structFieldPrefix() const{
    std::stringstream ss;
    if (primitiveType_!=PrimitiveType::char_t) {
        ss<<primitiveType_;
        return ss.str ();
    }

    ss<<"std::array<char, "<<length_<<">";
    return ss.str ();
}

std::string ILink3FieldDefinition::getFunctionReturnType() const {
    std::stringstream ss;
    if (primitiveType_!=PrimitiveType::char_t) {
        ss<<primitiveType_;
        return ss.str ();
    }

    return "std::string_view";
}

std::string ILink3FieldDefinition::getFunctionImpl([[maybe_unused]]const std::string &fieldName) const {
    std::stringstream ss;
    ss<<"return ";
    if (primitiveType_!=PrimitiveType::char_t) {
        //return blockData_->UUID;
        ss<<"blockData_->"<<fieldName<<";";
    } else {
        //return std::string_view(&blockData_->Session[0], 3);
        ss<<"std::string_view(&blockData_->"<<fieldName<<"[0], "<<length_<<");";
    }
    return ss.str ();
}
