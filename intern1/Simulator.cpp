#include "include/Simulator.h"
#include "include/Animation.h"
#include "include/Skeleton.h"
#include <queue>
#include "include/AnimationDataMatrixSetup.h"
#include "include/GL/glew.h"

void Simulator::setupModelMatrix(void)
{
    AnimationDataMatrixSetup matrixSetup;
    _animation->AnimationDataTraver(matrixSetup);
}

void Simulator::animationDataMaping(void)
{
    std::queue<AnimationData*> dataQueue;
    std::vector<Bone>& boneVector = _skeleton->getBoneVector();
    uint32 index = 0;

    dataQueue.push(&_animation->_rootNode);
    while (dataQueue.size() != 0)
    {
        AnimationData* curData = dataQueue.front();
        dataQueue.pop();

        glGenVertexArrays(1, &VAO[index]);
        glBindVertexArray(VAO[index]);

        glGenBuffers(1, &VBO[index]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[index]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(boneVector[index]._postion), &boneVector[index]._postion, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);	
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glBindVertexArray(0);
        for (int i =0; i < curData->_childrens.size(); ++i)
            dataQueue.push(&curData->_childrens[i]);
        index++;
    }
}

void Simulator::draw(void) const
{
    for (uint32 vao : VAO)
    {
        glBindVertexArray(vao);
        glDrawArrays(GL_POINTS,0,1);
        glBindVertexArray(0);
    }
}