#pragma once
#include <string>
#include <vector>
#include "Bone.h"
#include "EnumHeader.h"

class Skeleton
{
    private:
    public:
        Skeleton() : _isDeg(false){};
        ~Skeleton(){};

        //setter
        void setIsDeg(bool isDeg){ _isDeg = isDeg;};
        void setMass(float mass){ _mass = mass;};
        void setGlobalLengthMultiplier(float globalLengthMultiplier){_globalLengthMultiplier = globalLengthMultiplier;};
        void setProgramName(std::string programname) {_programName = programname;};
        
        //getter
        bool getIsDeg(void) const { return _isDeg;};
        float getGBL(void) const { return _globalLengthMultiplier;};
        std::vector<Bone>& getBoneVector(void){return _boneVector;};
        const std::vector<Bone>& getBoneVector(void) const {return _boneVector;};
        int32 findBoneIndex(const std::string& name) const;
        float getSkeletonHeight(void);
        float getSkeletonWidth(void);
        glm::vec3 getCharLocalPosition(BONEID boneID);
    private:
        std::string       _programName;
        bool              _isDeg;
        float             _mass;
        float             _globalLengthMultiplier;
        std::vector<Bone> _boneVector;
        
};