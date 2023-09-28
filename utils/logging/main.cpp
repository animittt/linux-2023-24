#include "logger.h"
int main() 
{
        Logger logger("example.log", true, true, true);

        logger.logf(Logger::INFO, __FILE__, __LINE__, "This is an informational message.");
        logger.logf(Logger::WARNING, __FILE__, __LINE__, "This is a warning message.");
        logger.logf(Logger::ERROR, __FILE__, __LINE__, "This is an error message.");
        logger.logf(Logger::DEBUG, __FILE__, __LINE__, "This is a debug message.");

        logger.logf(Logger::INFO, nullptr, 0, "This message has no source information.");

        int someValue = 42;
        logger.logf(Logger::INFO, __FILE__, __LINE__, "The answer to everything is: %d", someValue);

        return 0;
}
