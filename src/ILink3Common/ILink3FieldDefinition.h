//
// Created by pbeerkens on 4/27/20.
//

#ifndef CMESIMULATOR_ILINK3FIELDDEFINITION_H
#define CMESIMULATOR_ILINK3FIELDDEFINITION_H

#include <string>
#include <unordered_map>
#include <ostream>

enum class PrimitiveType {none_t,char_t, uint8_t, uint16_t, uint32_t, uint64_t,int32_t, int64_t};

struct ILink3FieldDefinition {
    std::string name_;
    std::string description_;
    std::size_t length_ {1};
    PrimitiveType primitiveType_ {PrimitiveType::none_t};
    std::optional <std::uint64_t> nullValue;

    //helper functions for code generation
    [[nodiscard]] std::string structFieldPrefix  () const;
    [[nodiscard]] std::string getFunctionReturnType  () const;
    [[nodiscard]] std::string getFunctionImpl (const std::string& fieldName) const;
    [[nodiscard]] std::string setFunctionImpl (const std::string& fieldName) const;
};

PrimitiveType PrimitiveTypeFromString (const std::string& type);

std::ostream& operator << (std::ostream& os, PrimitiveType type);

bool primitiveTypeIsUnsigned (PrimitiveType pt);

#endif //CMESIMULATOR_ILINK3FIELDDEFINITION_H

