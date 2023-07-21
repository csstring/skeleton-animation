#include "include/CmuFileParser.h"
#include "include/Skeleton.h"
#include "include/Animation.h"
#include "include/Bone.h"
#include "include/GLM/gtx/transform.hpp"
#include "fstream"

bool CmuFileParser::loadCmuFile(void)
{
    std::ifstream ifs(_filePath);

    if (ifs.is_open() == false) 
        return false;

    while (ifs.good())
    {
        char firstCh = ifs.get();
        std::string buffer;
        
        if (isspace(firstCh)) 
            continue;
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
            parseAsfUnits(ifs);
        else if (buffer == "documentation") //정확한 형식 파악 불가 추후재검토
        {
            std::getline(ifs, buffer);
            std::getline(ifs, buffer);
            std::getline(ifs, buffer);
        }
        else if (buffer == "root")
            parseAsfRoot(ifs);
        else if (buffer == "bonedata") //parsing hierarchy
            parseAsfBoneData(ifs);
        else 
        {
            break;
        }
    }
    ifs.close();
    return true;
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
    bone._length = 0;
    bone._direction = glm::vec3(0.0f);

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
                else if (buffer == "RZ") bone._dof.push_back(DOF::RZ);
                else if (buffer == "TX") bone._dof.push_back(DOF::TX);
                else if (buffer == "TY") bone._dof.push_back(DOF::TY);
                else if (buffer == "TZ") bone._dof.push_back(DOF::TZ);
                else break;
            }
        }
        else if (buffer == "position")
        {
            ifs >> bone._postion.x;
            ifs >> bone._postion.y;
            ifs >> bone._postion.z;
        }
        else if (buffer == "orientation")
        {
            ifs >> bone._axis.x;
            ifs >> bone._axis.y;
            ifs >> bone._axis.z;
            
            if (_skeleton->getIsDeg())
            {
                bone._axis.x *= PI / 180.0;
                bone._axis.y *= PI / 180.0;
                bone._axis.z *= PI / 180.0;
            }
        }
    }
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
                ifs >> bone._boneIndex;
            else if (buffer == "name")
                ifs >> bone._boneName;
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
                ifs >> bone._axis.x;
                ifs >> bone._axis.y;
                ifs >> bone._axis.z;
                
                if (_skeleton->getIsDeg())
                {
                    bone._axis.x *= PI / 180.0;
                    bone._axis.y *= PI / 180.0;
                    bone._axis.z *= PI / 180.0;
                }
                std::getline(ifs, buffer); //xyz parsing
            }
            else if (buffer == "dof")
                parserAsfDof(ifs, bone);
        }
        _skeleton->getBoneVector().push_back(bone);
    }

    return true;
}

bool CmuFileParser::parserAsfDof(std::ifstream& ifs, Bone& bone)
{
    std::string buffer;

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
    }
    for (DOF dof : bone._dof)
    {
        std::getline(ifs, buffer, '(');
        float min, max;
        ifs >> min, ifs >> max;
        bone._limits.push_back({dof, min, max});
        std::getline(ifs, buffer);
    }
    
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
        AnimationData* animationData = _animation->returnAnimationData(boneIndex);

        if (boneIndex == -1)
            ft_assert("we can't find bone name");

        //root
        if (boneIndex == 0)
        {
            Bone& bone = _skeleton->getBoneVector()[animationData->_boneIndex];
            bone._parentBoneIndex = 0;
            glm::vec3 axis = bone._axis;
            
            glm::mat4 rotX = glm::rotate(axis[0], glm::vec3(1.0f,0.0f,0.0f));
            glm::mat4 rotY = glm::rotate(axis[1], glm::vec3(0.0f,1.0f,0.0f));
            glm::mat4 rotZ = glm::rotate(axis[2], glm::vec3(0.0f,0.0f,1.0f));

            glm::mat4 c = rotZ * rotY * rotX * glm::mat4(1.0f);
            bone._c = glm::quat_cast(c);
            bone._invC = glm::quat_cast(glm::inverse(c));
            bone._b = glm::vec3(bone._postion.x, bone._postion.y, bone._postion.z);
        }

        //other
        animationData->_childrens.resize(v.size());
        for (int i =0; i < v.size(); ++i)
        {
            animationData->_childrens[i]._boneIndex = _skeleton->findBoneIndex(v[i]);

            Bone& bone = _skeleton->getBoneVector()[animationData->_childrens[i]._boneIndex];
            bone._parentBoneIndex = boneIndex;
            glm::vec3 axis = bone._axis;
//mat4 fixme
            glm::mat4 rotX = glm::rotate(axis[0], glm::vec3(1.0f,0.0f,0.0f));
            glm::mat4 rotY = glm::rotate(axis[1], glm::vec3(0.0f,1.0f,0.0f));
            glm::mat4 rotZ = glm::rotate(axis[2], glm::vec3(0.0f,0.0f,1.0f));

            glm::mat3 c = rotZ * rotY * rotX;
            bone._c = glm::quat_cast(c);
            bone._invC = glm::quat_cast(glm::inverse(c));

            glm::vec3 dir = _skeleton->getGBL() * bone._length * bone._direction;
            bone._b = glm::vec3(dir);
        }
    }
    return true;
}