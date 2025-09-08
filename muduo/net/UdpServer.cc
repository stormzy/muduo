#include <muduo/net/UdpServer.h>

#include <muduo/base/Logging.h>
#include <muduo/net/Acceptor.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/EventLoopThreadPool.h>
#include <muduo/net/SocketsOps.h>

#include <stdio.h>  // snprintf

using namespace muduo;
using namespace muduo::net;

UdpServer::UdpServer(EventLoop* loop,
            const InetAddress& listenAddr,
            const string& nameArg)
  : loop_(loop),
    ipPort_(listenAddr.toIpPort()),
    name_(nameArg),
    listenAddr_(listenAddr),
    acceptor_(new Acceptor(loop, listenAddr, true, true)),
    threadPool_(new EventLoopThreadPool(loop, name_)),
    nextConnId_(1)
{
    acceptor_->setUdpNewConnectionCallback(
        std::bind(&UdpServer::newConnection, this, _1, _2, _3));
}

UdpServer::~UdpServer()
{
  loop_->assertInLoopThread();
  LOG_TRACE << "UdpServer::~UdpServer [" << name_ << "] destructing";

  for (auto& item : connections_)
  {
    UdpConnectionPtr conn(item.second);
    item.second.reset();
    conn->getLoop()->runInLoop(
      std::bind(&UdpConnection::connectDestroyed, conn));
  }
}

void UdpServer::setThreadNum(int numThreads)
{
  assert(0 <= numThreads);
  threadPool_->setThreadNum(numThreads);
}

void UdpServer::start()
{
  if (started_.getAndSet(1) == 0)
  {
    threadPool_->start(threadInitCallback_);

    assert(!acceptor_->listening());
    loop_->runInLoop(
        std::bind(&Acceptor::listen, get_pointer(acceptor_)));
  }
}

void UdpServer::newConnection(int sockfd, const InetAddress& peerAddr, std::vector<char>& recvData)
{
  loop_->assertInLoopThread();
  EventLoop* ioLoop = threadPool_->getNextLoop();
  Socket sock(sockfd, true);
  sock.bindAddress(listenAddr_);
  sock.bindUdpPeerAddress(peerAddr); // call ::connect
  
  char buf[64];
  snprintf(buf, sizeof buf, "-%s#%d", ipPort_.c_str(), nextConnId_);
  ++nextConnId_;
  string connName = name_ + buf;
  LOG_INFO << "UdpServer::newConnection [" << name_
           << "] - new connection [" << connName
           << "] from " << peerAddr.toIpPort();
  UdpConnection::Ptr conn = std::make_shared<UdpConnection> (ioLoop, 
                                                          connName, 
                                                          sock.fd(),
                                                          listenAddr_,
                                                          peerAddr);
  connections_[connName] = conn;
  conn->setConnectionCallback(connectionCallback_);
  conn->setMessageCallback(messageCallback_);
  conn->setCloseCallback(closeCallback_);

  ioLoop->runInLoop(std::bind(&UdpConnection::connectionEstablished, conn));
}