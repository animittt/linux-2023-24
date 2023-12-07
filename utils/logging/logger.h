#pragma once

#include <iostream>
#include <fstream>
#include <ctime>
#include <chrono>
#include <sstream>
#include <string>
#include <iomanip>

#define LOG_INFO(filename,message) getLogging(filename).Log(INFO, message, __func__, __LINE__)
#define LOG_WARNING(filename,message) getLogging(filename).Log(WARNING, message,  __func__, __LINE__)
#define LOG_ERROR(filename,message) getLogging(filename).Log(ERROR, message, __func__, __LINE__)
#define LOG_FATAL(filename,message) getLogging(filename).Log(FATAL, message, __func__, __LINE__)

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
    explicit Logger(const char* filename);
    ~Logger();
    static std::string LogLevelStr(logLevel level);
    void Log(logLevel level,const std::string& message, const std::string& FUNC, int LINE );
    static char const* getColor(logLevel level);
};

inline Logger& getLogging(const char* filename = nullptr)
{
    static Logger log(filename);
    return log;
}
