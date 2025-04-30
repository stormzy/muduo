#ifndef MUDUO_NET_UDPSERVER_H
#define MUDUO_NET_UDPSERVER_H

#include <muduo/base/Types.h>
#include <muduo/base/Atomic.h>
#include <muduo/net/UdpConnection.h>

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
    using ThreadInitCallback = std::function<void(EventLoop*)>;
    UdpServer(EventLoop* loop,
            const InetAddress& listenAddr,
            const string& nameArg);
    ~UdpServer();  // force out-line dtor, for std::unique_ptr members.

    void start();
    void setThreadNum(int numThreads);
    void setThreadInitCallback(const ThreadInitCallback& cb)
    { threadInitCallback_ = cb; }
    /// valid after calling start()
    std::shared_ptr<EventLoopThreadPool> threadPool()
    { return threadPool_; }

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
    void removeConnection();

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
    ThreadInitCallback threadInitCallback_;
    AtomicInt32 started_;

    // always in loop thread
    int nextConnId_;
    std::map<string, UdpConnection::Ptr> connections_;
};

} // namespace net
} // namespace muduo

#endif // MUDUO_NET_UDPSERVER_H
