#include "RtpDemuxer.h"

void RtpDemuxer::inputRtpPacket(RtpPacket::Ptr packet)
{
    buffer_.ensureWritableBytes(packet->buffer_->readableBytes());

    
    // 1. set this.pt

    // 2. demux by difference pt

    // 3. save payload to buffer_

    // 4. when a frame is over, call the frameCallBack_
}