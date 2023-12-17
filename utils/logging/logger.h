#pragma once

#include <iostream>
#include <fstream>
#include <ctime>
#include <chrono>
#include <sstream>
#include <string>
#include <iomanip>

extern const char* path;

#define LOG_INFO(message) getLogging().Log(INFO, message, __func__, __LINE__)
#define LOG_WARNING(message) getLogging().Log(WARNING, message,  __func__, __LINE__)
#define LOG_ERROR(message) getLogging().Log(ERROR, message, __func__, __LINE__)
#define LOG_FATAL(message) getLogging().Log(FATAL, message, __func__, __LINE__)

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

void setLogger(const char* filename)
{
    path = filename;
}

inline Logger& getLogging()
{
    static Logger log(path);
    return log;
}
