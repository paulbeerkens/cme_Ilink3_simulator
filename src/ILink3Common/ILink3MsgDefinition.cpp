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
    }
    return "";
}

std::string ILink3FieldInMsgDefinition::getFunctionImpl(const std::string& fieldName) const {
    if (fieldDefinition_) {
        return fieldDefinition_->getFunctionImpl (fieldName);
    } else if (enumDefinition_) {
        return enumDefinition_->getFunctionImpl (fieldName);
    }
    return "";
}

std::string ILink3FieldInMsgDefinition::setFunctionImpl(const std::string &fieldName) const {
    if (fieldDefinition_) {
        return fieldDefinition_->setFunctionImpl (fieldName);
    } else if (enumDefinition_) {
        return enumDefinition_->setFunctionImpl (fieldName);
    }
    return "";
}
