//
// Created by pbeerkens on 4/28/20.
//

#ifndef CMESIMULATOR_ILINK3MSGDEFINITION_H
#define CMESIMULATOR_ILINK3MSGDEFINITION_H

#include <string>
#include <ostream>


struct ILink3MsgDefinition {
    enum class ID:std::size_t  {None=0};


    std::string name_;
    std::string description_;
    std::size_t blockLength_ {0};
    ID id_ {0};
};

std::ostream& operator<< (std::ostream& os, ILink3MsgDefinition::ID id);

#endif //CMESIMULATOR_ILINK3MSGDEFINITION_H
