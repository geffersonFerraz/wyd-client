#include "pch.h"
#include "TMEffectFirework.h"
#include "TMGlobal.h"

namespace
{
	constexpr float kRocketTime = 0.7f;      // seconds until the burst
	constexpr float kStartDrop = 2.0f;       // rocket start, below the position it is given
	constexpr float kRocketRise = 3.0f;
	constexpr float kRocketDrift = 1.4f;     // max sideways travel of the rocket
	constexpr float kRocketLength = 1.4f;
	constexpr float kRocketWidth = 0.13f;
	constexpr float kFlashTime = 0.15f;
	constexpr float kWhiteTime = 0.2f;       // sparks start white-hot
	constexpr float kSparkSize = 0.11f;      // half size
	constexpr float kSparkSpeed = 6.5f;
	constexpr float kSparkDrag = 3.0f;       // opens fast, then almost stops
	constexpr float kGravity = 2.4f;
	constexpr float kMaxLife = 3.0f;
	constexpr int kSparks = 100;
	constexpr int kSparkTexture = 56;

	// One color per burst: the server sends a type from 0 to 7.
	constexpr unsigned int kColors[8] =
	{
		0xFF80FF90, 0xFFFFE0B0, 0xFFFFA0D0, 0xFF80B0FF,
		0xFFFFF080, 0xFFC090FF, 0xFF80FFF0, 0xFFFF9070,
	};

	float RandUnit()
	{
		return static_cast<float>(rand() % 1001) * 0.001f;
	}

	unsigned int MixColor(unsigned int dwColor, float fWhite, float fFade)
	{
		unsigned int dwOut = static_cast<unsigned int>(fFade * 255.0f) << 24;
		for (int nShift = 0; nShift <= 16; nShift += 8)
		{
			float fChannel = static_cast<float>((dwColor >> nShift) & 0xFF);
			fChannel += (255.0f - fChannel) * fWhite;
			dwOut |= static_cast<unsigned int>(fChannel * fFade) << nShift;
		}
		return dwOut;
	}

	TMVector3 Offset(const TMVector3& vec, const TMVector3& vDir, float fAmount)
	{
		return TMVector3{ vec.x + vDir.x * fAmount, vec.y + vDir.y * fAmount, vec.z + vDir.z * fAmount };
	}

	void PushQuad(RDLVERTEX* pOut, int& nCount, const TMVector3 (&vCorner)[4], unsigned int dwColor)
	{
		static const int nOrder[6] = { 0, 1, 2, 0, 2, 3 };
		static const float fU[4] = { 0.02f, 0.98f, 0.98f, 0.02f };
		static const float fV[4] = { 0.98f, 0.98f, 0.02f, 0.02f };

		for (int nIndex : nOrder)
		{
			RDLVERTEX& vertex = pOut[nCount++];
			vertex.position = vCorner[nIndex];
			vertex.diffuse = dwColor;
			vertex.tu = fU[nIndex];
			vertex.tv = fV[nIndex];
		}
	}

	void PushSprite(RDLVERTEX* pOut, int& nCount, const TMVector3& vCenter, const TMVector3& vRight, const TMVector3& vUp, float fSize, unsigned int dwColor)
	{
		TMVector3 vLeft = Offset(vCenter, vRight, -fSize);
		TMVector3 vOther = Offset(vCenter, vRight, fSize);
		const TMVector3 vCorner[4] = { Offset(vLeft, vUp, -fSize), Offset(vOther, vUp, -fSize), Offset(vOther, vUp, fSize), Offset(vLeft, vUp, fSize) };
		PushQuad(pOut, nCount, vCorner, dwColor);
	}
}

