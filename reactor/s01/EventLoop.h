// excerpts from http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#ifndef MUDUO_NET_EVENTLOOP_H
#define MUDUO_NET_EVENTLOOP_H

#include "thread/Thread.h"

#include <boost/scoped_ptr.hpp>
#include <vector>

namespace muduo
{

class Channel;
class Poller;

class EventLoop : boost::noncopyable
{
 public:

  EventLoop();

  // force out-line dtor, for scoped_ptr members.
  ~EventLoop();

  ///
  /// Loops forever.
  ///
  /// Must be called in the same thread as creation of the object.
  ///
  void loop();

  void quit();

  // internal use only
  void updateChannel(Channel* channel);
  // void removeChannel(Channel* channel);

  void assertInLoopThread()
  {
    if (!isInLoopThread())
    {
      abortNotInLoopThread();
    }
  }

  bool isInLoopThread() const { return threadId_ == CurrentThread::tid(); }

 private:

  void abortNotInLoopThread();

  typedef std::vector<Channel*> ChannelList;

  bool looping_; /* atomic */
  bool quit_; /* atomic */
  const pid_t threadId_;
  // Poller和EventLoop是组合关系
  // Poller的生存周期由EventLoop来控制
  boost::scoped_ptr<Poller> poller_; // 最好使用 std::unique_ptr
  // Channel和EventLoop是聚合关系
  // 一个EventLoop可以有多个Channel，但是不管理它的生存周期
  // Channel的生存周期由TcpConnection、Acceptor、Connector等类控制
  ChannelList activeChannels_;
};

}

#endif  // MUDUO_NET_EVENTLOOP_H
