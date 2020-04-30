//
// Created by pbeerkens on 4/29/20.
//

#ifndef CMESIMULATOR_ILINK3ENUMDEFINITION_H
#define CMESIMULATOR_ILINK3ENUMDEFINITION_H

#include <string>
#include "ILink3FieldDefinition.h"

struct ILink3EnumDefinition {
    std::string name_;
    const ILink3FieldDefinition* encodingType {nullptr};

};


#endif //CMESIMULATOR_ILINK3ENUMDEFINITION_H
