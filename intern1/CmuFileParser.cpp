#include "include/CmuFileParser.h"
#include "include/Skeleton.h"
#include "include/Animation.h"
#include "include/Bone.h"
#include "fstream"
#include "include/GLM/glm.hpp"
#include "include/GLM/gtc/matrix_transform.hpp"
#include <algorithm>
#include <iostream>
#include <cctype>
bool CmuFileParser::loadCmuFile(void)
{
    std::ifstream ifs(_filePath);

    if (ifs.is_open() == false) 
        return false;
    while (ifs.good())
    {
        char firstCh = ifs.get();
        std::string buffer;
        
        //assert(firstCh != '#' && firstCh != ';');
        if (isspace(firstCh)) continue;
        if (firstCh == '#')
        {
            std::getline(ifs, buffer);
            continue;
        }

        ifs >> buffer;

        if (buffer == "version")
        {
            ifs >> buffer;
        }
        else if (buffer == "name")
        {
            ifs >> buffer;
            _skeleton->setProgramName(buffer);
        }
        else if (buffer == "units")
            assert(parseAsfUnits(ifs));
        else if (buffer == "documentation") //정확한 형식 파악 불가 추후재검토
        {
            std::getline(ifs, buffer);
            std::getline(ifs, buffer);
            continue;
        }
        else if (buffer == "root")
            assert(parseAsfRoot(ifs));
        else if (buffer == "bonedata") //parsing hierarchy
        {
            assert(parseAsfBoneData(ifs));
        }
        else 
        {
            break;
            assert(false);
        }
    }
    ifs.close();
}

bool CmuFileParser::parseAsfUnits(std::ifstream& ifs)
{
    float mass, length;
    std::string buffer, deg;

    ifs >> buffer, ifs >> mass;
    if (buffer != "mass" || mass <= 0)
        return false;
    _skeleton->setMass(mass);

    ifs >> buffer, ifs >> length;
    if (buffer != "length" || length <= 0)
        return false;
    _skeleton->setGlobalLengthMultiplier(length);

    ifs >> buffer; 
    if (buffer != "angle")
        return false;
    
    ifs >> deg;
    if (deg == "deg")
        _skeleton->setIsDeg(true);
    
    return true;
}

bool CmuFileParser::parseAsfRoot(std::ifstream& ifs)
{
    Bone bone;

    bone._boneIndex = 0;
    bone._boneName = "root";
    
    std::string buffer;
    for (int i = 0; i < 4; ++i)
    {
        ifs >> buffer;
        if (buffer == "order")//fix me hard coding
        {
            while (ifs >> buffer)
            {
                if (buffer == "RX") bone._dof.push_back(DOF::RX);
                else if (buffer == "RY") bone._dof.push_back(DOF::RY);
                else if (buffer == "RZ") { bone._dof.push_back(DOF::RZ);
                    break;
                }
                else if (buffer == "TX") bone._dof.push_back(DOF::TX);
                else if (buffer == "TY") bone._dof.push_back(DOF::TY);
                else if (buffer == "TZ") bone._dof.push_back(DOF::TZ);
            }
        }
        else if (buffer == "axis")
        {
            std::getline(ifs, buffer);//fix me
        }
        else if (buffer == "position")
        {
            ifs >> bone._postion.x;
            ifs >> bone._postion.y;
            ifs >> bone._postion.z;
        }
        else if (buffer == "orientation")
        {
            ifs >> bone._orientation.x;
            ifs >> bone._orientation.y;
            ifs >> bone._orientation.z;
            
            if (_skeleton->getIsDeg())
            {
                bone._orientation.x *= PI / 180.0;
                bone._orientation.y *= PI / 180.0;
                bone._orientation.z *= PI / 180.0;
            }
        }
        else 
        {
            ft_assert("root parser fail");
        }
    }
    bone.setupBone();
    _skeleton->getBoneVector().push_back(bone);
    return true;
}


