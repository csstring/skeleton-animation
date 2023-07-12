#pragma once
#include "common.h"
#include <string>
class Skeleton;
class Animation;
struct AnimationData;

class AMCFileParser : Noncopyable
{
    public : 
        explicit AMCFileParser(std::string filePath, Skeleton* skeleton, Animation* animation) 
                : _filePath(filePath), _skeleton(skeleton), _animation(animation), _total(0){};
                
                ~AMCFileParser()
                {
                    _skeleton = NULL;
                    _animation = NULL;
                };
        bool    loadAMCFile();
    private:
        void    boneDataindexing(std::vector<AnimationData*>& indexVector);

    private:
        const std::string _filePath;
        Skeleton*         _skeleton;
        Animation*        _animation;
        uint32            _total;

};