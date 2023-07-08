#include "include/Simulator.h"
#include "include/Animation.h"
#include "include/Skeleton.h"
#include "include/AnimationDataMatrixSetup.h"
#include "include/GL/glew.h"
#include "include/Cylinder.h"
#include <queue>

void Simulator::initialize(void)
{
    int64 size = _skeleton->getBoneVector().size();

    VAO.resize(size);
    VBO.resize(size);
    TVBO.resize(size);
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
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);//size 열의 개수
        // glGenBuffers(1, &TVBO[index]);
        // glBindBuffer(GL_ARRAY_BUFFER, TVBO[index]);
        // glEnableVertexAttribArray(0);	
        // glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);//size 열의 개수

        glBindVertexArray(0);

        for (int i =0; i < curData->_childrens.size(); ++i)
            dataQueue.push(&curData->_childrens[i]);
        index++;
    }
}
#include "include/GLM/gtx/quaternion.hpp"
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
        Bone bone =_skeleton->getBoneVector()[curData->_boneIndex];
        glm::mat4 toParentDir = curData->_worldTrans[animationTime] * ft_rotate(glm::vec3(0.0,0.0,1.0), bone._direction);// * glm::inverse(test3); 
        Cylinder cylinder(0.2, 0.7 *_skeleton->getGBL() * bone._length ,16, toParentDir);
        cylinder.initialize();
        cylinder.render(VBO[index]);
        glBindVertexArray(0);

        for (int i =0; i < curData->_childrens.size(); ++i)
            dataQueue.push(&curData->_childrens[i]);
        index++;
    }
}