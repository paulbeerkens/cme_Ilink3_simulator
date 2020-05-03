//
// Created by pbeerkens on 4/26/20.
//

#ifndef CMESIMULATOR_MESSAGEBUFFER_H
#define CMESIMULATOR_MESSAGEBUFFER_H


#include <cstddef>

class MessageBuffer {
public:
    explicit MessageBuffer (std::size_t size);
    ~MessageBuffer();

    void expandIfRequired (std::size_t size); //expands buffer if required. Will not shrink the buffer
    void reset (); //sets read and write buffer to beginning

    std::size_t getBufSize () const {return bufSize_;}

    void write (void* buf, std::size_t);

    bool bad () const {return bad_;}

    inline size_t getWrtBufFree () const { return bufSize_ - uintptr_t (wrdPtr_ - buf_); }
    inline size_t getLeftToRead () const { return uintptr_t (wrdPtr_ - rdPtr_); }

    bool readFromSocket (std::int32_t socket, std::size_t size);

    [[nodiscard]] const char* getRdPtr () const {return rdPtr_;};
    void moveRdPtr (std::size_t bytes);
protected:
    char* buf_    {nullptr};
    char* rdPtr_  {nullptr};
    char* wrdPtr_ {nullptr};
    bool bad_     {false}; //if true then the buffer indicated an error at some point
    std::size_t bufSize_ {0};

};


#endif //CMESIMULATOR_MESSAGEBUFFER_H
