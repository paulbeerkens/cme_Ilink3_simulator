//
// Created by pbeerkens on 5/6/20.
//

#include <sstream>
#include "ILink3MsgDefinition.h"

std::string ILink3FieldInMsgDefinition::getFunctionReturnType() const {
    if (fieldDefinition_) {
        return fieldDefinition_->getFunctionReturnType ();
    } else if (enumDefinition_) {
        return enumDefinition_->getFunctionReturnType ();
    } else if (compositeDefinition_.has_value()) {
        std::stringstream ss;
        //ss<<"const IL3Composite::"<<*compositeDefinition_<<"&";
        ss<<"IL3Composite::"<<*compositeDefinition_<<"";
        return ss.str ();
    }
    return "";
}

std::string ILink3FieldInMsgDefinition::getFunctionImpl(const std::string& fieldName) const {
    if (fieldDefinition_) {
        return fieldDefinition_->getFunctionImpl (fieldName);
    } else if (enumDefinition_) {
        return enumDefinition_->getFunctionImpl (fieldName);
    } else if (compositeDefinition_.has_value()) {
        //I am returning by value because all composite types are less than 64 bytes so far
        //so same as returning a pointer (reference).
        std::stringstream ss;
        ss<<"return blockData_->"<<fieldName<<";";
        return ss.str ();
    }
    return "";
}

std::string ILink3FieldInMsgDefinition::setFunctionImpl(const std::string &fieldName) const {
    if (fieldDefinition_) {
        return fieldDefinition_->setFunctionImpl (fieldName);
    } else if (enumDefinition_) {
        return enumDefinition_->setFunctionImpl (fieldName);
    } else if (compositeDefinition_.has_value()) {
        std::stringstream ss;
        ss<<"blockDataWrite_."<<fieldName<<"=v;";
        return ss.str ();
    }
    return "";
}
