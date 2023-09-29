#include <iostream>
#include <fstream>
#include <ctime>
#include <chrono>
#include <sstream>
#include <string>
#include <iomanip>
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
    explicit Logger(const char* filename) : outf(filename), out(filename ? outf : std::cout), colored((filename == nullptr))
    {

    }
    ~Logger()
    {
        out << std::flush;
        outf.close();
    }
    std::string LogLevelStr(logLevel level)
    {
        switch (level)

        {

            case INFO: return "INFO";

            case WARNING: return "WARNING";

            case ERROR: return "ERROR";

            case FATAL: return "FATAL";

            case DEBUG: return "DEBUG";

            default: return "UNKNOWN";

        }
    }
    void Log(logLevel level,const std::string& FILE, int LINE, const std::string& message)
    {
        std::time_t now = std::time(nullptr);
        std::tm timeInfo = *std::localtime(&now);

        if (colored)
        {
            out << getColor(level) << "[" << std::put_time(&timeInfo, "%T") << "] " << "[" << FILE << ":" << __func__ << ":" << LINE << "]" << " [" << LogLevelStr(level) << "] " << message <<getColor(logLevel::INFO) << std::endl;

        }
        else
            out << "[" << std::put_time(&timeInfo, "%T") << "] " << "[" << FILE << ":" << __func__ << ":" << LINE << "]" << " [" << LogLevelStr(level) << "] " << message << std::endl;

    }
    char const * getColor(logLevel level) const
    {
        switch(level)
        {
            case ERROR:
                return "\033[31m";
            case WARNING:
                return "\033[33m";
            case DEBUG:
                return "\033[34m";
            default:
                return "\u001b[0m";
        }
    }
};

