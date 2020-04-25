//
// Created by pbeerkens on 4/25/20.
//

#ifndef CMESIMULATOR_ICONNECTIONCB_H
#define CMESIMULATOR_ICONNECTIONCB_H

#include <memory>

class FIXPConnection;

class IConnectionCB {
public:
    virtual void connectionEnd (std::shared_ptr<FIXPConnection> connection)=0;
};

#endif //CMESIMULATOR_ICONNECTIONCB_H
