//
// Created by pbeerkens on 4/23/20.
//

#ifndef CMESIMULATOR_FIXPCONNECTION_H
#define CMESIMULATOR_FIXPCONNECTION_H

#include <cstdint>
#include <string>
#include "IConnectionCB.h"

class FIXPConnection: public std::enable_shared_from_this<FIXPConnection> {
public:
    FIXPConnection (std::int32_t socket, const std::string& remoteHost, IConnectionCB& cb);

    ~FIXPConnection() {
        stop ();

    }

    void stop ();

    void processMessages ();

    uint32_t getConnectionId() const {return connectionId_;}

protected:

    std::int32_t socket_;
    const std::string remoteHost_;

    inline static std::atomic <std::uint32_t> nextFreeConnectionId_ {1};
    std::uint32_t connectionId_;

    //std::unique_ptr <std::thread> threadPtr_;
    std::atomic <bool> requestedToTerminate_ {false};

    IConnectionCB& cb_;

    //bool readN (char* buf, std::size_t bytesToRead);
};


#endif //CMESIMULATOR_FIXPCONNECTION_H
