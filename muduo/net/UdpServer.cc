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
    threadPool_(new EventLoopThreadPool(loop, name_))
{
    acceptor_->setUdpNewConnectionCallback(
        std::bind(&UdpServer::newConnection, this, _1, _2, _3));
}

void UdpServer::newConnection(int sockfd, const InetAddress& peerAddr, std::vector<char>& recvData)
{
  loop_->assertInLoopThread();
  sockets::bindOrDie(sockfd, listenAddr_.getSockAddr());
  sockets::connect(sockfd, peerAddr.getSockAddr());
//   EventLoop* ioLoop = threadPool_->getNextLoop();
  
  char buf[64];
  snprintf(buf, sizeof buf, "-%s#%d", ipPort_.c_str(), sockfd);
  string sessionName = name_ + buf;
  LOG_INFO << "UdpServer::newConnection [" << name_
           << "] - new connection [" << sessionName
           << "] from " << peerAddr.toIpPort();
//   InetAddress localAddr(listenAddr_);
//   TcpConnectionPtr conn(new TcpConnection(ioLoop,
//                                           connName,
//                                           sockfd,
//                                           localAddr,
//                                           peerAddr));
//   connections_[connName] = conn;
//   conn->setConnectionCallback(connectionCallback_);
//   conn->setMessageCallback(messageCallback_);
//   conn->setWriteCompleteCallback(writeCompleteCallback_);
//   conn->setCloseCallback(
//       std::bind(&TcpServer::removeConnection, this, _1)); // FIXME: unsafe
//   ioLoop->runInLoop(std::bind(&TcpConnection::connectEstablished, conn));
}