#include "logger.h"
int main()
{
        Logger ua(nullptr);

        ua.Log(logLevel::INFO, __FILE__, __LINE__, "This is an informational message.");
        ua.Log(logLevel::WARNING, __FILE__, __LINE__, "This is a warning message.");

        ua.Log(logLevel::ERROR, __FILE__, __LINE__, "This is an error message.");
        Logger uaa("example.log");
        uaa.Log(logLevel::INFO, __FILE__, __LINE__, "info message");
        return 0;
}
