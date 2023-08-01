#include "../include/IK/IKUtility.h"
#include "../include/Bone.h"
#include "../include/EnumHeader.h"

bool limitAngleCheck(const Bone& bone, const glm::quat& boneRot)
{
    glm::vec3 eulerAngle = glm::eulerAngles(boneRot);

    for (auto& limit : bone._limits)
    {
        DOF dof;
        float min, max;
        std::tie(dof, min, max) = limit;
        if (dof == DOF::RX && (eulerAngle.x < min || max < eulerAngle.x))
            return false;
        else if (dof == DOF::RY && (eulerAngle.y < min || max < eulerAngle.y))
            return false;
        else if (dof == DOF::RZ && (eulerAngle.z < min || max < eulerAngle.z))
            return false;
    }
    return true;
}