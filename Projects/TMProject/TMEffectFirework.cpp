#include "pch.h"
#include "TMEffectFirework.h"
#include "TMEffectBillBoard.h"
#include "TMGlobal.h"

namespace
{
	constexpr unsigned int kRocketTime = 700;
	constexpr float kRocketHeight = 4.0f;
	constexpr int kSparks = 60;
	constexpr int kSparkTexture = 56;

	// Burst colors: the server sends a type from 0 to 7.
	constexpr unsigned int kColors[8][2] =
	{
		{ 0xFFFF4030, 0xFFFFC040 },
		{ 0xFF40FF50, 0xFFC0FF40 },
		{ 0xFF4070FF, 0xFF40E0FF },
		{ 0xFFFFE040, 0xFFFFFFFF },
		{ 0xFFFF40E0, 0xFFFF90C0 },
		{ 0xFF40FFE0, 0xFF4090FF },
		{ 0xFFB040FF, 0xFFFF40A0 },
		{ 0xFFFFFFFF, 0xFFFF8030 },
	};

	float RandUnit()
	{
		return static_cast<float>(rand() % 1001) * 0.001f;
	}

	TMEffectBillBoard* NewSpark(unsigned int dwLifeTime, float fScale, unsigned int dwColor, TMVector3 vecStart)
	{
		auto pSpark = new TMEffectBillBoard(kSparkTexture, dwLifeTime, fScale, fScale, fScale, 0.0f, 1, 80);
		pSpark->m_efAlphaType = EEFFECT_ALPHATYPE::EF_BRIGHT;
		pSpark->m_vecStartPos = vecStart;
		pSpark->m_vecPosition = vecStart;
		pSpark->SetColor(dwColor);
		g_pCurrentScene->m_pEffectContainer->AddChild(pSpark);
		return pSpark;
	}
}

TMEffectFireWork::TMEffectFireWork(TMVector3 vecPositiont, int nType) : TreeNode(0)
{
	m_vecPosition = vecPositiont;
	m_nType = (nType % 8 + 8) % 8;
	m_dwColor = kColors[m_nType][0];
	m_dwStartTime = g_pTimerManager->GetServerTime();
	m_bBomb = 0;
	m_nColorType = 0;

	if (!g_pCurrentScene || !g_pCurrentScene->m_pEffectContainer)
		return;

	auto pRocket = NewSpark(kRocketTime, 0.35f, 0xFFFFE0A0, m_vecPosition);
	pRocket->m_nParticleType = 1; // rises m_fParticleV during its lifetime
	pRocket->m_fParticleV = kRocketHeight;
}

TMEffectFireWork::~TMEffectFireWork()
{
}

int TMEffectFireWork::FrameMove(unsigned int dwServerTime)
{
	dwServerTime = g_pTimerManager->GetServerTime();

	if (m_bBomb || dwServerTime - m_dwStartTime < kRocketTime)
		return 1;

	m_bBomb = 1;
	g_pObjectManager->DeleteObject(this);

	if (!g_pCurrentScene || !g_pCurrentScene->m_pEffectContainer)
		return 1;

	TMVector3 vecCenter{ m_vecPosition.x, m_vecPosition.y + kRocketHeight, m_vecPosition.z };

	NewSpark(500, 3.0f, kColors[m_nType][1], vecCenter);

	for (int i = 0; i < kSparks; ++i)
	{
		// Uniform direction on a sphere; the billboard moves 5 units along it.
		float fY = 2.0f * RandUnit() - 1.0f;
		float fAngle = RandUnit() * 2.0f * 3.1415927f;
		float fRing = sqrtf(1.0f - fY * fY);
		float fReach = 0.9f + RandUnit() * 0.5f;

		auto pSpark = NewSpark(1200 + rand() % 800, 0.3f + RandUnit() * 0.2f, kColors[m_nType][i % 2], vecCenter);
		pSpark->m_nParticleType = 17;
		pSpark->m_vecDir = TMVector3{ cosf(fAngle) * fRing * fReach, fY * fReach, sinf(fAngle) * fRing * fReach };
	}

	return 1;
}

int TMEffectFireWork::Render()
{
	return 1;
}

Particle* TMEffectFireWork::SetParticle(Particle* result, unsigned short wType, float fLifeTime, TMVector3 vBasePosition, TMVector3 vBaseVelocity)
{
	return nullptr;
}

void TMEffectFireWork::SetCustomFireWork(char* pBuffer)
{
}
