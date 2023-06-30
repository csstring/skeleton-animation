#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "Animation.h"//fixme
#include "Bone.h"//fixme
//class Animation;
//class Bone;

class Skeleton
{
private:
    std::string _programName;
    bool _isDeg;
    float _mass;
    float _globalLengthMultiplier;
    std::vector<Bone> _boneVector;
    
public:
    Skeleton(/* args */) : _isDeg(false){};
    ~Skeleton(){};

    //setter
    void setIsDeg(bool isDeg){ _isDeg = isDeg;};
    void setMass(float mass){ _mass = mass;};
    void setGlobalLengthMultiplier(float globalLengthMultiplier){_globalLengthMultiplier = globalLengthMultiplier;};
    void setProgramName(std::string programname) {_programName = programname;};
    //getter
    bool getIsDeg(void){ return _isDeg;};
    float getGBL(void){ return _globalLengthMultiplier;};


    std::vector<Bone>& getBoneVector(void){return _boneVector;};
    void draw(void) const;
    int32 findBoneIndex(const std::string& name) const;
};