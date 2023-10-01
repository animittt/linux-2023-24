#include <iostream>
#include "argument_parser.h"
#include "argument.h"
void argument_parser::args_iterator::parse()
{
    if (_argv != nullptr && _index < _argc)
    {
        _index = optind;
        if(_current_opt != -1)
        {

            _current_opt = (char)getopt(_argc, _argv, _optstr);
            if (_current_opt != -1)
            {
                _arg._key = _current_opt;
                _arg._value = std::nullopt;
                if (optarg != nullptr)
                {
                    _arg._value = optarg;
                }
            }
        }

        if(optind < _argc && _current_opt == -1)
        {
            _arg._key.clear();
            _arg._key = _argv[optind];
            ++optind;
            _index = optind;
        }

        if(_current_opt == '?')
        {
            throw std::logic_error("invalid argument passed!");
        }
    }
}

argument_parser::args_iterator::args_iterator(int argc, char ** argv, const char * optstr, int index)
        : _argc(argc)
        , _argv(argv)
        , _optstr(optstr)
        , _index(index)
{
    parse();
}
argument_parser::args_iterator&  argument_parser::args_iterator::operator++()
{
    parse();
    return *this;
}
argument_parser::args_iterator::value_type argument_parser::args_iterator::operator*()
{
    return _arg;
}

bool argument_parser::args_iterator::operator!=(const args_iterator& other) const
{
    return _index != other._index;
}
bool argument_parser::args_iterator::operator==(const args_iterator& other) const
{
    return _index == other._index;
}

argument_parser::argument_parser(int argc, char**  argv, const char* optstr )
        : optstr_(optstr)
        , argc_(argc)
        , argv_(argv)
{
}
argument_parser::args_iterator argument_parser::begin()
{
    optind = 0;
    return argument_parser::args_iterator{argument_parser::argc_, argument_parser::argv_, argument_parser::optstr_, optind};
}
[[nodiscard]]argument_parser::args_iterator argument_parser::begin() const
{
    optind = 0;
    return args_iterator{argc_, argv_, optstr_, optind};
}

argument_parser::args_iterator argument_parser::end()
{
    return args_iterator{argc_, nullptr, nullptr, argc_};
}

[[nodiscard]]argument_parser::args_iterator argument_parser::end() const {
    return args_iterator{argc_, nullptr, nullptr, argc_};
}