#pragma once
#include <iostream>
#include <fstream>
#include <ctime>
#include <chrono>
#include <sstream>
#include <string>
#include <iomanip>

#define LOG_INFO(message) getLogging().Log(INFO, message, __FILE__, __func__, __LINE__)
#define LOG_WARNING(message) getLogging().Log(WARNING, message, __FILE__, __func__, __LINE__)
#define LOG_ERROR(message) getLogging().Log(ERROR, message, __FILE__, __func__, __LINE__)
#define LOG_FATAL(message) getLogging().Log(FATAL, message, __FILE__, __func__, __LINE__)
enum  logLevel
{
    INFO, WARNING, ERROR, FATAL, DEBUG
};

class Logger
{
private:
    std::ostream& out;
    std::ofstream outf;
    bool colored;

public:
    explicit Logger(const char* filename) :
            outf(filename)
            , out(filename ? outf : std::cout)
            , colored((filename == nullptr))
    {
    }
    ~Logger()
    {
        out << std::flush;
        outf.close();
    }
    static std::string LogLevelStr(logLevel level)
    {
        switch (level)
        {
            case INFO:
                return "INFO";
            case WARNING:
                return "WARNING";
            case ERROR:
                return "ERROR";
            case FATAL:
                return "FATAL";
            case DEBUG:
                return "DEBUG";
            default:
                return "UNKNOWN";
        }
    }
    void Log(logLevel level,const std::string& message, const std::string& FILE, const std::string& FUNC, int LINE )
    {
        std::time_t currentTime = std::time(nullptr);
        std::tm* timeInfo = std::localtime(&currentTime);
        if (colored)
        {
            out << getColor(level)
                << "[" << std::put_time(timeInfo, "%T")
                << "] " << "[" << FILE
                << ":" << FUNC
                << ":" << LINE
                << "]" << " [" << LogLevelStr(level)
                << "] "<< message

                    << getColor(logLevel::INFO) <<'\n';
        }
        else
            out << "[" << std::put_time(timeInfo, "%T")
                << "] " << "[" << FILE
                << ":" << FUNC
                << ":" << LINE
                << "]" << " [" << LogLevelStr(level)
                << "] "<< message
                << '\n';
        if(level == FATAL)
        {
            exit(EXIT_FAILURE);
        }

    }
    static char const* getColor(logLevel level)
    {
        switch(level)
        {
            case INFO:
                return "\033[36m";
            case ERROR:
                return "\033[31m";
            case WARNING:
                return "\033[33m";
            case DEBUG:
                return "\033[34m";
            case FATAL:
                return "\033[31;1;4m";
            default:
                return "\033[0m";
        }
    }
};

inline Logger& getLogging()
{
    static Logger log(nullptr);
    return log;
}
