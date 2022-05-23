
#ifndef WRITER_INL
#define WRITER_INL

namespace Writer
{

    template <typename... Args>
    constexpr std::wstringstream Accumulate(Args const &...args)
    {
        std::wstringstream ss{};
        (void)std::initializer_list<int>{(ss << args << L' ', 0)...};
        return ss;
    };

    inline std::wstring GetSysTime()
    {
        auto time{Time::GetLocalTime()};

        std::wstring out{L"00:00:00|00.00| "};
        WORD t[]{
            time.wHour, time.wMinute, time.wSecond, time.wDay, time.wMonth};
        UINT i{};
        for (auto &each : t)
        {
            out[i] += each / 10u;
            out[i + 1] += each % 10u;
            i += 3;
        };
        return out;
    };

    template <Type T>
    constexpr std::wstring_view prefix()
    {
        switch (T)
        {
        case Type::None:
            return L"None     |";
        case Type::Log:
            return L"Log      |";
        case Type::Warning:
            return L"Warning  |";
        case Type::Error:
            return L"Error    |";
        default:
            return L"          ";
        }
    };

    template <Type MT, Out O>
    struct Message
    {
    };
    template <typename T>
    struct AddSpace
    {
        AddSpace(const T &ref) : ref{ref} {};
        friend std::wostream &operator<<(std::wostream &os, const AddSpace &that)
        {
            return os << L' ' << that.ref;
        }

    private:
        const T &ref{};
    };
    template <Type MT>
    struct Message<MT, Console>
    {
        template <typename Arg, typename... Args>
        constexpr static void Write(const Arg arg, const Args... args)
        {
            static std::mutex m{};
            std::wstringstream ss{};
            (((ss << prefix<MT>() << GetSysTime()) << arg) << ... << AddSpace(args)) << L'\n';
            std::lock_guard<std::mutex> lockGuard{m};
            std::wcout << ss.str().c_str();
        };
    };
    template <Type MT>
    struct Message<MT, File>
    {
        template <typename Arg, typename... Args>
        constexpr static void Write(const Arg arg, const Args... args)
        {
            static std::mutex m{};
            std::wstringstream ss{};
            (((ss << prefix<MT>() << GetSysTime()) << arg) << ... << AddSpace(args)) << L'\n';
            std::lock_guard<std::mutex> lockGuard{m};
            Wrapper::File file{Wrapper::File::CreateFile(L"Log.txt")};
            file.Write(ss.view());
        };
    };

 
};
#endif