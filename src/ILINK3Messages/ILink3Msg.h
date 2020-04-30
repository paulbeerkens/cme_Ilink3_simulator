//
// Created by pbeerkens on 4/27/20.
//

#ifndef CMESIMULATOR_ILINK3MSG_H
#define CMESIMULATOR_ILINK3MSG_H

#include <iostream>
#include <array>
#include "MessageBuffer.h"
#include <Ilink3EnumsGen.h>


class ILink3Msg {
public:

};

struct Data {
    std::uint16_t length {0};
    char* varData;
};

class NegotiateMsg {
public:
    inline static const std::size_t id {500};

    std::string_view getCustomerFlow () const {static std::string CustomerFlow ("IDEMPOTENT"); return CustomerFlow;}
    std::string_view HMACVersion () const {static std::string HMACVersion ("CME-1-SHA-256"); return HMACVersion;}
    std::string_view getHMACSignature () const {return std::string_view (&blockData_->HMACSignature [0],32);};
    std::string_view getAccessKeyID () const {return std::string_view (&blockData_->AccessKeyID [0],20);};
    std::string_view getSession () const {return std::string_view (&blockData_->Session [0],3);};
    std::string_view getFirm () const {return std::string_view (&blockData_->Firm [0],5);};
    std::uint64_t getUUID () const {return blockData_->UUID;}
    std::uint64_t getRequestTimestamp () const {return blockData_->RequestTimestamp;}

    void readFromBuffer (MessageBuffer &msgBuffer);
protected:
    struct __attribute__ ((packed))  BlockData {
        std::array<char, 32> HMACSignature;
        std::array<char, 20> AccessKeyID;
        std::uint64_t UUID;
        std::uint64_t RequestTimestamp;
        std::array<char, 3> Session;
        std::array<char, 5> Firm;
    };

    const BlockData* blockData_ {nullptr};

    Data data;
};

//enum class FTI: std::uint8_t {Backup=0, Primary=1, Null=255};
//enum class SplitMsgE: std::uint8_t {SplitMessageDelayed=0, OutofOrderMessageDelayed=1, CompleteMessageDelayed=2,Null=255};

class NegotiationResponseMsg {
public:
    inline static const std::size_t id {500};

protected:
    struct __attribute__ ((packed))  BlockData {
        std::uint64_t UUID;
        std::uint64_t RequestTimestamp;
        std::uint16_t SecretKeySecureIDExpiration {65535};
        IL3Enum::FTI FaultToleranceIndicator {IL3Enum::FTI::Null};
        IL3Enum::SplitMsg SplitMsg {IL3Enum::SplitMsg::Null};
        std::uint32_t PreviousSeqNo;
        std::uint64_t PreviousUUID;
        std::uint16_t length {0};
    };

    const BlockData* blockData_;

    Data data;
};

class NegotiationResponseMsgOut: public NegotiationResponseMsg {
public:
    NegotiationResponseMsgOut () {
        blockData_=&blockDataWrite;
    };

    void writeToBuffer (MessageBuffer &msgBuffer);
protected:
    BlockData blockDataWrite;
};



#endif //CMESIMULATOR_ILINK3MSG_H
