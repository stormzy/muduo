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
  using ConnectionCallback = std::function<void(const UdpSession::Ptr&)>;
  using MessageCallback = std::function<void(const UdpSession::Ptr&, Buffer*)>;
  using CloseCallback = std::function<void(const UdpSession::Ptr&)>;
  UdpSession(EventLoop* loop,
             const string& name,
             int sockfd,
             const InetAddress& localAddr,
             const InetAddress& peerAddr);

  void send(const void* data, int len);
  void send(const StringPiece& message);
  void send(Buffer* message); 

  void setConnectionCallback(const ConnectionCallback& cb)
  { connectionCallback_ = cb; }

  void setMessageCallback(const MessageCallback& cb)
  { messageCallback_ = cb; }

  void setWriteCompleteCallback(const WriteCompleteCallback& cb)
  { writeCompleteCallback_ = cb; }

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
  ConnectionCallback connectionCallback_;
  MessageCallback messageCallback_;
  WriteCompleteCallback writeCompleteCallback_;
  CloseCallback closeCallback_;
  Buffer inputBuffer_;
  Buffer outputBuffer_; // FIXME: use list<Buffer> as output buffer.
};

} // namespace net
} // namespace muduo

#endif //MUDUO_NET_UDPSESSION_H