#ifndef MUDUO_RTP_RTPHEADER_H
#define MUDUO_RTP_RTPHEADER_H

#include <cstdint>
#include <vector>

/** rfc url: https://www.rfc-editor.org/rfc/rfc1889.html#section-5.1
 * 
 * The RTP header has the following format:

    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |V=2|P|X|  CC   |M|     PT      |       sequence number         |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                           timestamp                           |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |           synchronization source (SSRC) identifier            |
   +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
   |            contributing source (CSRC) identifiers             |
   |                             ....                              |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */

class RtpHeader {
public:
    // 最小的RTP头部大小（没有CSRC）
    static constexpr size_t kMinHeaderSize = 12;

    size_t length() {
        return kMinHeaderSize + csrcs.size();
    }

    void clear() {
        version = 2;
        has_padding = false;
        has_extension = false;
        csrc_count = 0;
        marker = false;
        pt = 0;
        seq = 0;
        timestamp = 0;
        ssrc = 0;

        std::vector<uint32_t> tmp;
        csrcs.swap(tmp);
    }

public:
    uint8_t version = 2;         // 2 bits, RFC规定通常为2
    bool has_padding = false;    // 1 bit
    bool has_extension = false;  // 1 bit
    uint8_t csrc_count = 0;      // 4 bits
    bool marker = false;         // 1 bit
    uint8_t pt = 0;    // 7 bits

    uint16_t seq = 0;
    uint32_t timestamp = 0;
    uint32_t ssrc = 0;

    //负载,如果有csrc和ext,前面为 4 * csrcCount + (4 + 4 * ext_len)
    // Note: payloadSize is not a member of RtpHeader !!!
    std::vector<uint32_t> csrcs; // most 15 ele
} __attribute__((packed));

union aaa
{
    uint16_t a: 1;

    /* data */
};


#endif // MUDUO_RTP_RTPHEADER_H