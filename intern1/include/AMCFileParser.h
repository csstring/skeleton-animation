#pragma once
#include "common.h"
#include <string>
class Skeleton;
class Bone;
class Animation;
class AMCFileParser : Noncopyable
{
    private:
        const std::string _filePath;
        Skeleton*         _skeleton;
        Animation*        _animation;
        uint32             _total;
    public : 
        explicit AMCFileParser(
            std::string filePath, 
            Skeleton* skeleton,
            Animation* animation
        ) : _filePath(filePath), _skeleton(skeleton), _animation(animation), _total(0)
        {};
        ~AMCFileParser()
        {
            _skeleton = NULL;
            _animation = NULL;
        };

        bool loadAMCFile();

};