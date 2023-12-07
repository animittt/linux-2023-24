#include <logger.h>

Logger::Logger(const char* filename) 
    : outf(filename)
    , out(filename ? outf : std::cout)
    , colored((filename == nullptr))
{
}

Logger::~Logger()
{
    out << std::flush;
    outf.close();
}

std::string Logger::LogLevelStr(logLevel level)
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


void Logger::Log(logLevel level,const std::string& message, const std::string& FUNC, int LINE )
{
    std::time_t currentTime = std::time(nullptr);
    std::tm* timeInfo = std::localtime(&currentTime);
    if (colored)
    {
        out << getColor(level)
            << "[" << std::put_time(timeInfo, "%T")
            << "] " << "["
            << FUNC << ":"
            << LINE << "]"
            << " [" << LogLevelStr(level)
            << "] "<< message

            << getColor(logLevel::INFO) <<'\n';
    }
    else
        out << "[" << std::put_time(timeInfo, "%T")
            << "] " << "[" << FUNC
            << ":" << LINE
            << "]" << " [" << LogLevelStr(level)
            << "] "<< message
            << '\n';
    if(level == FATAL)
    {
        exit(EXIT_FAILURE);
    }
    out<<'\n';
    out.flush();

}

char const* Logger::getColor(logLevel level)
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
