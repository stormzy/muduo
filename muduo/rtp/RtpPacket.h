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

    uint16_t seq_;
    Timestamp receiveTime_;
    Buffer::Ptr buffer_;
};


#endif // MUDUO_RTP_RTPPACKET_H