TMEffectFireWork::TMEffectFireWork(TMVector3 vecPositiont, int nType) : TreeNode(0)
{
	m_vecPosition = TMVector3{ vecPositiont.x, vecPositiont.y - kStartDrop, vecPositiont.z };
	m_vecEnd = TMVector3
	{
		m_vecPosition.x + (RandUnit() * 2.0f - 1.0f) * kRocketDrift,
		m_vecPosition.y + kRocketRise,
		m_vecPosition.z + (RandUnit() * 2.0f - 1.0f) * kRocketDrift
	};
	m_fElapsed = 0.0f;
	m_nType = (nType % 8 + 8) % 8;
	m_dwColor = kColors[m_nType];
	m_dwStartTime = g_pTimerManager->GetServerTime();
	m_bBomb = 0;
	m_nColorType = 0;

	for (int i = 0; i < kSparks; ++i)
	{
		// Uniform direction on a sphere, for a shell of sparks.
		float fY = 2.0f * RandUnit() - 1.0f;
		float fAngle = RandUnit() * 2.0f * 3.1415927f;
		float fRing = sqrtf(1.0f - fY * fY);
		float fSpeed = kSparkSpeed * (0.85f + RandUnit() * 0.3f);

		Particle& spark = m_Particle[i];
		spark.vInitialPosition = m_vecEnd;
		spark.vPosition = m_vecEnd;
		spark.vInitialVelocity = TMVector3{ cosf(fAngle) * fRing * fSpeed, fY * fSpeed, sinf(fAngle) * fRing * fSpeed };
		spark.vLaunchVelocity = spark.vInitialVelocity;
		spark.fLifetime = 2.2f + RandUnit() * (kMaxLife - 2.2f);
		spark.fMaturity = 0.0f;
		spark.wType = 0;
		spark.fSize = kSparkSize;
	}
}

TMEffectFireWork::~TMEffectFireWork()
{
}

int TMEffectFireWork::FrameMove(unsigned int dwServerTime)
{
	dwServerTime = g_pTimerManager->GetServerTime();
	m_fElapsed = static_cast<float>(dwServerTime - m_dwStartTime) * 0.001f;

	if (m_fElapsed < kRocketTime)
		return 1;

	m_bBomb = 1;

	float fAge = m_fElapsed - kRocketTime;
	if (fAge >= kMaxLife)
	{
		g_pObjectManager->DeleteObject(this);
		return 1;
	}

	float fOpen = (1.0f - expf(-kSparkDrag * fAge)) / kSparkDrag;
	float fFall = 0.5f * kGravity * fAge * fAge;

	for (int i = 0; i < kSparks; ++i)
	{
		Particle& spark = m_Particle[i];
		spark.fMaturity = fAge;
		spark.vPosition = Offset(spark.vInitialPosition, spark.vInitialVelocity, fOpen);
		spark.vPosition.y -= fFall;
	}

	return 1;
}

