//
// Created by pbeerkens on 4/26/20.
//

#include <cstdlib>
#include <stdint.h>
#include <cstdint>
#include <cstring>
#include <cerrno>
#include "MessageBuffer.h"
#include <unistd.h>
#include <assert.h>
#include <string>
#include <Logger/LogMacros.h>

MessageBuffer::MessageBuffer(std::size_t size) {
    buf_= reinterpret_cast<char*>(malloc (size));
    bufSize_=buf_?size:0;
    bad_=(buf_==nullptr);
    wrdPtr_=rdPtr_=buf_;
}

MessageBuffer::~MessageBuffer() {
    free (buf_);
    buf_= nullptr;
}

void MessageBuffer::expandIfRequired(std::size_t size) {
    if (size>bufSize_) {
        buf_= reinterpret_cast<char*>(realloc (reinterpret_cast<void*>(buf_), size));
        bufSize_=buf_?size:0;
        bad_=(buf_==nullptr);
    }
}

void MessageBuffer::reset() {
    wrdPtr_=rdPtr_=buf_;
    bad_=false;
}

void MessageBuffer::write(void *buf, std::size_t size) {
    auto sizeLeft= getWrtBufFree();

    if (size<=sizeLeft) {
        memcpy (wrdPtr_, buf, size);
        wrdPtr_+=size;
    } else {
        bad_=true;
    }
}

bool MessageBuffer::readFromSocket(std::int32_t socket, std::size_t bytesToRead) {
    if (bytesToRead>getWrtBufFree()) {
        bad_=true;
        return false;
    }

    decltype(bytesToRead) bytesRead=0;

    while (bytesRead < bytesToRead) {
        auto n = ::read(socket, wrdPtr_, bytesToRead - bytesRead);
        if (n>0) { //received data OK
            bytesRead+=n;
            wrdPtr_+=n;
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
    return false;
}




