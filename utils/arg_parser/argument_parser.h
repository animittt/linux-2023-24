#pragma once
#include <string>
#include <optional>
#include <utility>
#include <vector>
#include <unistd.h>
#include <getopt.h>
#include <cstdlib>
#include <argument.h>

class argument_parser
{

private:
    char** argv_;
    int argc_;
    const char* optstr_;

public:
    class args_iterator
    {
    private:
        int _argc;
        char** _argv;
        int _index{};
        char _current_opt;
        argument _arg;
        const char* _optstr;

    public:
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = argument;
        using pointer           = argument*;  // or also value_type*
        using reference         = argument&;

        void parse();

        args_iterator(int, char**, const char*, int);

        args_iterator& operator++();

        reference operator*();

        pointer operator->();
        bool operator!=(const args_iterator& other) const;

        bool operator==(const args_iterator& other) const;
    };
    argument_parser(int, char**, const char*);

    args_iterator begin();

    [[nodiscard]]args_iterator begin() const;

    args_iterator end();

    [[nodiscard]]args_iterator end() const;
};
