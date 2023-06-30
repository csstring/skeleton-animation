#pragma once
#include "common.h"
#include <string>
class Skeleton;
class Bone;
class Animation;
class CmuFileParser : Noncopyable
{
    private:
        const std::string _filePath;
        Skeleton*         _skeleton;
        Animation*        _animation;
    public : 
        explicit CmuFileParser(
            std::string filePath, 
            Skeleton* skeleton,
            Animation* animation
        ) : _filePath(filePath), _skeleton(skeleton), _animation(animation)
        {};
        ~CmuFileParser()
        {
            _skeleton = NULL;
            _animation = NULL;
        };

        bool loadCmuFile();
        bool parseAsfVersion();
        bool parseAsfName();
        bool parseAsfUnits(std::ifstream& ifs);
        bool parseAsfDocumentation();
        bool parseAsfRoot(std::ifstream& ifs);
        bool parseAsfBoneData(std::ifstream& ifs);
        bool parseAsfHierarchy(std::ifstream& ifs);

        bool parserAsfDof(std::ifstream& ifs, Bone& bone);
};