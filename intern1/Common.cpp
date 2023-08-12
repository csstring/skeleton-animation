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

glm::mat4 ft_rotate(glm::vec3 start, glm::vec3 end)
{ 
    start = glm::normalize(start);
    end = glm::normalize(end);
    glm::vec3 axis = glm::cross(start, end);
    glm::vec3 axisNormal = glm::normalize(axis);
    float angle = glm::acos(glm::dot(start, end));
    return glm::rotate(angle, axisNormal);
}

glm::vec3 quatToEulerDivideRatio(const glm::quat& quat, float ratio)
{
    glm::vec3 axis = glm::axis(quat);
    float angle = glm::angle(quat);
    angle *= ratio;
    glm::quat newQuat = glm::angleAxis(angle, axis);

    return glm::eulerAngles(newQuat);
}

glm::quat quatDivideRatio(const glm::quat& quat, float ratio)
{
    glm::vec3 axis = glm::axis(quat);
    float angle = glm::angle(quat);
    angle *= ratio;
    return glm::angleAxis(angle, axis);
}

std::string ft_to_string(const physx::PxVec3& vec)
{
    std::string oss;
    oss = "PxVec3(" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + ", " +std::to_string(vec.z) + ")";
    return oss;
}

