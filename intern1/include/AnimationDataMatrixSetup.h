#pragma once
#include "AnimationTreeTraversal.h"
#include "common.h"

class AnimationDataMatrixSetup: public AnimationTreeTraversal
{
    public :
        virtual void processNode(AnimationData& data) override;
};
