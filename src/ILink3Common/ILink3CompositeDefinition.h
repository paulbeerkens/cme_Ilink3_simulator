//
// Created by pbeerkens on 5/12/20.
//

#ifndef CMESIMULATOR_ILINK3COMPOSITEDEFINITION_H
#define CMESIMULATOR_ILINK3COMPOSITEDEFINITION_H


#include <string>
#include <vector>
#include "ILink3FieldDefinition.h"


struct ILink3CompositeField {
    std::string name_;
    const ILink3FieldDefinition *encodingType_{nullptr};
};

struct ILink3CompositeDefinition {
    std::string name_;
    std::string description_;
    std::vector <ILink3CompositeField> fields_;
};

#endif //CMESIMULATOR_ILINK3COMPOSITEDEFINITION_H
