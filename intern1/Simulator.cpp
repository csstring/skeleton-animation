#include "include/Simulator.h"
#include "include/Animation.h"
#include "include/Skeleton.h"
#include "include/AnimationDataMatrixSetup.h"
#include "include/GL/glew.h"
#include <queue>

void Simulator::initialize(void)
{
    int64 size = _skeleton->getBoneVector().size();

    VAO.resize(size);
    VBO.resize(size);
    _total = _animation->_rootNode._localRotation.size();
}

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
    
        glEnableVertexAttribArray(0);	
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);//size 열의 개수
        glVertexAttribDivisor(0, 1);

        glBindVertexArray(0);

        for (int i =0; i < curData->_childrens.size(); ++i)
            dataQueue.push(&curData->_childrens[i]);
        index++;
    }
}

void Simulator::draw(uint32 animationTime, uint32 shaderProgram)
{
    std::queue<AnimationData*> dataQueue;
    std::vector<Bone>& boneVector = _skeleton->getBoneVector();
    uint32 index = 0;

    dataQueue.push(&_animation->_rootNode);
    while (dataQueue.size() != 0)
    {
        AnimationData* curData = dataQueue.front();
        dataQueue.pop();

        glBindVertexArray(VAO[index]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[index]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(curData->_pos[animationTime]), &curData->_pos[animationTime], GL_STREAM_DRAW);
        glDrawArrays(GL_POINTS,0,1);
        glBindVertexArray(0);

        for (int i =0; i < curData->_childrens.size(); ++i)
            dataQueue.push(&curData->_childrens[i]);
        index++;
    }
}