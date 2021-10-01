#include "Log.h"

namespace Log
{
	bool optionOutputConsole = false;
	ofstream stream;
	wofstream stream_wide;

	bool Open(string path)
	{
		stream.open(path, std::ios_base::out | std::ios_base::app);

		if (!stream.is_open())
		{
			return false;
		}

		stream.setf(std::ios_base::showbase);
		stream.flush();

		return true;
	}

	bool Open(string path, bool allowConsoleOutput)
	{
		optionOutputConsole = allowConsoleOutput;
		return Open(path);
	}

	bool OpenW(wstring path)
	{
		stream_wide.open(path, std::ios_base::out | std::ios_base::app);

		if (!stream_wide.is_open())
		{
			return false;
		}

		stream_wide.setf(std::ios_base::showbase);
		stream_wide.flush();

		return true;
	}

	bool OpenW(wstring path, bool allowConsoleOutput)
	{
		optionOutputConsole = allowConsoleOutput;
		return OpenW(path);
	}

	void Close()
	{
		if (stream.is_open())
		{
			stream.flush();
			stream.close();
		}
		optionOutputConsole = false;
	}

	void CloseW()
	{
		if (stream_wide.is_open())
		{
			stream_wide.flush();
			stream_wide.close();
		}
		optionOutputConsole = false;
	}

	bool IsOpen()
	{
		return stream.is_open();
	}

	bool IsOpenW()
	{
		return stream_wide.is_open();
	}

	void Log(LLevel level, const char* format, ...)
	{
		if (!IsOpen())
			return;

		SYSTEMTIME time;
		GetLocalTime(&time);

		const char level_names[][6] = { "FATAL", "ERROR", "WARN ", "INFO ", "TRACE" };

		stream << std::right << std::setfill('0')
			<< std::setw(2) << time.wDay << '/'
			<< std::setw(2) << time.wMonth << '/'
			<< std::setw(4) << time.wYear << ' '
			<< std::setw(2) << time.wHour << ':'
			<< std::setw(2) << time.wMinute << ':'
			<< std::setw(2) << time.wSecond << ':'
			<< std::setw(3) << time.wMilliseconds << ' '
			<< '[' << std::setw(5) << GetCurrentThreadId() << ']' << std::setfill(' ')
			<< " | " << level_names[static_cast<unsigned int>(level)] << " | " << std::left;

		va_list args;
		size_t len = 0;
		va_start(args, format);
		len = std::vsnprintf(NULL, 0, format, args);
		va_end(args);

		char* outputStrBuffer = new char[len + 1];
		memset(outputStrBuffer, 0, len + 1);

		va_start(args, format);
		vsnprintf(outputStrBuffer, len + 1, format, args);
		stream << outputStrBuffer << std::endl;

		if (optionOutputConsole)
		{
			cout << level_names[static_cast<unsigned int>(level)] << L" | ";
			vprintf(format, args);
			cout << std::endl;
		}

		va_end(args);
		delete[] outputStrBuffer;
	}

	void Log(const char* format, ...)
	{
		va_list args;
		va_start(args, format);
		Log(LLevel::Info, format, args);
		va_end(args);
	}

	void LogW(LLevel level, const wchar_t* format, ...)
	{
		if (!IsOpenW())
			return;

		SYSTEMTIME time;
		GetLocalTime(&time);

		const wchar_t level_names[][6] = { L"FATAL", L"ERROR", L"WARN ", L"INFO ", L"TRACE" };

		stream_wide << std::right << std::setfill(L'0')
			<< std::setw(2) << time.wDay << L'/'
			<< std::setw(2) << time.wMonth << L'/'
			<< std::setw(4) << time.wYear << L' '
			<< std::setw(2) << time.wHour << L':'
			<< std::setw(2) << time.wMinute << L':'
			<< std::setw(2) << time.wSecond << L':'
			<< std::setw(3) << time.wMilliseconds << L' '
			<< L'[' << std::setw(5) << GetCurrentThreadId() << L']' << std::setfill(L' ')
			<< L" | " << level_names[static_cast<unsigned int>(level)] << L" | " << std::left;

		va_list args;
		size_t len = 0;
		va_start(args, format);
		len = std::vswprintf(NULL, 0, format, args);
		va_end(args);

		wchar_t* outputStrBuffer = new wchar_t[len + 1];
		memset(outputStrBuffer, 0, len + 1);

		va_start(args, format);
		vswprintf(outputStrBuffer, len + 1, format, args);
		stream_wide << outputStrBuffer << std::endl;

		if (optionOutputConsole)
		{
			cout << level_names[static_cast<unsigned int>(level)] << L" | ";
			vwprintf(format, args);
			cout << std::endl;
		}

		va_end(args);
		delete[] outputStrBuffer;
	}

	void LogW(const wchar_t* format, ...)
	{
		va_list args;
		va_start(args, format);
		LogW(LLevel::Info, format, args);
		va_end(args);
	}

	void FlushLogBuffers()
	{
		if (IsOpen())
			stream.flush();
		if (IsOpenW())
			stream_wide.flush();
	}


};
