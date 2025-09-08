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

    const char* p = buf->peek();
    RtpHeader *rtp_header = (RtpHeader *)p;
    
    RtpPacket::Ptr packet = std::make_shared<RtpPacket>();
    packet->seq_ = rtp_header->seq;
    packet->buffer_->swap(*buf);
    packet->receiveTime_ = receiveTime;

    jitterBuffer_.inputRtpPacket(packet);
}