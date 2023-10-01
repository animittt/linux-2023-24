#include <iostream>
#include <algorithm>
#include "arg_parser.h"
int main(int argc, char** argv)
{

    arg_parser parser(argc, argv, "cdf:r:");
    for (auto it = parser.begin(); it != parser.end(); ++it)
    {
        auto taken = (*it)._value;
        std::cout << (*it)._key << " " << ((*it)._value ? (*it)._value.value() : "" ) << std::endl;
    }
    return 0;
}