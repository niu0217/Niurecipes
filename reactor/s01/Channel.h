// excerpts from http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#ifndef MUDUO_NET_CHANNEL_H
#define MUDUO_NET_CHANNEL_H

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

namespace muduo
{

class EventLoop;

///
/// A selectable I/O channel.
///
/// This class doesn't own the file descriptor.
/// The file descriptor could be a socket,
/// an eventfd, a timerfd, or a signalfd
/// 对IO事件的响应与封装 注册IO的可读可写等事件
class Channel : boost::noncopyable
{
 public:
  typedef boost::function<void()> EventCallback;

  Channel(EventLoop* loop, int fd);

  void handleEvent();
  void setReadCallback(const EventCallback& cb)
  { readCallback_ = cb; }
  void setWriteCallback(const EventCallback& cb)
  { writeCallback_ = cb; }
  void setErrorCallback(const EventCallback& cb)
  { errorCallback_ = cb; }

  int fd() const { return fd_; }
  int events() const { return events_; }
  void set_revents(int revt) { revents_ = revt; }
  bool isNoneEvent() const { return events_ == kNoneEvent; }

  void enableReading() { events_ |= kReadEvent; update(); }
  // void enableWriting() { events_ |= kWriteEvent; update(); }
  // void disableWriting() { events_ &= ~kWriteEvent; update(); }
  // void disableAll() { events_ = kNoneEvent; update(); }

  // for Poller
  int index() { return index_; }
  void set_index(int idx) { index_ = idx; }

  EventLoop* ownerLoop() { return loop_; }

 private:
   // 调用关系：调用EventLoop的updateChannel()
   //         ==>  再调用Poller的updateChannel(channel)
   // 将fd_的可读可写等事件注册到Poller中
   void update();

   static const int kNoneEvent;
   static const int kReadEvent;
   static const int kWriteEvent;

   EventLoop *loop_;
   // Channel不拥有文件描述符，也就是说当Channel析构的时候
   // 文件描述符并没有被close掉
   // Channel和文件描述符是关联关系，一个Channel有一个文件描述符
   // EventLoop和文件描述符也是关联关系，一个EventLoop有多个文件描述符
   // 那么文件描述符fd_是被谁管理的呢？答案是Socket类，它的生命周期由Socket类来管理
   // Socket类的析构函数会调用close来关闭文件描述符fd_
   const int fd_;
   int events_;  // 它关心的事件
   int revents_; // 目前活跃的事件
   int index_;   // used by Poller.

   EventCallback readCallback_;
   EventCallback writeCallback_;
   EventCallback errorCallback_;
};

}
#endif  // MUDUO_NET_CHANNEL_H
