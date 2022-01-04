#include <iostream>
#include <windows.h>
#include <sstream>
#include <string>

#pragma once


// Logger: handles receiving and then writing logs to a file
//===========================================================

class Logger
{
private:
    std::wstringstream stream;
    HANDLE file;

public:
    typedef std::ostream& (*ManipFn)(std::ostream&);

    // initialize the log file as "<executable name>_log.txt"
    Logger()
    {
        char fileName[MAX_PATH + 4] = { 0 };
        char exeName[MAX_PATH] = { 0 };

        GetModuleFileName(NULL, exeName, MAX_PATH);
        exeName[strlen(exeName) - 4] = '\0';
        strcat_s(fileName, exeName);
        strcat_s(fileName, "_log.txt");

        this->file = CreateFile(fileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
    }

    // override the << operator to redirect values to our internal stream
    template <typename T>
    Logger& operator <<(T const& value)
    {
        stream << value;
        return *this;
    }

    // override the << operator to detect the std::endl stream manipulation
    Logger& operator<<(ManipFn manip)
    {
        if (manip == static_cast<ManipFn>(std::endl))
            this->write_log();

        return *this;
    }

    void write_log()
    {
        std::wstring log;
        DWORD logLength;

        stream << L"\n";
        log = stream.str();
        logLength = wcslen(log.c_str()) * sizeof(wchar_t);

        WriteFile(this->file, log.c_str(), logLength, NULL, NULL);

        stream.str(std::wstring()); // clear the stream
        stream.clear(); // clear errors
    }

    void close()
    {
        CloseHandle(this->file);
    }
};
