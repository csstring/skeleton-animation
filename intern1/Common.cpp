#include "include/Common.h"
#include "include/GLM/gtx/transform.hpp"
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

glm::mat4 ft_rotate(const glm::vec3 start, const glm::vec3 end)
{
    glm::vec3 axis = glm::cross(start, end);
    glm::vec3 axisNormal = glm::normalize(axis);
    float angle = glm::acos(glm::dot(start, end));

    return glm::rotate(angle, axisNormal);
}