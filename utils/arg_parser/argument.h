#pragma once
//if both _key and _value are empty then the argument is invalidated.
//if _key is empty and _value is not then it is a mandatory argument.
//if _key is not empty and the _value is empty then it is a flag
//if both _key and _value are non-empty then it is an option
class argument
{
public:
    std::string _key;
    std::optional<std::string> _value;
};