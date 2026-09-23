#include <windows.h>
#include <cstdio>
#include <ctime>
#include <string>
#include <utility>

namespace Output
{
    inline void Enable_Vt()
    {
        HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD mode = 0;
        GetConsoleMode(h, &mode);
        mode |= 0x0004;
        SetConsoleMode(h, mode);
    }

    inline std::string Timestamp()
    {
        std::time_t t = std::time(nullptr);
        std::tm bt{};
        localtime_s(&bt, &t);
        char buf[16]{};
        std::snprintf(buf, sizeof(buf), "[%02d:%02d:%02d]", bt.tm_hour, bt.tm_min, bt.tm_sec);
        return std::string(buf);
    }

    inline void Write_Prefix(const char* color, const char* tag)
    {
        auto ts = Timestamp();
        std::printf("%s %s[%s]\033[0m ", ts.c_str(), color, tag);
    }

    template<typename... A>
    inline void Info(const char* format, A&&... args)
    {
        Enable_Vt();
        Write_Prefix("\033[36m", "INFO");
        std::printf(format, std::forward<A>(args)...);
        std::printf("\n");
    }

    template<typename... A>
    inline void Warning(const char* format, A&&... args)
    {
        Enable_Vt();
        Write_Prefix("\033[33m", "WARNING");
        std::printf(format, std::forward<A>(args)...);
        std::printf("\n");
    }

    template<typename... A>
    inline void Error(const char* format, A&&... args)
    {
        Enable_Vt();
        Write_Prefix("\033[31m", "ERROR");
        std::printf(format, std::forward<A>(args)...);
        std::printf("\n");
    }

    template<typename... A>
    inline void Success(const char* format, A&&... args)
    {
        Enable_Vt();
        Write_Prefix("\033[32m", "SUCCESS");
        std::printf(format, std::forward<A>(args)...);
        std::printf("\n");
    }

    template<typename... A>
    inline void Fang(const char* format, A&&... args)
    {
        Enable_Vt();
        Write_Prefix("\033[38;2;245;181;245m","FANG.WTF");
        std::printf(format, std::forward<A>(args)...);
        std::printf("\n");
    }
}