int TMEffectFireWork::Render()
{
	if (g_bHideEffect == 1 || g_bHideBackground == 1)
		return 1;

	const D3DXMATRIX& matView = g_pDevice->m_matView;
	const TMVector3 vRight{ matView._11, matView._21, matView._31 };
	const TMVector3 vUp{ matView._12, matView._22, matView._32 };
	const TMVector3 vForward{ matView._13, matView._23, matView._33 };

	RDLVERTEX vertices[(kSparks + 1) * 6];
	int nCount = 0;

	if (!m_bBomb)
	{
		float fProgress = m_fElapsed / kRocketTime;
		float fTravel = fProgress * (1.4f - 0.4f * fProgress);

		TMVector3 vDir{ m_vecEnd.x - m_vecPosition.x, m_vecEnd.y - m_vecPosition.y, m_vecEnd.z - m_vecPosition.z };
		float fDistance = sqrtf(vDir.x * vDir.x + vDir.y * vDir.y + vDir.z * vDir.z);
		vDir = TMVector3{ vDir.x / fDistance, vDir.y / fDistance, vDir.z / fDistance };

		TMVector3 vSide{ vDir.y * vForward.z - vDir.z * vForward.y, vDir.z * vForward.x - vDir.x * vForward.z, vDir.x * vForward.y - vDir.y * vForward.x };
		float fSide = sqrtf(vSide.x * vSide.x + vSide.y * vSide.y + vSide.z * vSide.z);
		if (fSide < 0.001f)
			vSide = vRight;
		else
			vSide = TMVector3{ vSide.x / fSide, vSide.y / fSide, vSide.z / fSide };

		float fHead = fTravel * fDistance;
		float fLength = kRocketLength < fHead ? kRocketLength : fHead;
		TMVector3 vHead = Offset(m_vecPosition, vDir, fHead);
		TMVector3 vTail = Offset(vHead, vDir, -fLength);

		const TMVector3 vCorner[4] =
		{
			Offset(vTail, vSide, -kRocketWidth * 0.15f), Offset(vTail, vSide, kRocketWidth * 0.15f),
			Offset(vHead, vSide, kRocketWidth), Offset(vHead, vSide, -kRocketWidth)
		};
		PushQuad(vertices, nCount, vCorner, 0xFFFFEEB0);
	}
	else
	{
		for (int i = 0; i < kSparks; ++i)
		{
			const Particle& spark = m_Particle[i];
			if (spark.fMaturity >= spark.fLifetime)
				continue;

			float fLife = spark.fMaturity / spark.fLifetime;
			float fFade = fLife < 0.55f ? 1.0f : (1.0f - fLife) / 0.45f;
			float fWhite = spark.fMaturity < kWhiteTime ? 1.0f - spark.fMaturity / kWhiteTime : 0.0f;
			PushSprite(vertices, nCount, spark.vPosition, vRight, vUp, spark.fSize, MixColor(m_dwColor, fWhite, fFade));
		}

		float fAge = m_fElapsed - kRocketTime;
		if (fAge < kFlashTime)
			PushSprite(vertices, nCount, m_vecEnd, vRight, vUp, 0.35f + fAge / kFlashTime * 0.3f, MixColor(m_dwColor, 0.7f, 1.0f - fAge / kFlashTime));
	}

	if (!nCount)
		return 1;

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	g_pDevice->m_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

	// Same additive setup as TMEffectBillBoard with EF_BRIGHT.
	g_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, 1u);
	g_pDevice->SetRenderState(D3DRS_DESTBLEND, 2u);
	g_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, 2u);
	g_pDevice->SetRenderState(D3DRS_FOGENABLE, 0);
	g_pDevice->SetRenderState(D3DRS_LIGHTING, 0);
	g_pDevice->SetRenderState(D3DRS_SRCBLEND, 5u);
	g_pDevice->SetRenderState(D3DRS_ALPHAFUNC, 8u);
	g_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, 0);
	g_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, 0);
	g_pDevice->SetRenderState(D3DRS_CULLMODE, 1u);

	g_pDevice->m_pd3dDevice->SetFVF(322u);
	g_pDevice->SetTexture(0, g_pTextureManager->GetEffectTexture(kSparkTexture, 5000u));
	g_pDevice->m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, nCount / 3, vertices, 24u);

	g_pDevice->SetRenderState(D3DRS_CULLMODE, 3u);
	g_pDevice->SetRenderState(D3DRS_LIGHTING, 1u);
	g_pDevice->SetRenderState(D3DRS_SRCBLEND, 2u);
	g_pDevice->SetRenderState(D3DRS_ALPHAFUNC, 7u);
	g_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, 1u);
	g_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, 1u);
	g_pDevice->SetRenderState(D3DRS_DESTBLEND, 6u);

	return 1;
}

Particle* TMEffectFireWork::SetParticle(Particle* result, unsigned short wType, float fLifeTime, TMVector3 vBasePosition, TMVector3 vBaseVelocity)
{
	return nullptr;
}

void TMEffectFireWork::SetCustomFireWork(char* pBuffer)
{
}
