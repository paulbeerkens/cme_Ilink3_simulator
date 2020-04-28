//
// Created by pbeerkens on 4/28/20.
//

#include "ILink3MsgDefinition.h"

std::ostream &operator<<(std::ostream &os, ILink3MsgDefinition::ID id) {
    os << static_cast <std::size_t> (id);
    return os;
}
