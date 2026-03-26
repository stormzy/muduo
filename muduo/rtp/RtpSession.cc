#include "RtpSession.h"

RtpSession::RtpSession() : 
    frameBuilder_(),
    jitterBuffer_(std::bind(&RtpH264FrameBuilder::processPacket, &frameBuilder_, std::placeholders::_1))
{

}

RtpSession::~RtpSession()
{

}

void RtpSession::onMessage(const UdpConnectionPtr& conn, Buffer* buf, Timestamp receiveTime)
{
    assert(buf);
    if (buf->readableBytes() < 12) {
        return;
    }

    Buffer::Ptr buffer = std::make_shared<Buffer>(1500);
    buffer->swap(*buf);

    RtpPacket::Ptr packet = std::make_shared<RtpPacket>(buffer);
    packet->receiveTime_ = receiveTime;

    jitterBuffer_.inputRtpPacket(packet);
}