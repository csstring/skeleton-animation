#pragma once
#include "AnimationTreeTraversal.h"
#include "common.h"

class AnimationDataResize : public AnimationTreeTraversal
{
    private:
        uint32 _size;
    public :
        AnimationDataResize(uint32 size) : _size(size){}
        virtual void processNode(AnimationData& data) override;
};
