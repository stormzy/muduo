#ifndef MUDUO_RTP_RTPHEADER_H
#define MUDUO_RTP_RTPHEADER_H

#include <cstdint>
#include <vector>

class RtpHeader {
public:
#if __BYTE_ORDER == __BIG_ENDIAN
    //版本号,固定为2
    uint32_t version: 2;
    //havePadding
    uint32_t havePadding: 1;
    //扩展
    uint32_t haveExtension: 1;
    //csrcCount
    uint32_t csrcCount: 4;
    //mark
    uint32_t mark: 1;
    //负载类型
    uint32_t pt: 7;
#else
    //csrcCount
    uint32_t csrcCount: 4;
    //扩展
    uint32_t haveExtension: 1;
    //havePadding
    uint32_t havePadding: 1; // rtp packet size is multiple of 4
    //版本号,固定为2
    uint32_t version: 2;
    //负载类型
    uint32_t pt: 7;
    //mark finish of one frame
    uint32_t mark: 1;
#endif

    uint16_t seq;
    uint32_t timeStamp;
    uint32_t ssrc;
    //负载,如果有csrc和ext,前面为 4 * csrcCount + (4 + 4 * ext_len)
    // Note: payloadSize is not a member of RtpHeader !!!
    // uint8_t defaultPayloadMemoryPlaceholder;

} __attribute__((packed));

#endif // MUDUO_RTP_RTPHEADER_H