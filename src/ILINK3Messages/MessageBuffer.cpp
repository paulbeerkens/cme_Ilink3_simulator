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
    wrtPtr_= rdPtr_=buf_;
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
    wrtPtr_= rdPtr_=buf_;
    bad_=false;
}

void MessageBuffer::write(void *buf, std::size_t size) {
    auto sizeLeft= getWrtBufFree();

    if (size<=sizeLeft) {
        memcpy (wrtPtr_, buf, size);
        wrtPtr_+=size;
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
        auto n = ::read(socket, wrtPtr_, bytesToRead - bytesRead);
        if (n>0) { //received data OK
            bytesRead+=n;
            wrtPtr_+=n;
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

bool MessageBuffer::sendToSocket(std::int32_t socket) {
    //std::uintptr_t bytesToWrite=wrtPtr_-buf_+1;
    auto localWrtPtr=buf_;

    while (localWrtPtr<wrtPtr_) {
      auto n=::write (socket, localWrtPtr, wrtPtr_-localWrtPtr); //wrtPtr_ is the location of first free byte so not +1 here if wrPtr_==buf_ then 0 bytes to be written.
      if (n>0) {
          localWrtPtr += n;
      } else if (n<0) { //received an error
          if (errno == EINTR) continue; //acceptable reason why write failed. Try again.
          LOGINFO ("Error while writing to socket with error code " + std::to_string(errno) + " (" + strerror(errno) + ")");
          return false;
      } else {
          assert (n == 0);

          LOGINFO ("EOF while writing to socket with error code " + std::to_string(errno) + " (" + strerror(errno) + ")");
          return false; //connection was closed
      }
    }
    return true;
}


void MessageBuffer::moveRdPtr(std::size_t bytes) {
    rdPtr_+=bytes;
    if (rdPtr_ > wrtPtr_) {
        bad_=true;
        rdPtr_-=bytes;
    }
}

void MessageBuffer::moveWrtPtr(std::size_t bytes) {
    wrtPtr_+=bytes;
    if (wrtPtr_ > buf_+ bufSize_) {
        bad_=true;
        wrtPtr_-=bytes;
    }
}







