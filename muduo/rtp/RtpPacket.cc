#include "RtpPacket.h"

#ifdef _WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif

RtpPacket::RtpPacket(Buffer::Ptr buffer)
{
    parse(buffer);
}

bool RtpPacket::parse(Buffer::Ptr buffer)
{
    if (buffer->readableBytes() < RtpHeader::kMinHeaderSize) {
        return false;
    }
    const char *p = buffer->peek();

    const uint8_t byte0 = p[0];
    header_.version = (byte0 & 0xC0) >> 6;
    header_.has_padding = (byte0 & 0x20) != 0;
    header_.has_extension = (byte0 & 0x10) != 0;
    header_.csrc_count = byte0 & 0x0F;

    const uint8_t byte1 = p[1];
    header_.marker = (byte1 & 0x80) != 0;
    header_.pt = byte1 & 0x7F;

    uint16_t seq_num_net;
    memcpy(&seq_num_net, &p[2], sizeof(uint16_t));
    header_.seq = ::ntohs(seq_num_net);

    uint32_t timestamp_net;
    memcpy(&timestamp_net, &p[4], sizeof(uint32_t));
    header_.timestamp = ::ntohl(timestamp_net);

    uint32_t ssrc_net;
    memcpy(&ssrc_net, &p[8], sizeof(uint32_t));
    header_.ssrc = ::ntohl(ssrc_net);
    
    size_t header_len = RtpHeader::kMinHeaderSize + header_.csrc_count * sizeof(uint32_t);
    if (buffer->readableBytes() < header_len) {
        return false;
    }
    
    if (header_.csrc_count > 0) {
        header_.csrcs.resize(header_.csrc_count);
        const char* csrc_ptr = p + RtpHeader::kMinHeaderSize;
        for (uint8_t i = 0; i < header_.csrc_count; ++i) {
            uint32_t csrc_net;
            memcpy(&csrc_net, csrc_ptr, sizeof(uint32_t));
            header_.csrcs[i] = ntohl(csrc_net);
            csrc_ptr += sizeof(uint32_t);
        }

        // RFC 3550 Section 5.3.1 定义了扩展头部的格式
        if (header_.has_extension) {
            // ... 解析扩展头的逻辑 ...
        }
    }
    
    const char* payload_ptr = p + header_len;
    size_t payload_len = buffer->readableBytes() - header_len;
    
    // 如果有padding，需要从payload_size_中减去padding的长度
    if (header_.has_padding) {
        if (buffer->readableBytes() - header_len == 0) {
            header_.clear();
            return false; // Padding存在但没有负载，包错误
        }
        const char* payload_ptr = p + header_.length();
        uint8_t padding_length = payload_ptr[payload_len - 1];
        if (payload_len < padding_length) {
            header_.clear();
            return false; // Padding长度大于负载，包错误
        }
        payload_len -= padding_length;
    }
    buffer_ = buffer;
    payloadLength_ = payload_len;

    return true;
}