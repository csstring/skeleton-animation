#include "Common.h"
#include <sstream>

void ft_assert(const std::string& exec)
{
    std::cout << exec << std::endl;
    assert(false);
}

std::vector<std::string> ft_split(const std::string& str)
{
    std::stringstream ss(str);
    std::vector<std::string> v;
    std::string buffer;

    while (ss >> buffer)
        v.push_back(buffer);
    v.shrink_to_fit();
    return v;
}