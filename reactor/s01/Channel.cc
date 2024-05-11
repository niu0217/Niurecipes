// excerpts from http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#include "Channel.h"
#include "EventLoop.h"
#include "logging/Logging.h"

#include <sstream>

#include <poll.h>

using namespace muduo;

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop* loop, int fdArg)
  : loop_(loop),
    fd_(fdArg),
    events_(0),
    revents_(0),
    index_(-1)
{
}

void Channel::update()
{
  loop_->updateChannel(this);
}

// POLLIN   可读事件
// POLLPRI  优先级数据可读事件
// POLLOUT  可写事件
// POLLNVAL 文件描述符非法，不是一个打开的文件描述符，不能执行 I/O 操作
// POLLERR  文件描述符上发生了错误，例如连接错误、接收错误等
// POLLRDHUP 连接的对端（远程端）关闭了连接或者关闭了写方向。
void Channel::handleEvent()
{
  if (revents_ & POLLNVAL) {
    LOG_WARN << "Channel::handle_event() POLLNVAL";
  }

  if (revents_ & (POLLERR | POLLNVAL)) {
    if (errorCallback_) errorCallback_();
  }
  if (revents_ & (POLLIN | POLLPRI | POLLRDHUP)) {
    if (readCallback_) readCallback_();
  }
  if (revents_ & POLLOUT) {
    if (writeCallback_) writeCallback_();
  }
}
