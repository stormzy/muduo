#include <muduo/net/UdpSession.h>

#include <muduo/base/Logging.h>
#include <muduo/base/WeakCallback.h>
#include <muduo/net/Channel.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/Socket.h>
#include <muduo/net/SocketsOps.h>

#include <errno.h>

using namespace muduo;
using namespace muduo::net;

UdpSession::UdpSession(EventLoop* loop,
             const string& name,
             int sockfd,
             const InetAddress& localAddr,
             const InetAddress& peerAddr)
  : loop_(loop),
    name_(name),
    reading_(true),
    socket_(new Socket(sockfd)),
    channel_(new Channel(loop, sockfd)),
    localAddr_(localAddr),
    peerAddr_(peerAddr)
{
  channel_->setReadCallback(
      std::bind(&UdpSession::handleRead, this, _1));
  channel_->setWriteCallback(
      std::bind(&UdpSession::handleWrite, this));
  channel_->setCloseCallback(
      std::bind(&UdpSession::handleClose, this));
  channel_->setErrorCallback(
      std::bind(&UdpSession::handleError, this));
  LOG_DEBUG << "UdpSession::ctor[" <<  name_ << "] at " << this
            << " fd=" << sockfd;
}

void UdpSession::sessionEstablished()
{
  loop_->assertInLoopThread();
  channel_->tie(shared_from_this());
  channel_->enableReading();

  connectionCallback_(shared_from_this());
}

void UdpSession::handleRead(Timestamp receiveTime)
{
  loop_->assertInLoopThread();
  int savedErrno = 0;
  ssize_t n = inputBuffer_.readFd(channel_->fd(), &savedErrno);
  if (n > 0)
  {
    messageCallback_(shared_from_this(), &inputBuffer_);
  }
  else if (n == 0)
  {
    handleClose();
  }
  else
  {
    errno = savedErrno;
    LOG_SYSERR << "UdpSession::handleRead";
    handleError();
  }
}

void UdpSession::handleWrite()
{
  loop_->assertInLoopThread();
  if (channel_->isWriting())
  {
    ssize_t n = sockets::write(channel_->fd(),
                               outputBuffer_.peek(),
                               outputBuffer_.readableBytes());
    if (n > 0)
    {
      outputBuffer_.retrieve(n);
      if (outputBuffer_.readableBytes() == 0)
      {
        channel_->disableWriting();
      } 
      else 
      {
        LOG_SYSERR << "UdpSession::handleWrite sent bytes is less than outputBuffer.readableBytes.";
      }
    }
    else
    {
      LOG_SYSERR << "UdpSession::handleWrite sockets::write return: " << n;
    }
  }
  else
  {
    LOG_TRACE << "Connection fd = " << channel_->fd()
              << " is down, no more writing";
  }
}

void UdpSession::handleClose()
{
  loop_->assertInLoopThread();
  LOG_TRACE << "fd = " << channel_->fd();
  // we don't close fd, leave it to dtor, so we can find leaks easily.
  channel_->disableAll();

  UdpSession::Ptr guardThis(shared_from_this());
  // must be the last line
  closeCallback_(guardThis);
}

void UdpSession::handleError()
{
  int err = sockets::getSocketError(channel_->fd());
  LOG_ERROR << "TcpConnection::handleError [" << name_
            << "] - SO_ERROR = " << err << " " << strerror_tl(err);
}

void UdpSession::send(const void* data, int len)
{
  if (loop_->isInLoopThread()) 
  {
    sendInLoop(data, len);
  } 
  else 
  {
    send(StringPiece(static_cast<const char*>(data), len));
  }
}

void UdpSession::send(const StringPiece& message)
{
  if (!message.empty())
  {
    if (loop_->isInLoopThread())
    {
      sendInLoop(message);
    }
    else
    {
      void (UdpSession::*fp)(const StringPiece& message) = &UdpSession::sendInLoop;
      loop_->runInLoop(std::bind(fp, this, string(message)));
    }
  }
}

void UdpSession::send(Buffer* buf)
{
  assert(buf);
  if (buf) 
  {
    if (loop_->isInLoopThread())
    {
      sendInLoop(buf->peek(), buf->readableBytes());
      buf->retrieveAll();
    }
    else
    {
      void (UdpSession::*fp)(const StringPiece& message) = &UdpSession::sendInLoop;
      loop_->runInLoop(std::bind(fp, this, buf->retrieveAllAsString()));
    }
  }
}

void UdpSession::sendInLoop(const StringPiece& message)
{
  sendInLoop(message.data(), message.size());
}

void UdpSession::sendInLoop(const void* data, size_t len)
{
  loop_->assertInLoopThread();
  ssize_t nwrote = 0;
  size_t remaining = len;
  bool faultError = false;
  // if no thing in output queue, try writing directly
  if (!channel_->isWriting() && outputBuffer_.readableBytes() == 0)
  {
    nwrote = sockets::write(channel_->fd(), data, len);
    if (nwrote >= 0)
    {
      remaining = len - nwrote;
      if (remaining > 0)
      {
        LOG_WARN << "UdpSession::sendInLoop sent bytes is less than data len.";
      }
    }
    else // nwrote < 0
    {
      nwrote = 0;
      if (errno != EWOULDBLOCK)
      {
        LOG_SYSERR << "UdpSession::sendInLoop";
        if (errno == EPIPE || errno == ECONNRESET) // FIXME: any others?
        {
          faultError = true;
        }
      }
    }
  }

  assert(remaining <= len);
  if (!faultError && remaining > 0)
  {
    // size_t oldLen = outputBuffer_.readableBytes();
    outputBuffer_.append(static_cast<const char*>(data)+nwrote, remaining);
    if (!channel_->isWriting())
    {
      channel_->enableWriting();
    }
  }
}