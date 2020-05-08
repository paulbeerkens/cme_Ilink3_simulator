//
// Created by pbeerkens on 4/29/20.
//

#include <sstream>
#include "ILink3EnumDefinition.h"

std::string ILink3EnumDefinition::structFieldPrefix() const {
    std::stringstream ss;
    ss<<"IL3Enum::"<<name_;
    return ss.str ();
}

std::string ILink3EnumDefinition::getFunctionReturnType() const {
    return structFieldPrefix (); //same as the structFieldPrefix
}

std::string ILink3EnumDefinition::getFunctionImpl(const std::string &fieldName) const {
    std::stringstream ss;
    ss<<"return blockData_->"<<fieldName<<";";
    return ss.str ();
}

std::string ILink3EnumDefinition::setFunctionImpl([[maybe_unused]] const std::string &fieldName) const {
    std::stringstream ss;
    ss<<"blockDataWrite_."<<fieldName<<"=v;";
    return ss.str ();
}
