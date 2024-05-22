#include "Channel.h"
#include "EventLoop.h"

#include <stdio.h>
#include <sys/timerfd.h>

muduo::EventLoop* g_loop;

void timeout()
{
  printf("Timeout!\n");
  g_loop->quit();
}

// timerfd_create把时间变成了一个文件描述符，该“文件”在定时器超时
// 的那一刻变得可读，这样就可以很方便的融入到select/poll框架中，用
// 统一的方式来处理IO事件和超时事件，这也正是Reactor模式的长处.
int main()
{
  muduo::EventLoop loop;
  g_loop = &loop;

  int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
  muduo::Channel channel(&loop, timerfd);
  channel.setReadCallback(timeout);
  channel.enableReading();

  struct itimerspec howlong;
  bzero(&howlong, sizeof howlong);
  howlong.it_value.tv_sec = 5;
  ::timerfd_settime(timerfd, 0, &howlong, NULL);

  loop.loop();

  ::close(timerfd);
}
