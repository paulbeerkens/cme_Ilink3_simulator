//
// Created by pbeerkens on 5/6/20.
//

#ifndef CMESIMULATOR_ILINK3MSGDEFINITION_H
#define CMESIMULATOR_ILINK3MSGDEFINITION_H

#include <cstdint>
#include <string>
#include <iostream>
#include <vector>
#include "ILink3FieldDefinition.h"
#include "ILink3EnumDefinition.h"

enum class MsgId: std::uint32_t {null=0};
enum class FieldId: std::uint32_t {null=0};

struct ILink3FieldInMsgDefinition {
    std::string name_;
    std::string description_;
    const ILink3FieldDefinition* fieldDefinition_ {nullptr};
    const ILink3EnumDefinition* enumDefinition_ {nullptr};
    FieldId fieldId_ {FieldId::null};
};

struct ILink3MsgDefinition {
    std::string name_;
    MsgId msgId_;
    std::size_t blockLength_;

    std::vector <ILink3FieldInMsgDefinition> fields_;
};

inline std::ostream& operator<< (std::ostream& os, MsgId id) {
    os<<static_cast<std::uint32_t>(id);
    return os;
}


#endif //CMESIMULATOR_ILINK3MSGDEFINITION_H
