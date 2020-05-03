//
// Created by pbeerkens on 4/23/20.
//

#include <future>
#include <assert.h>
#include "FIXPConnection.h"



#include <string.h>







//https://www.cmegroup.com/confluence/display/EPICSANDBOX/iLink+3+-+Simple+Binary+Encoding#space-menu-link-content











/*
bool FIXPConnection::readN(char* buf, std::size_t bytesToRead) {
    decltype(bytesToRead) bytesRead=0;

    while (bytesRead < bytesToRead) {
        auto n = ::read(socket_, buf+bytesRead, bytesToRead - bytesRead);
        if (n>0) { //received data OK
            bytesRead+=n;
        } else if (n<0) { //received an error
            if (errno == EAGAIN || errno == EINTR) continue; //these are acceptable reasons why read failed. Try again.
            return false;
        } else {
            assert (n == 0);

            LOGINFO ("EOF while reading from socket with error code " + std::to_string(errno) + " (" + strerror(errno) + ")");
                return false; //connection was closed
        }
    }

    return bytesRead == bytesToRead;
}
*/