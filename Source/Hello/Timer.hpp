#ifndef TIMER_HPP
#define TIMER_HPP

namespace Time 
{

  SYSTEMTIME GetLocalTime() noexcept;
  long long QueryPerformanceFrequency() noexcept;

  class  Timer;

}
#endif