#ifndef APP_LOG_INCLUDED
#define APP_LOG_INCLUDED

#include <windows.h>
#include <stdio.h>
#include <stdint.h>

#include <string>
#include <iostream>
#include <iomanip>      // std::setw
#include <fstream>

namespace Log
{
    using namespace std;

    enum class LLevel
    {
        Fatal,
        Error,
        Warning,
        Info,
        Debug,
    };

    extern bool optionOutputConsole;
    extern ofstream stream;
    extern wofstream stream_wide;

    bool Open(string path);
    bool Open(string path, bool allowConsoleOutput);
    bool OpenW(wstring path);
    bool OpenW(wstring path, bool allowConsoleOutput);
    void Close();
    void CloseW();
    bool IsOpen();
    bool IsOpenW();
    void Log(LLevel level, const char* format, ...);
    void Log(const char* format, ...);
    void LogW(LLevel level, const wchar_t* format, ...);
    void LogW(const wchar_t* format, ...);
    void FlushLogBuffers();
};

#endif