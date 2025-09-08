#ifndef MUDUO_RTP_RTPJITTERBUFFER
#define MUDUO_RTP_RTPJITTERBUFFER

#include <stdint.h>
#include <map>
#include <Buffer.h>
#include <string.h>
#include <functional>
#include <Timestamp.h>
#include "RtpPacket.h"

using muduo::net::Buffer;
using muduo::Timestamp;

class RtpJitterBuffer {
public:
    using RtpSortedPacketCallBack = std::function<void(RtpPacket::Ptr)>;
    RtpJitterBuffer(const RtpSortedPacketCallBack& sortedPacketCallBack, uint32_t timeoutMs = 100);
    ~RtpJitterBuffer();

    void inputRtpPacket(RtpPacket::Ptr packet);

private:
    bool started_;
    uint16_t nextSeq_;
    uint32_t timeout_; // ms
    int64_t startWaitTimestamp_; // ms
    std::map<uint16_t, RtpPacket::Ptr> packets_; // <seq, rtp packet>
    RtpSortedPacketCallBack sortedPacketCallBack_;
};

#endif // MUDUO_RTP_RTPJITTERBUFFER