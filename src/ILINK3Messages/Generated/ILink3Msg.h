//
// Created by pbeerkens on 4/27/20.
//

#ifndef CMESIMULATOR_ILINK3MSG_H
#define CMESIMULATOR_ILINK3MSG_H

#include <iostream>
#include <array>
#include "../MessageBuffer.h"
#include <Generated/ILink3EnumsGen.h>
#include <cstring>


namespace IL3Msg {


struct Data {
    std::uint16_t length{0};
    char *varData;

    void writeToBuffer(MessageBuffer &msgBuffer);
};

class NegotiateMsg {
public:
    inline static const std::size_t id{500};

    //Constant value representing type of flow from customer to CME
    std::string_view getCustomerFlow() const {
        static std::string CustomerFlow("IDEMPOTENT");
        return CustomerFlow;
    }

    //Constant value representing CME HMAC version
    std::string_view HMACVersion() const {
        static std::string HMACVersion("CME-1-SHA-256");
        return HMACVersion;
    }

    //Contains the HMAC signature.
    std::string_view getHMACSignature() const { return std::string_view(&blockData_->HMACSignature[0], 32); };

    //Contains the AccessKeyID assigned to this session on this port.
    std::string_view getAccessKeyID() const { return std::string_view(&blockData_->AccessKeyID[0], 20); };

    //Session ID.
    std::string_view getSession() const { return std::string_view(&blockData_->Session[0], 3); };

    //Firm ID
    std::string_view getFirm() const { return std::string_view(&blockData_->Firm[0], 5); };

    //Session Identifier defined as type long (uInt64); recommended to use timestamp as number of microseconds since epoch (Jan 1, 1970)
    std::uint64_t getUUID() const { return blockData_->UUID; }

    //Time of request; recommended to use timestamp as number of nanoseconds since epoch (Jan 1, 1970)
    std::uint64_t getRequestTimestamp() const { return blockData_->RequestTimestamp; }

    void readFromBuffer(MessageBuffer &msgBuffer);

protected:
    struct __attribute__ ((packed))  BlockData {
        std::array<char, 32> HMACSignature;
        std::array<char, 20> AccessKeyID;
        std::uint64_t UUID;
        std::uint64_t RequestTimestamp;
        std::array<char, 3> Session;
        std::array<char, 5> Firm;
    };

    const BlockData *blockData_{nullptr};

    Data data;
};

class NegotiationResponseMsg {
public:
    inline static const std::size_t id{501};

protected:
    struct __attribute__ ((packed))  BlockData {
        std::uint64_t UUID;
        std::uint64_t RequestTimestamp;
        std::uint16_t SecretKeySecureIDExpiration {65535};
        IL3Enum::FTI FaultToleranceIndicator {IL3Enum::FTI::Null};
        IL3Enum::SplitMsg SplitMsg {IL3Enum::SplitMsg::Null};
        std::uint32_t PreviousSeqNo;
        std::uint64_t PreviousUUID;
    };

    const BlockData *blockData_;

    Data data;
};

class NegotiationResponseMsgOut : public NegotiationResponseMsg {
public:
    NegotiationResponseMsgOut() {
        blockData_ = &blockDataWrite;
    };

    void setUUID (std::uint64_t uuid) {blockDataWrite.UUID=uuid;}

    void setRequestTimestamp (std::uint64_t ts) {blockDataWrite.RequestTimestamp=ts;}

    void setSecretKeySecureIDExpiration (std::uint16_t v) {blockDataWrite.SecretKeySecureIDExpiration=v;}

    void setFaultToleranceIndicator (IL3Enum::FTI fti) {blockDataWrite.FaultToleranceIndicator=fti;}

    void setSplitMsg (IL3Enum::SplitMsg splitMsg) {blockDataWrite.SplitMsg=splitMsg;}

    void setPreviousSeqNo (std::uint32_t seqno) {blockDataWrite.PreviousSeqNo=seqno;}

    void setPreviousUUID (std::uint64_t prevUUid) {blockDataWrite.PreviousUUID=prevUUid;}

    void writeToBuffer(MessageBuffer &msgBuffer);

protected:
    BlockData blockDataWrite;
};

} //end of namespace IL3Msg

#endif //CMESIMULATOR_ILINK3MSG_H
