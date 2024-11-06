#ifndef MUDUO_NET_UDPSERVER_H
#define MUDUO_NET_UDPSERVER_H

#include <muduo/base/Types.h>
#include <muduo/base/Atomic.h>
#include <muduo/net/UdpSession.h>

#include <map>

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

    void setConnectionCallback(const UdpConnectionCallback& cb)
    { connectionCallback_ = cb; }

    /// Set message callback.
    /// Not thread safe.
    void setMessageCallback(const UdpMessageCallback& cb)
    { messageCallback_ = cb; }

    void setCloseCallback(const UdpCloseCallback& cb)
    { closeCallback_ = cb; }

  private:
    void newConnection(int sockfd, const InetAddress&, std::vector<char>&);
    void removeSession();

  private:
    EventLoop* loop_; // bind socket loop
    const string ipPort_;
    const string name_;
    InetAddress listenAddr_;
    std::unique_ptr<Acceptor> acceptor_; // avoid revealing Acceptor
    std::shared_ptr<EventLoopThreadPool> threadPool_;
    UdpConnectionCallback connectionCallback_;
    UdpMessageCallback messageCallback_;
    UdpCloseCallback closeCallback_;
    // always in loop thread
    int nextSessionId_;
    std::map<string, UdpSession::Ptr> sessions_;
};

} // namespace net
} // namespace muduo

#endif // MUDUO_NET_UDPSERVER_H