bool CmuFileParser::parseAsfBoneData(std::ifstream& ifs)
{
    while (true)
    {
        Bone bone;
        std::string buffer;
        while (ifs >> buffer)
        {
            if (buffer == "end")
                break;
            
            if (buffer == ":hierarchy")
            {
               parseAsfHierarchy(ifs);
               return true;
            }
            else if (buffer == "id")
            {
                ifs >> bone._boneIndex;
            }
            else if (buffer == "name")
            {
                ifs >> bone._boneName;
            }
            else if (buffer == "direction")
            {
                ifs >> bone._direction.x;
                ifs >> bone._direction.y;
                ifs >> bone._direction.z;
            }
            else if (buffer == "length")
            {
                ifs >> bone._length;
                assert(bone._length != 0);
            }
            else if (buffer == "axis")
            {
                ifs >> bone._orientation.x;
                ifs >> bone._orientation.y;
                ifs >> bone._orientation.z;
                
                if (_skeleton->getIsDeg())
                {
                    bone._orientation.x *= PI / 180.0;
                    bone._orientation.y *= PI / 180.0;
                    bone._orientation.z *= PI / 180.0;
                }
                std::getline(ifs, buffer); //xyz parsing
            }
            else if (buffer == "dof")
            {
                parserAsfDof(ifs, bone);
            }
        }

        //bone.setupBone();
        _skeleton->getBoneVector().push_back(bone);
    }

    return true;
}

bool CmuFileParser::parserAsfDof(std::ifstream& ifs, Bone& bone)
{
    std::string buffer;
    int dofCount = 0;

    while (ifs >> buffer)
    {
        if (buffer == "limits")
            break;
        if (buffer == "rx") bone._dof.push_back(DOF::RX);
        else if (buffer == "ry") bone._dof.push_back(DOF::RY);
        else if (buffer == "rz") bone._dof.push_back(DOF::RZ);
        else if (buffer == "tx") bone._dof.push_back(DOF::TX);
        else if (buffer == "ty") bone._dof.push_back(DOF::TY);
        else if (buffer == "tz") bone._dof.push_back(DOF::TZ);
        dofCount++;
        //fix me 객체안에 저장해야함 임시로 버리는중 
    }
    while (dofCount--)//fix me 객체안에 저장해야함 임시로 버리는중 
        std::getline(ifs, buffer);
    
    return true;
}

bool CmuFileParser::parseAsfHierarchy(std::ifstream& ifs)
{
    std::string buffer;
    std::string parent;
    std::vector<std::string> v;

    while (ifs >> parent)
    {
        if (parent == "end")
            break;
        else if (parent == "begin")
            continue;
        
        std::getline(ifs, buffer);
        v = ft_split(buffer);
        
        int32 boneIndex = _skeleton->findBoneIndex(parent);
        AnimationData* animationData = _animation->returnAnimationData(parent);

        if (boneIndex == -1)
            ft_assert("we can't find bone name");
        animationData->_childrens.resize(v.size());
        //animationData->_boneIndex = boneIndex;
        for (int i =0; i < v.size(); ++i)
        {
            animationData->_childrens[i]._name = v[i];
            animationData->_childrens[i]._boneIndex = _skeleton->findBoneIndex(v[i]);
            animationData->_childrens[i]._parentPointer = animationData;

            Bone& bone = _skeleton->getBoneVector()[animationData->_childrens[i]._boneIndex];
            glm::vec3 axis = bone._orientation;
            animationData->_childrens[i].__c = glm::rotate(glm::mat4(1.0f), glm::radians(axis[0]), glm::vec3(1.0f,0.0f,0.0f));
            animationData->_childrens[i].__c = glm::rotate(animationData->_childrens[i].__c, glm::radians(axis[1]), glm::vec3(0.0f,1.0f,0.0f));
            animationData->_childrens[i].__c = glm::rotate(animationData->_childrens[i].__c, glm::radians(axis[2]), glm::vec3(0.0f,0.0f,1.0f));

            glm::vec3 dir = bone._direction * _skeleton->getGBL() * bone._length;
            animationData->__b = glm::translate(glm::mat4(1.0f), dir);
        }
    }
    return true;
}