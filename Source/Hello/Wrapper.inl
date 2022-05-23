
#ifndef WRAPPER_INL
#define WRAPPER_INL

namespace Wrapper
{
    class File
    {
    public:
        enum class Disposition : DWORD
        {
            Create_Always = CREATE_ALWAYS,
            Create_New = CREATE_NEW,
            Open_Always = OPEN_ALWAYS,
            Open_Existing = OPEN_EXISTING,
            Truncate_Existing = TRUNCATE_EXISTING,
        };
        enum class Access : DWORD
        {
            Generic_All = GENERIC_ALL,
            Generic_Execute = GENERIC_EXECUTE,
            Generic_Read = GENERIC_READ,
            Generic_Write = GENERIC_WRITE,
            File_Append_Data = FILE_APPEND_DATA,

        };
        File(HANDLE file) : m_file{file} {};
        File(const File &) = delete;
        File &operator=(const File &) = delete;
        File(File &&) = delete;
        File &operator=(File &&) = delete;
        HANDLE Get() const noexcept { return m_file; };
        bool IsValid() const noexcept { return m_file != 0ull; }; // Writer.inl(33,22): warning C4365: 'argument': conversion from 'int' to 'const unsigned __int64', signed/unsigned mismatch

        void Attach(HANDLE file) noexcept
        {
            if (m_file != file)
            {
                File tmp{file};
                swap(tmp, *this);
            };
        };

        virtual ~File()
        {
            if (IsValid())
                ::CloseHandle(m_file);
        };

        static HANDLE IvalidHandle() noexcept { return 0; };
        static HANDLE CreateFile(const wchar_t *filename, Access access = Access::File_Append_Data, Disposition disposition = Disposition::Open_Always) noexcept
        {
            using AccessT = std::underlying_type<Access>::type;
            using DispositionT = std::underlying_type<Disposition>::type;
            return CreateFile2(filename, static_cast<AccessT>(access), 0,
                               static_cast<AccessT>(disposition), 0);
        };

        DWORD Write(const wchar_t *text) const noexcept
        {
            return Write(std::wstring_view{text});
        };

        DWORD Write(std::wstring_view text) const noexcept
        {
            DWORD numbytes{};
            ::WriteFile(m_file, text.data(),
                        static_cast<DWORD>(text.size() * 2),
                        &numbytes, nullptr);
            return numbytes;
        };

    private:
        HANDLE m_file{};
        friend void swap(File &left, File &right) noexcept { std::swap(left.m_file, right.m_file); };
    };

};

#endif
