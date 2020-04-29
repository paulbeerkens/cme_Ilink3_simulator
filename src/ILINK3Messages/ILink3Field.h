//
// Created by pbeerkens on 4/29/20.
//

#ifndef CMESIMULATOR_ILINK3FIELD_H
#define CMESIMULATOR_ILINK3FIELD_H

class ILInk3BaseField  {
public:
};

template <typedef BaseType>
class ILink3Field {
    bool readFromBuffer (MessageBuffer& msgBuffer);
protected:
    BaseType data_;
};


#endif //CMESIMULATOR_ILINK3FIELD_H
