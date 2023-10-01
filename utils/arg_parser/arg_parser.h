#pragma once
#include <string>
#include <optional>
#include <utility>
#include <vector>
#include <unistd.h>
#include <getopt.h>
#include <cstdlib>
#include "argument.h"

extern char* optarg;
extern int optind, opterr, optopt;

class arg_parser
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
        args_iterator(int argc, char** argv, const char* optstr, int index, argument arg)
                : _argc(argc)
                , _argv(argv)
                , _optstr(optstr)
                ,_index(index)
                , _arg(std::move(arg))
        {
            if (_argv != nullptr && _index < _argc)
            {
                if(_current_opt != -1)
                {
                    _index = optind;
                    _current_opt = (char)getopt(_argc, _argv, _optstr);
                    if (_current_opt != -1)
                    {
                        _arg._key.clear();
                        _arg._key.push_back(_current_opt);
                        _arg._value = std::nullopt;
                        if (optarg != nullptr)
                        {
                            _arg._value = optarg;
                        }
                    }
                }

                if(optind < _argc && _current_opt == -1)
                {
                    _arg._value = _argv[optind];
                    std::cout << _arg._value.value();
                    ++optind;
                    _index = optind;
                }

                if(_current_opt == '?')
                {
                    throw std::logic_error("invalid argument passed!");
                }
            }
        }

        args_iterator& operator++() {
            if (_argv != nullptr && _index < _argc)
            {
                _index = optind;
                if(_current_opt != -1)
                {
                    _current_opt = (char)getopt(_argc, _argv, _optstr);
                    if (_current_opt != -1)
                    {
                        _arg._key.clear();
                        _arg._key.push_back(_current_opt);
                        _arg._value = std::nullopt;
                        if (optarg != nullptr)
                            _arg._value = optarg;
                    }
                }

                if(optind < _argc && _current_opt == -1)
                {
                    _arg._value = _argv[optind];
                    ++optind;
                }

                if (_current_opt == '?')
                {
                    throw std::logic_error("invalid argument passed!");
                }
            }
            return *this;
        }
        value_type operator*()
        {
            return _arg;
        }

        bool operator!=(const args_iterator& other) const
        {
            return _index != other._index;
        }
        bool operator==(const args_iterator& other) const
        {
            return _index == other._index;
        }
    };

    arg_parser(int argc, char**  argv, const char* optstr )
            : optstr_(optstr)
            , argc_(argc)
            , argv_(argv)
    {
    }

    args_iterator begin()
    {
        optind = 0;
        return args_iterator{argc_, argv_, optstr_, optind, argument()};
    }

    [[nodiscard]]args_iterator begin() const
    {
        optind = 0;
        return args_iterator{argc_, argv_, optstr_, optind, argument()};
    }

    args_iterator end()
    {
        return args_iterator{argc_, nullptr, nullptr, argc_, argument()};
    }

    [[nodiscard]]args_iterator end() const
    {
        return args_iterator{argc_, nullptr, nullptr, argc_, argument()};
    }
    const char* getstr()
    {
        return optstr_;
    }


};
