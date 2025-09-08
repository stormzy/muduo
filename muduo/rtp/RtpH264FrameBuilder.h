#ifndef MUDUO_RTP_RTPH264FRAMEBUILDER
#define MUDUO_RTP_RTPH264FRAMEBUILDER

#include "RtpPacket.h"
#include <list>

class RtpH264FrameBuilder 
{
public:
    RtpH264FrameBuilder();
    ~RtpH264FrameBuilder();

    void processPacket(RtpPacket::Ptr packet);

private:
    bool started_;
    uint16_t nextSeq_;
    std::list<RtpPacket::Ptr> packets_;
};

#endif // MUDUO_RTP_RTPH264FRAMEBUILDER