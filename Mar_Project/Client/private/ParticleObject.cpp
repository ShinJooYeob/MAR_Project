#include "stdafx.h"
#include "..\public\ParticleObject.h"



CParticleObject::CParticleObject(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CParticleObject::CParticleObject(const CParticleObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT CParticleObject::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));

	m_ParticleList.clear();

	return S_OK;
}

HRESULT CParticleObject::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));


	FAILED_CHECK(SetUp_ParticleDesc(pArg));

	FAILED_CHECK(SetUp_Components());



	FAILED_CHECK(Initialize_Child_Clone());

	return S_OK;
}


_int CParticleObject::Update(_double fTimeDelta)
{
	if (0 > __super::Update(fTimeDelta)) return -1;

	m_PassedLifeTime += fTimeDelta;


	std::list<PARTICLEATT>::iterator iter;

	if (m_PassedLifeTime < m_ParticleDesc.TotalParticleTime)
	{
		for (iter = m_ParticleList.begin(); iter != m_ParticleList.end(); iter++)
		{

			m_boundingBox.ResetBoudingBox(iter->_NowparantPos);
			iter->_age += fTimeDelta;

			Update_Position_by_Velocity(&(*iter), fTimeDelta);

			if (m_ParticleDesc.ParticleColorChage)
			{
				Update_ColorChange(&(*iter), fTimeDelta);
			}

			if (m_ParticleDesc.bSubPraticle)
			{
				iter->SubParticle->Reset_FixedPostion((iter->_position));
			}
			// 범위 체크
			if (m_boundingBox.isPointInside(iter->_position) == false || iter->_age > iter->_lifeTime)
			{
				// 리셋
				ResetParticle(&(*iter));
			}
		}

	}
	else
		
	{

	}


	return _int();
}

_int CParticleObject::LateUpdate(_double fTimeDelta)
{
	return _int();
}

_int CParticleObject::Render()
{
	return _int();
}

_int CParticleObject::LateRender()
{
	return _int();
}

void CParticleObject::ResetParticle(PARTICLEATT * attribute)
{
}

void CParticleObject::Reset_Velocity(_float3 & fAttVlocity)
{
}

void CParticleObject::Update_Position_by_Velocity(PARTICLEATT * tParticleAtt, _float fTimeDelta)
{
}

void CParticleObject::Update_SizeChange(PARTICLEATT * tParticleAtt, _float fTimeDelta)
{
}

void CParticleObject::Update_ColorChange(PARTICLEATT * tParticleAtt, _float fTimeDelta)
{
}

HRESULT CParticleObject::SetUp_Components()
{
	return E_NOTIMPL;
}

HRESULT CParticleObject::SetUp_ParticleDesc(void * pArg)
{
	return E_NOTIMPL;
}

HRESULT CParticleObject::SetUp_ConstTable()
{
	return E_NOTIMPL;
}

HRESULT CParticleObject::SetUp_OnShader()
{
	return E_NOTIMPL;
}

CGameObject * CParticleObject::Clone(void * pArg)
{
	return nullptr;
}

void CParticleObject::Free()
{
}
