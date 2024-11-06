#ifndef MUDUO_NET_UDPSESSION_H
#define MUDUO_NET_UDPSESSION_H

#include <muduo/base/noncopyable.h>
#include <muduo/base/StringPiece.h>
#include <muduo/base/Types.h>
#include <muduo/net/Callbacks.h>
#include <muduo/net/Buffer.h>
#include <muduo/net/InetAddress.h>

#include <memory>

namespace muduo
{
namespace net
{

class Channel;
class EventLoop;
class Socket;

class UdpSession :  noncopyable,
                    public std::enable_shared_from_this<UdpSession>
{
public:
  using Ptr = std::shared_ptr<UdpSession>;
  UdpSession(EventLoop* loop,
             const string& name,
             int sockfd,
             const InetAddress& localAddr,
             const InetAddress& peerAddr);

  string name() const { return name_; }
  const InetAddress& localAddress() const { return localAddr_; }
  const InetAddress& peerAddress() const { return peerAddr_; }

  void forceClose();
  void forceCloseWithDelay(double seconds);

  void send(const void* data, int len);
  void send(const StringPiece& message);
  void send(Buffer* message); 

  void setConnectionCallback(const UdpConnectionCallback& cb)
  { connectionCallback_ = cb; }

  void setMessageCallback(const UdpMessageCallback& cb)
  { messageCallback_ = cb; }

  /// Internal use only.
  void setCloseCallback(const UdpCloseCallback& cb)
  { closeCallback_ = cb; }

  void sessionEstablished();
private:
  void handleRead(Timestamp receiveTime);
  void handleWrite();
  void handleClose();
  void handleError();
  void sendInLoop(const StringPiece& message);
  void sendInLoop(const void* data, size_t len);

private:
  EventLoop* loop_;
  const string name_;
  bool reading_;
  // we don't expose those classes to client.
  std::unique_ptr<Socket> socket_;
  std::unique_ptr<Channel> channel_;
  const InetAddress localAddr_;
  const InetAddress peerAddr_;
  UdpConnectionCallback connectionCallback_;
  UdpMessageCallback messageCallback_;
  UdpCloseCallback closeCallback_;
  Buffer inputBuffer_;
  Buffer outputBuffer_; // FIXME: use list<Buffer> as output buffer.
};

} // namespace net
} // namespace muduo

#endif //MUDUO_NET_UDPSESSION_H