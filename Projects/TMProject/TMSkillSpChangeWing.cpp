#include "pch.h"
#include "TMSkillSpChangeWing.h"
#include "TMObject.h"
#include "TMShade.h"
#include "TMEffectParticle.h"
#include "TMEffectSkinMesh.h"
#include "TMHuman.h"
#include "TMGlobal.h"

TMSkillSpChangeWing::TMSkillSpChangeWing(TMVector3 vecPosition, int nType, TMObject* pOwner)
{
    m_nType = nType;
    m_pOwner = (TMHuman*)pOwner;
    m_vecPosition = vecPosition;
    m_dwStartTime = g_pTimerManager->GetServerTime();
    m_dwLifeTime = 100;

    if (m_nType == 0)
        m_dwLifeTime = 1000;

    if (m_nType == 1)
        m_dwLifeTime = 990;


    if (pOwner)
        m_fAngle = pOwner->m_fAngle;
    else
        m_fAngle = 0.0f;

    int nLevel = 3;

    if (!nType)
    {
        for (int i = 0; i < 1; ++i)
        {
            auto pWing = new TMEffectSkinMesh(86, vecPosition, vecPosition, nLevel, pOwner);

            if (pWing != nullptr)
            {
                pWing->m_stLookInfo.Mesh1 = 0;
                pWing->m_stLookInfo.Mesh0 = 0;
                pWing->m_stLookInfo.Skin1 = 0;
                pWing->m_stLookInfo.Skin0 = 0;
                pWing->m_nMotionType = 11;
                pWing->InitObject(0);
                pWing->m_dwStartTime = m_dwStartTime + 400 * i;

                if (pWing->m_pSkinMesh)
                {
                    pWing->m_pSkinMesh->m_dwStartOffset = pWing->m_dwStartTime;

                    pWing->m_pSkinMesh->m_vScale.x = 1.1f;
                    pWing->m_pSkinMesh->m_vScale.y = 1.1f;
                    pWing->m_pSkinMesh->m_vScale.z = 1.1f;
                }

                pWing->m_dwLifeTime = m_dwLifeTime - 100 * i;
                /*pWing->m_efAlphaType = EEFFECT_ALPHATYPE::EF_NONEBRIGHT;
               pWing->m_StartColor.r = ((float)i * 0.40000001f) + 0.2f;
                pWing->m_StartColor.g = ((float)i * 0.40000001f) + 0.2f;
                pWing->m_StartColor.b = ((float)i * 0.40000001f) + 0.2f;
                pWing->m_EndColor.r = ((float)i * 0.40000001f) + 0.2f;
                pWing->m_EndColor.g = ((float)i * 0.40000001f) + 0.2f;
                pWing->m_EndColor.b = ((float)i * 0.40000001f) + 0.2f;*/
                pWing->m_nFade = 3;
                pWing->m_fAngle = m_fAngle;
                g_pCurrentScene->m_pEffectContainer->AddChild(pWing);
            }
        }
    }

    m_pParticle = nullptr;
}
TMSkillSpChangeWing::~TMSkillSpChangeWing()
{
    if (m_pLightMap != nullptr)
        g_pObjectManager->DeleteObject(m_pLightMap);
}