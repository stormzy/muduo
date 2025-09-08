#include <muduo/net/UdpServer.h>

#include <muduo/base/Atomic.h>
#include <muduo/base/Logging.h>
#include <muduo/base/Thread.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>

#include <utility>

#include <stdio.h>
#include <unistd.h>

using namespace muduo;
using namespace muduo::net;

int numThreads = 0;

class RtpServer {
public:
    RtpServer(EventLoop* loop, const InetAddress& listenAddr) 
    : server_(loop, listenAddr, "EchoServer"),
      oldCounter_(0),
      startTime_(Timestamp::now())
  {
    server_.setConnectionCallback(
        std::bind(&RtpServer::onConnection, this, _1));
    server_.setMessageCallback(
        std::bind(&RtpServer::onMessage, this, _1, _2));
    server_.setThreadNum(numThreads);
  }

  void start()
  {
    LOG_INFO << "starting " << numThreads << " threads.";
    server_.start();
  }

private:
  void onConnection(const UdpConnectionPtr& conn)
  {
    LOG_TRACE << conn->peerAddress().toIpPort() << " -> "
        << conn->localAddress().toIpPort();
  }

  void onMessage(const UdpConnectionPtr& conn, Buffer* buf)
  {
    size_t len = buf->readableBytes();
    transferred_.addAndGet(len);
    receivedMessages_.incrementAndGet();
    conn->send(buf);
  }

  UdpServer server_;
  AtomicInt64 transferred_;
  AtomicInt64 receivedMessages_;
  int64_t oldCounter_;
  Timestamp startTime_;

};

int main(int argc, char* argv[])
{
    LOG_INFO << "pid = " << getpid() << ", tid = " << CurrentThread::tid();
  if (argc > 1)
  {
    numThreads = atoi(argv[1]);
  }
  EventLoop loop;
  InetAddress listenAddr(2025);
  RtpServer server(&loop, listenAddr);

  server.start();

  loop.loop();
  return 0;
}