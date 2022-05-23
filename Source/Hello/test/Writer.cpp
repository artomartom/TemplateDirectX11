
#include "../Hello.hpp"
#include <thread>

int main()
{
    auto Write{
        []()
        {
            const wchar_t *txt[10]{
                L" 1",
                L" 2",
                L" 3",
                L" 4",
                L" 5",
                L" 6",
                L" 7",
                L" 8",
                L" 9",
                L" 0",
            };

            for (auto &each : txt)
                Log<File>::Write(each, ' ', each);
        }};

    std::thread write1{Write};
    std::thread write2{Write};
    write1.join();
    write2.join();
};
