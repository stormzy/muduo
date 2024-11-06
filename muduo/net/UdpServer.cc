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
    nextSessionId_(1)
{
    acceptor_->setUdpNewConnectionCallback(
        std::bind(&UdpServer::newConnection, this, _1, _2, _3));
}

void UdpServer::newConnection(int sockfd, const InetAddress& peerAddr, std::vector<char>& recvData)
{
  loop_->assertInLoopThread();
  EventLoop* ioLoop = threadPool_->getNextLoop();
  Socket sock(sockfd, true);
  sock.bindAddress(listenAddr_);
  sock.bindUdpPeerAddress(peerAddr);
  
  char buf[64];
  snprintf(buf, sizeof buf, "-%s#%d", ipPort_.c_str(), nextSessionId_);
  ++nextSessionId_;
  string sessionName = name_ + buf;
  LOG_INFO << "UdpServer::newConnection [" << name_
           << "] - new connection [" << sessionName
           << "] from " << peerAddr.toIpPort();
  UdpSession::Ptr session = std::make_shared<UdpSession> (ioLoop, 
                                                          sessionName, 
                                                          sock.fd(),
                                                          listenAddr_,
                                                          peerAddr);
  sessions_[sessionName] = session;
  session->setConnectionCallback(connectionCallback_);
  session->setMessageCallback(messageCallback_);
  session->setCloseCallback(closeCallback_);

  ioLoop->runInLoop(std::bind(&UdpSession::sessionEstablished, session));
}