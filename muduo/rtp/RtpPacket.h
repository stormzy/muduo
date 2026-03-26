#ifndef MUDUO_RTP_RTPPACKET_H
#define MUDUO_RTP_RTPPACKET_H

#include "RtpHeader.h"
#include "Buffer.h"
#include <Timestamp.h>

using muduo::net::Buffer;
using muduo::Timestamp;

class RtpPacket {
public:
    using Ptr = std::shared_ptr<RtpPacket>;

    RtpPacket() = default;
    ~RtpPacket() = default;

    RtpPacket(Buffer::Ptr buffer);

    uint16_t seq() { return header_.seq; }
    uint32_t timestamp() { return header_.timestamp; }
    uint32_t ssrc() { return header_.ssrc; }

private:
    bool parse(Buffer::Ptr buffer);

public:
    Timestamp receiveTime_;

    RtpHeader header_;
    Buffer::Ptr buffer_; // include header
    size_t payloadLength_;
};


#endif // MUDUO_RTP_RTPPACKET_H