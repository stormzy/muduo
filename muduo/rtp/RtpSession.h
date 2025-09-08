#ifndef MUDUO_RTP_RTPSESSION_H
#define MUDUO_RTP_RTPSESSION_H

#include "muduo/net/UdpConnection.h"
#include "RtpJitterBuffer.h"
#include "RtpH264FrameBuilder.h"

using namespace muduo::net;

class RtpSession {
public:
    RtpSession();
    ~RtpSession();

    void onMessage(const UdpConnectionPtr& conn, Buffer* buf, Timestamp receiveTime);

public:
    RtpH264FrameBuilder frameBuilder_;
    RtpJitterBuffer jitterBuffer_;
};

#endif // MUDUO_RTP_RTPSESSION_H