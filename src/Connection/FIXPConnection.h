//
// Created by pbeerkens on 4/23/20.
//

#ifndef CMESIMULATOR_FIXPCONNECTION_H
#define CMESIMULATOR_FIXPCONNECTION_H


#include <cstdint>

class FIXPConnection {
public:
    FIXPConnection (std::int32_t socket)
    :socket_ (socket) {};

    ~FIXPConnection() {
//        ::shutdown(socket_, SHUT_WR); //send the FIN to let the client know that we are done
  //      ::close (socket_);
    }
protected:
    std::int32_t socket_;


};


#endif //CMESIMULATOR_FIXPCONNECTION_H
