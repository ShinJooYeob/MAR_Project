#include "stdafx.h"
#include "..\public\Bullet.h"
#include "Player.h"


CBullet::CBullet(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CWeapon(pDevice,pDeviceContext)
{
}

CBullet::CBullet(const CBullet & rhs)
	: CWeapon(rhs)
{
}

HRESULT CBullet::Initialize_Prototype(void * pArg)
{
	__super::Initialize_Prototype(pArg);

	return S_OK;
}

HRESULT CBullet::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone_Bullet(pArg));



	FAILED_CHECK(SetUp_Components());

	if (pArg != nullptr)
		memcpy(&m_vTargetDir, pArg, sizeof(_float3));

	_float3 t = m_pPlayer->Get_FirePos();
	_float3 tt = ((CTransform*)(m_pPlayer->Get_Component(TAG_COM(Com_Transform))))->Get_MatrixState(CTransform::STATE_POS);

	m_pTransformCom->Set_MatrixState(CTransform::STATE_POS,	m_pPlayer->Get_FirePos());

	return S_OK;
}

_int CBullet::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;

	m_pTransformCom->MovetoDir(m_vTargetDir.XMVector(), fDeltaTime);

	m_fLifeTime += _float(fDeltaTime);

	if (m_fLifeTime > m_fTotalLifeTime)
		Set_IsDead();

	return _int();
}

_int CBullet::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;



	return _int();
}

_int CBullet::Render()
{
	return _int();
}

_int CBullet::LateRender()
{
	return _int();
}

HRESULT CBullet::SetUp_Components()
{

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VNAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));


	CTransform::TRANSFORMDESC tDesc = {};

	tDesc.fMovePerSec = 50;
	tDesc.fRotationPerSec = XMConvertToRadians(60);
	tDesc.fScalingPerSec = 1;
	tDesc.vPivot = _float3(0, 0, 0);

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom, &tDesc));


	return S_OK;
}

void CBullet::Free()
{
	__super::Free();


	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModel);
	Safe_Release(m_pColliderCom);
}
