//
// Created by pbeerkens on 4/23/20.
//

#include <future>
#include <assert.h>
#include "FIXPConnection.h"
#include <Logger/LogMacros.h>
#include <sys/socket.h>
#include <ILINK3Messages/ILink3Headers.h>
#include <string.h>

#define LOGID "[ConId: "<<connectionId_<<"] "

//todo start a thread
//read the Simple Open Framing Header (contains size)
//Read the entire message
//process the message
//callback
//unittest this
//https://www.cmegroup.com/confluence/display/EPICSANDBOX/iLink+3+-+Simple+Binary+Encoding#space-menu-link-content
FIXPConnection::FIXPConnection(std::int32_t socket, const std::string& remoteHost)
:socket_ (socket)
,remoteHost_ (remoteHost){
    connectionId_=nextFreeConnectionId_++;

    std::promise <void> threadStartedPromise_;
    threadPtr_=std::make_unique <std::thread> ([this, &threadStartedPromise_] () {
        LOGINFO ("Starting connection thread for connection ID:"<<this->connectionId_);
        threadStartedPromise_.set_value();
        this->processMessages ();
        LOGINFO ("Finished connection thread for connection ID:"<<this->connectionId_);
    });

    threadStartedPromise_.get_future().wait(); //wait till thread has been started;
}

void FIXPConnection::processMessages() {
    while (!requestedToTerminate_) {
        SOFH header;

        if (!readN (reinterpret_cast<char*> (&header), sizeof (header))) {
            LOGWARN(LOGID<<"Failed to read Simple Open Framing Header.");
            break;
        }

        if (header.enodingType_!=0xcafe) {
            //TODO report this error to a central bad message repository which will be shown on a web page for debugging purposes
            LOGERROR (LOGID<<"Encoding type in header is not recognized. Not a valid message. Received "<<std::showbase<<std::hex<<header.enodingType_);
            break;
        }

        //TODO write a message buffer to receive the message in.
        //TODO ask the message factory to create the message from the buffer.

        //Something lke this?
        // auto c = std::make_shared<MyClass>("Test");
        // std::thread([c](){
        //        c->doWork ();
        //    }).detach();
        //    std::cout << "After async" << std::endl;



        //asio https://www.boost.org/doc/libs/1_53_0/doc/html/boost_asio/example/chat/chat_server.cpp

        std::cout<<"Length: "<<header.msgSize_<<" Encoding: "<<header.enodingType_<<std::endl;
    }
}

void FIXPConnection::stop() {
    requestedToTerminate_=true;
    if (socket_!=-1) {
        ::shutdown (socket_, SHUT_RDWR); //send the FIN. This makes out code come out of recv calls
        ::close (socket_);
        socket_=-1;
    }

    if (threadPtr_) {
        if (threadPtr_->joinable()) {
            LOGINFO ("Waiting for thread for connection with ID:"<<connectionId_<<" to stop.");
            threadPtr_->join ();
        }
        threadPtr_=nullptr;
    }
}

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
