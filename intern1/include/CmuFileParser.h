#pragma once
#include "Common.h"
#include <string>
class Skeleton;
struct Bone;
class Animation;

static const char* asfPath = "./asf/02.asf";
class CmuFileParser : Noncopyable
{
    public : 
        explicit CmuFileParser(
            std::string filePath, 
            Skeleton* skeleton,
            Animation* animation
        ) : _filePath(filePath), _skeleton(skeleton), _animation(animation){};

        ~CmuFileParser()
        {
            _skeleton = NULL;
            _animation = NULL;
        };

        bool              loadCmuFile();
        bool              parseAsfUnits(std::ifstream& ifs);
        bool              parseAsfRoot(std::ifstream& ifs);
        bool              parseAsfBoneData(std::ifstream& ifs);
        bool              parseAsfHierarchy(std::ifstream& ifs);
        bool              parserAsfDof(std::ifstream& ifs, Bone& bone);

    private:
        const std::string _filePath;
        Skeleton*         _skeleton;
        Animation*        _animation;
};