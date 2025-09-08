#ifndef MUDUO_RTP_RTPSERVER_H
#define MUDUO_RTP_RTPSERVER_H

#include <map>
#include <string>
#include <memory>
#include "RtpSession.h"

class RtpServer {
public:
    RtpServer(bool isUdp);
    ~RtpServer();

private:
    bool isUdp_;
    std::map<uint32_t, std::shared_ptr<RtpSession> > sessions_;
};

#endif // MUDUO_RTP_RTPSERVER_H