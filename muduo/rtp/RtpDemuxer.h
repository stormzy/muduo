#ifndef MUDUO_RTP_RTPDEMUXER_H
#define MUDUO_RTP_RTPDEMUXER_H

#include "RtpPacket.h"

class RtpDemuxer {
public:
    void inputRtpPacket(RtpPacket::Ptr packet);

public:
    uint8_t pt_;
    Buffer buffer_;
};

#endif // MUDUO_RTP_RTPDEMUXER_H