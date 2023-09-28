//
// Created by mitani on 9/28/23.
//
#pragma once
#include <thread>
#include <iomanip>
#include <ctime>
#include <iostream>
#include <sstream>
#include <syncstream>
#include <chrono>
#include <fstream>
#include <mutex>
#pragma GCC diagnostic ignored "-Wformat-security"

class Logger
{
public:

    enum severity_t { INFO, WARNING, ERROR, FATAL, DEBUG };

private:

    static inline Logger* this_ptr;
    std::ofstream outf;
    std::ostream& out;
    bool const debug;
    bool const show_source;
    bool const colored;


public:
    explicit Logger(char const* filename, bool debug_param = true, bool show_source_param = true, bool colored_param = true)
            : outf(filename)
            , out(filename ?
                  outf : std::cout)
            , debug(debug_param)
            , show_source(show_source_param)
            , colored(colored_param)
    {

        this_ptr = this;
    }

    ~Logger()
    {
        out << std::flush;
        outf.close();
        this_ptr = nullptr;
    }

    Logger(const Logger&) = delete;

    Logger& operator=(const Logger&) = delete;

    Logger(Logger&&) = delete;

    Logger& operator=(Logger&&) = delete;

    template<typename ...T>
    void logf(Logger::severity_t severity, const char * FILE, size_t LINE, char const* fmt, T&& ... args)
    {
        if(nullptr == this_ptr)
            throw std::runtime_error("The logger must first be instantiated");

        if (Logger::DEBUG == severity && !this_ptr->debug)
            return;

        this_ptr->logf_internal(severity, FILE, LINE, fmt, std::forward<T>(args)...);


    }



private:

    char const * get_severity_str(Logger::severity_t severity) const
    {
        switch(severity)
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
        }
        return nullptr;
    }

    char const * get_severity_color_str(Logger::severity_t severity) const
    {
        switch(severity)
        {
            case Logger::ERROR:
                return "\033[31m";
            case Logger::WARNING:
                return "\033[33m";
            case Logger::DEBUG:
                return "\033[34m";
            default:
                return "\u001b[0m";
        }
    }

    template<typename ...T>
    void logf_internal(Logger::severity_t severity, const char * FILE, size_t LINE, char const * fmt, T&& ... args)
    {
        bool logToFile = (&out == &outf);
        std::time_t cur_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::tm* cur_time_local = std::localtime(&cur_time);
        if(show_source && nullptr != FILE)
        {
            if (logToFile)
                out << std::put_time(cur_time_local, "%Y.%m.%d [%H:%M:%S]")<<" (FROM: " << FILE << ":" << LINE << ") "<<  get_severity_str(severity) << ": " << this->format(fmt, std::forward<T>(args)...)<< std::endl;
            else
                out << get_severity_color_str(severity) << std::put_time(cur_time_local, "%Y.%m.%d [%H:%M:%S]")<<" (FROM: " << FILE << ":" << LINE << ") "<<  get_severity_str(severity) << ": " << this->format(fmt, std::forward<T>(args)...) << get_severity_color_str(Logger::severity_t::INFO) << std::endl;
        }
        else
        {
            if (logToFile)
                out << std::put_time(cur_time_local, "%Y.%m.%d [%H:%M:%S] ") << get_severity_str(severity) << ": " << this->format(fmt, std::forward<T>(args)...)  << std::endl;
            else
                out << get_severity_color_str(severity) << std::put_time(cur_time_local, "%Y.%m.%d [%H:%M:%S] ") << get_severity_str(severity) << ": " << this->format(fmt, std::forward<T>(args)...) << get_severity_color_str(Logger::severity_t::INFO) << std::endl;
        }
               }

    template<typename ... Args>
    std::string format( char const * format, Args&& ... args )
    {
        int size_s = std::snprintf( nullptr, 0, format, args ... ) + 1; // + 1 is for \0
        if( size_s <= 0 ){ throw std::runtime_error( "Error during formatting." ); }
        auto size = static_cast<size_t>( size_s );
        auto buf = std::make_unique<char[]>( size );
        std::snprintf( buf.get(), size, format, args ... );
        return std::string{ buf.get(), buf.get() + size - 1 };
    }
};
