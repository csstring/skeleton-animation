#include "include/ContactCallBack.h"
void ContactCallBack::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
{
    PX_UNUSED((pairHeader));
    std::vector<PxContactPairPoint> contactPoints;
    std::cout << "contact count : " <<nbPairs << std::endl;

    for(PxU32 i=0;i<nbPairs;i++)
    {
        PxU32 contactCount = pairs[i].contactCount;
        if(contactCount)
        {
            contactPoints.resize(contactCount);
            pairs[i].extractContacts(&contactPoints[0], contactCount);

            for(PxU32 j=0;j<contactCount;j++)
            {
                // gContactPositions.push_back(contactPoints[j].position);
                // gContactImpulses.push_back(contactPoints[j].impulse);
            }
        }
    }
}