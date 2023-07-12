#include "include/Simulator.h"
#include "include/Animation.h"
#include "include/Skeleton.h"
#include "include/GL/glew.h"
#include "include/Cylinder.h"
#include "include/AnimationCompressor.h"
#include "include/GLM/gtx/spline.hpp"
#include "include/GLM/gtx/quaternion.hpp"
#include "include/Line.h"
#include <algorithm>
#include <queue>
const float compressMul[] = {0 ,10.5, 94.6615, 355.184};
void Simulator::initialize(void)
{
    int64 size = _skeleton.getBoneVector().size();

    VAO.resize(size);
    VBO.resize(size);
    VBC.resize(size);
    _transForm.resize(size);
    boneBufferMaping();
    _keyCount = _animations[0]._rootNode._localRotation.size();

    AnimationCompressor compressor;
    // 압축률 추출기
    // float mul = 0;
    // int count = 0;
    // for (int i = 0; i <= 60; i += 20)
    // {
    //     while (true)
    //     {
    //         std::vector<uint32> v = compressor.getCompressedData(_animation, mul / 100000000.0f);
    //         if ((_keyCount-v.size()) * 100 / (_keyCount) >= i)
    //         {
    //             std::cout << (_keyCount-v.size()) * 100 / (_keyCount ) << "%" << std::endl;
    //             std::cout << "mul : " << mul << std::endl;
    //             _compresskeyFrame[count] = v;
    //             count++;
    //             mul++;
    //             break;
    //         }
    //         mul *= 1.3;
    //     }
    // }
    for (int i =0; i < 4; ++i)
        _compresskeyFrame[i] = compressor.getCompressedData(&_animations[0], compressMul[i] / 100000000.0f);
}

void Simulator::boneBufferMaping(void)
{
    std::vector<Bone>& boneVector = _skeleton.getBoneVector();
    uint32 boneIndex = 0;
    for(Bone& bone : boneVector)
    {
        glGenVertexArrays(1, &VAO[bone._boneIndex]);
        glBindVertexArray(VAO[bone._boneIndex]);

        glGenBuffers(1, &VBO[bone._boneIndex]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[bone._boneIndex]);
        glEnableVertexAttribArray(0);	
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);//size 열의 개수
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glGenBuffers(1, &VBC[bone._boneIndex]);
        glBindBuffer(GL_ARRAY_BUFFER, VBC[bone._boneIndex]);
        glEnableVertexAttribArray(1);	
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);//size 열의 개수
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);
    }
}

void Simulator::draw(uint32 animationTime, uint32 shaderProgram)
{
    std::vector<Bone>& boneVector = _skeleton.getBoneVector();

    for (int i =0; i <=0; ++i)//comprees test
    {
        update(animationTime, i);
        glm::vec3 color(0.0f);
        color[i] = 1;
        for(Bone& bone : boneVector)
        {
            glBindVertexArray(VAO[bone._boneIndex]);
            glm::mat4 toParentDir = _modelPos * _transForm[bone._boneIndex] * ft_rotate(glm::vec3(0.0,0.0,1.0), bone._direction);// * glm::inverse(test3); 
            // Cylinder cylinder(0.2, 0.7 *_skeleton.getGBL() * bone._length ,16, toParentDir);
            // cylinder.initialize();
            // cylinder.render(VBO[bone._boneIndex]);
            Line line(0.7 *_skeleton.getGBL() * bone._length, toParentDir);
            line.initialize(color, VBC[bone._boneIndex]);
            line.render(VBO[bone._boneIndex]);
            glBindVertexArray(0);
        }
    }
}

void Simulator::updateTransForm(const AnimationData& node, glm::mat4 wolrdTrans, float* keyArray)
{
    glm::quat t0 = node._localRotation[keyArray[0]];
    glm::quat t1 = node._localRotation[keyArray[1]];
    glm::quat t2 = node._localRotation[keyArray[2]];
    glm::quat t3 = node._localRotation[keyArray[3]];
    glm::quat interpolR = glm::catmullRom(t0,t1,t2,t3, (keyArray[4] -keyArray[1])/(keyArray[2]-keyArray[1]));
    glm::quat t4 = node._localRotation[keyArray[4]];

    glm::mat4 interpolT = glm::catmullRom(//trans vec3로 바꿔야하나
                        node._localTrans[keyArray[0]],
                        node._localTrans[keyArray[0]],
                        node._localTrans[keyArray[0]],
                        node._localTrans[keyArray[0]],
                        (keyArray[4] -keyArray[1])/(keyArray[2]-keyArray[1])
                    );
    wolrdTrans = wolrdTrans * interpolT * glm::toMat4(interpolR);
    _transForm[node._boneIndex] = wolrdTrans;
    for (const AnimationData& child : node._childrens)
        updateTransForm(child, wolrdTrans, keyArray);
}

void Simulator::update(uint32 keyTime, uint32 animationIndex)//나중에 압축 데이터를 직접 넣어야 하나?
{
    float keyArray[5];
    keyArray[4] = keyTime;
    std::vector<uint32>::iterator it;
    it = std::lower_bound(_compresskeyFrame[animationIndex].begin(), _compresskeyFrame[animationIndex].end(), keyTime) + 1;
    for (int i =3; i >=0; --i)
    {
        if (it == _compresskeyFrame[animationIndex].end())
        {
            it--;
            keyArray[i] = *it;
        }
        else if (it == _compresskeyFrame[animationIndex].begin())
            keyArray[i] = *it;
        else
        {
            keyArray[i] = *it;
            it--;
        }
    }

    updateTransForm(_animations[0]._rootNode, glm::mat4(1.0f), keyArray);
    updateTransForm(*_animations[1].returnAnimationData(11/*lowerback*/), _transForm[0], keyArray);
    if (keyTime == 1)//0 nan
    {
        glm::vec4 trans = _transForm[0] * glm::vec4(0,0,0,1);
        _modelPos = glm::translate(_modelPos, -(glm::vec3)trans);
    } else if (keyTime == _keyCount-1){
        glm::vec4 trans = _transForm[0] * glm::vec4(0,0,0,1); 
        _modelPos = glm::translate(_modelPos, (glm::vec3)trans);
    }
}