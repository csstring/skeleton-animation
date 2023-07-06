#pragma once
#include "AnimationTreeTraversal.h"

class AnimationDataMatrixSetup: public AnimationTreeTraversal
{
    public :
        virtual void processNode(AnimationData& data) override;
};
