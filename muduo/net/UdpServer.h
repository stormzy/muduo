#ifndef MUDUO_NET_UDPSERVER_H
#define MUDUO_NET_UDPSERVER_H

#include <muduo/base/Types.h>
#include <muduo/base/Atomic.h>
#include <muduo/net/UdpSession.h>

namespace muduo
{
namespace net
{

class Acceptor;
class EventLoop;
class EventLoopThreadPool;

class UdpServer : public noncopyable
{
  public:
    UdpServer(EventLoop* loop,
            const InetAddress& listenAddr,
            const string& nameArg);
    ~UdpServer();  // force out-line dtor, for std::unique_ptr members.

  private:
    void newConnection(int sockfd, const InetAddress&, std::vector<char>&);

  private:
    EventLoop* loop_; // bind socket loop
    const string ipPort_;
    const string name_;
    InetAddress listenAddr_;
    std::unique_ptr<Acceptor> acceptor_; // avoid revealing Acceptor
    std::shared_ptr<EventLoopThreadPool> threadPool_;

};

} // namespace net
} // namespace muduo

#endif // MUDUO_NET_UDPSERVER_H
