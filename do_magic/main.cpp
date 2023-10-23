#include <iostream>
#include "do_magic.hpp"

int main()
{
    do_magic();
    std::string s;
    std::getline(std::cin, s);
    std::cout << s;
}