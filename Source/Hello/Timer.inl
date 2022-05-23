#ifndef TIMER_INL
#define TIMER_INL

namespace Time
{

  using immediate = std::integral_constant<bool, true>;
  using deferred = std::integral_constant<bool, false>;

  inline SYSTEMTIME GetLocalTime() noexcept
  {
    SYSTEMTIME tmp{};
    ::GetLocalTime(&tmp);
    return tmp;
  };
  inline long long QueryPerformanceFrequency() noexcept
  {
    LARGE_INTEGER tmp;
    ::QueryPerformanceFrequency(&tmp);
    return tmp.QuadPart;
  };

  class Timer
  {
  public:
    explicit Timer(immediate = immediate{})
        : m_Frequency{Time::QueryPerformanceFrequency()},
          m_GlblTime{},
          Intrnl_Time{}
    {
      Count();
      Intrnl_Time = {0, 0}; //  discard  time pass  from OS's start, so timer counts his life time
    };

    explicit Timer(deferred)
        : Timer(immediate{})
    {
      m_GlblTime = 0;
    };

    virtual ~Timer() = default;

    void TimeStamp() const noexcept {};

    void Count() noexcept
    {
      long long ElapsedTicks{};
      ::QueryPerformanceCounter(reinterpret_cast<::LARGE_INTEGER *>(&ElapsedTicks));
      ElapsedTicks = ElapsedTicks - m_GlblTime;
      Intrnl_Time.m_Delta = (static_cast<double>(ElapsedTicks) / static_cast<double>(m_Frequency));
      Intrnl_Time.m_SinceStart += (ElapsedTicks * 1000) / m_Frequency;
      m_GlblTime += ElapsedTicks;
    };

    void Pause() noexcept
    {
      m_PausedAt = Intrnl_Time.m_SinceStart;
    };
    void Unpause() noexcept
    {
      Intrnl_Time.m_SinceStart -= Intrnl_Time.m_SinceStart - m_PausedAt;
      m_PausedAt = 0;
    };

    /**
     * Unpause if pause, and vice-versa
     */
    void Switch() noexcept
    {
      (m_PausedAt) ? Unpause() : Pause();
    };

    void Reset() noexcept
    {
      Timer tmp{};
      swap(tmp, *this);
    };

    Timer(const Timer &other)
        : m_Frequency{other.m_Frequency},
          m_GlblTime{other.m_GlblTime},
          Intrnl_Time{other.Intrnl_Time} {};
    Timer operator=(const Timer &other) { return Timer{other}; };

    std::strong_ordering operator<=>(const Timer &other) { return Intrnl_Time.m_SinceStart <=> other.Intrnl_Time.m_SinceStart; };

    Timer(Timer &&) = default;
    Timer &operator=(Timer &&) = default;
    // warning C5027: 'Timer::Timer': move assignment operator was implicitly defined as deleted

    template <typename T>
    T GetDelta() noexcept { return m_PausedAt ? static_cast<T>(0) : static_cast<T>(Intrnl_Time.m_Delta); };

    // gets time since object creation,  if  paused gets time when was pause
    template <typename T>
    T GetCount() noexcept { return m_PausedAt ? static_cast<T>(0) : static_cast<T>(Intrnl_Time.m_SinceStart); };

  protected:
  private:
    const long long m_Frequency{};

    long long m_GlblTime{}; // since OS's start + since object creation, aka what  ::QueryPerformanceFrequency returned in last Count()
    long long m_PausedAt{};

    struct
    {
      long long m_SinceStart{}; // since object creation(1 =1 milisecond)
      double m_Delta{};         // Delta(1.000000=1 second)
    } Intrnl_Time;

    friend void swap(Timer &Left, Timer &Right) noexcept
    {
      Timer Tmp{std::move(Left)};
      Left = std::move(Right);
      Right = std::move(Tmp);
    };
  };

}
#endif