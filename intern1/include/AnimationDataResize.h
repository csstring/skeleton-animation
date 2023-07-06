#pragma once
#include "AnimationTreeTraversal.h"
#include "common.h"

class AnimationDataResize : public AnimationTreeTraversal
{
    public :
        explicit AnimationDataResize(uint32 size) : _size(size){}
        virtual void processNode(AnimationData& data) override;
    
    private:
        uint32 _size;
};
