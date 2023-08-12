#pragma once
#include "include/Common.h"
using namespace physx;
class ContactCallBack : public PxSimulationEventCallback
{
    private:
        /* data */
    public:
        ContactCallBack(){};
        virtual ~ContactCallBack(){};
        virtual void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs);
        
        virtual void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count){}
        virtual void onWake(PxActor** actors, PxU32 count){}
        virtual void onSleep(PxActor** actors, PxU32 count){}
        virtual void onTrigger(PxTriggerPair* pairs, PxU32 count){}
        virtual void onAdvance(const PxRigidBody*const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count){}
};
