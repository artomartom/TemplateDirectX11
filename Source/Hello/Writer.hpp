
#ifndef WRITER_HPP
#define WRITER_HPP

namespace Writer
{
    constexpr bool IsConsoleAvailable()
    {
#if (!defined(CONSOLE)) && (defined(GUI))
#define GUI_ONLY(expr) expr
#define CONSOLE_ONLY(expr)

        return false;
#else
#define GUI_ONLY(expr)
#define CONSOLE_ONLY(expr) expr
        return true;
#endif
    };

    enum class Out : uint32_t
    {
        None = 0,
        Console = 1,
        File = (Console + 1),
    };
    enum class Type : uint32_t
    {
        None = 0,
        Log = 1,
        Warning = (Log + 1),
        Error = (Warning + 1),

    };

    inline std::wstring GetSysTime();

    template <Type T>
    constexpr std::wstring_view prefix();

    template <Type T, Out O>
    struct Message;

    template <Out OT>
    using Log = Message<Type::Log, OT>;

    template <Out OT>
    using Warning = Message<Type::Warning, OT>;

    template <Out OT>
    using Error = Message<Type::Error, OT>;
};
#endif
