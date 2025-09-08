#include "RtpJitterBuffer.h"
#include "RtpHeader.h"

RtpJitterBuffer::RtpJitterBuffer(const RtpSortedPacketCallBack& sortedPacketCallBack, uint32_t timeoutMs)
 :  started_(false),
    nextSeq_(0),
    timeout_(timeoutMs),
    startWaitTimestamp_(-1),
    sortedPacketCallBack_(sortedPacketCallBack)
{

}

void RtpJitterBuffer::inputRtpPacket(RtpPacket::Ptr packet)
{
    uint16_t seq = packet->seq_;

    if (!started_) {
        started_ = true;
        nextSeq_ = seq;
    }

    if (startWaitTimestamp_ != -1) {
        int64_t elapsed_ms = Timestamp::now().milliSecondsSinceEpoch() - startWaitTimestamp_;
        if (elapsed_ms > timeout_) {
            assert(!packets_.empty());
            nextSeq_ = packets_.begin()->first;
            startWaitTimestamp_ = -1;
        }
    }

    bool shouldDropPacket = false;
    if (seq < nextSeq_) {
        if (nextSeq_ < seq + 1024) {
            shouldDropPacket = true;
        }
    } else if (nextSeq_ && seq - nextSeq_ > (0xFFFF >> 1)){
        shouldDropPacket = true;
    }

    if (shouldDropPacket) {
        return;
    }
    
    packets_[seq] = packet;

    // pop sorted rtp packet
    auto nextIt = packets_.find(nextSeq_);
    while (!packets_.empty() && packets_.end() != nextIt) 
    {
        uint16_t seq = nextIt->first;
        RtpPacket::Ptr packet = nextIt->second;
        if (sortedPacketCallBack_) {
            sortedPacketCallBack_(packet);
        }
        packets_.erase(nextIt);
        nextSeq_ = seq + 1;
        nextIt = packets_.find(nextSeq_);
    }

    if (packets_.empty()) {
        startWaitTimestamp_ = -1;
    }

    if (!packets_.empty() && startWaitTimestamp_ == -1) {
        startWaitTimestamp_ = packets_.begin()->second->receiveTime_.milliSecondsSinceEpoch();
    }
}

