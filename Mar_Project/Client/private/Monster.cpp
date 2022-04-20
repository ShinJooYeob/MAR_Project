#include "stdafx.h"
#include "..\public\Monster.h"
#include "Terrain.h"
#include "Player.h"

_uint CALLBACK Monster_Add_Force_Thread(void* _Prameter)
{
	THREADARG tThreadArg{};
	memcpy(&tThreadArg, _Prameter, sizeof(THREADARG));
	delete _Prameter;


	CMonster* pMonster = (CMonster*)(tThreadArg.pArg);
	FAILED_CHECK(pMonster->Calculate_Force(tThreadArg.IsClientQuit, tThreadArg.CriSec));
	return 0;
}


CMonster::CMonster(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice,pDeviceContext)
{
}

CMonster::CMonster(const CMonster & rhs)
	:CGameObject(rhs)
{
}

HRESULT CMonster::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));
	return S_OK;
}

HRESULT CMonster::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	m_pPlayer = (CPlayer*)(g_pGameInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Player)));
	NULL_CHECK_RETURN(m_pPlayer, E_FAIL);

	Safe_AddRef(m_pPlayer);

	m_pPlayerTransfrom = (CTransform*)(m_pPlayer->Get_Component(TAG_COM(Com_Transform)));

	m_pInstance = g_pGameInstance;

	return S_OK;
}

_int CMonster::Update(_double fDeltaTime)
{
	return _int();
}

_int CMonster::LateUpdate(_double fDeltaTime)
{
	return _int();
}

_int CMonster::Render()
{
	return _int();
}

_int CMonster::LateRender()
{
	return _int();
}

void CMonster::Add_Force(CTransform* pTransfrom, _float3 vDir, _float Power)
{

	m_vAddedForce = (vDir.Get_Nomalize() * Power) + m_vAddedForce.XMVector();


	if (!m_bIsAddForceActived)
	{
		pAFTransfrom = pTransfrom;
		m_bIsAddForceActived = true;
		g_pGameInstance->PlayThread(Monster_Add_Force_Thread, this);
	}

	
}

HRESULT CMonster::Calculate_Force(_bool * _IsClientQuit, CRITICAL_SECTION * _CriSec)
{

	DWORD  NowTick = GetTickCount();
	DWORD  OldTick = NowTick;

	while (true)
	{
		if (*_IsClientQuit == true)
			return S_OK;

		NowTick = GetTickCount();

		if ((NowTick - OldTick) < g_fDeltaTime * 1000)
			continue;
		OldTick = NowTick;

		_float Power = m_vAddedForce.Get_Lenth();
		_Vector vDir = m_vAddedForce.Get_Nomalize();


		if (Power * 0.78f < 0.2)
		{
			EnterCriticalSection(_CriSec);
			m_vAddedForce = _float3(0, 0, 0);
			LeaveCriticalSection(_CriSec);
			break;
		}

		EnterCriticalSection(_CriSec);
		pAFTransfrom->MovetoDir_bySpeed(vDir, Power, g_fDeltaTime);
		m_vAddedForce = vDir * Power * 0.78f;
		LeaveCriticalSection(_CriSec);


	}

	EnterCriticalSection(_CriSec);
	m_bIsAddForceActived = false;
	LeaveCriticalSection(_CriSec);


	return S_OK;
}

_int CMonster::Update_Pattern(_double fDeltaTime)
{
	return _int();
}

_bool CMonster::Check_Movable_Terrain(CTransform * pTransform, _fVector TargetDir, _float fMovalbeHeight)
{

	CGameInstance* pInstance = GetSingle(CGameInstance);

	CTerrain* pTerrain = (CTerrain*)(pInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Terrain)));

	_bool bIsMovable = false;

	_Vector MonsterPos = pTransform->Get_MatrixState(CTransform::STATE_POS);
	_Vector TargetPos = MonsterPos + XMVector3Normalize(TargetDir);

	bIsMovable = pTerrain->Check_Movable_Terrain(&bIsMovable, pTransform->Get_MatrixState(CTransform::STATE_POS), TargetPos, fMovalbeHeight);


	return bIsMovable;
}



HRESULT CMonster::Set_Monster_On_Terrain(CTransform* pTransform, _double fDeltaTime)
{
	CGameInstance* pInstance = GetSingle(CGameInstance);

	m_LevitationTime += fDeltaTime;
	_float fGravity = _float((m_LevitationTime) * (m_LevitationTime)* -29.4f);
	
	pTransform->MovetoDir_bySpeed(XMVectorSet(0, 1.f, 0, 0), fGravity, fDeltaTime);


	CTerrain* pTerrain = (CTerrain*)(pInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Terrain)));

	_bool bIsOn = false;

	_float3 CaculatedPos = pTerrain->PutOnTerrain(&bIsOn, pTransform->Get_MatrixState(CTransform::STATE_POS), m_vOldPos.XMVector());

	if (bIsOn)
	{
		m_LevitationTime = 0;
		pTransform->Set_MatrixState(CTransform::STATE_POS, CaculatedPos);
	}

	return S_OK;
}

HRESULT CMonster::Update_WanderAround(CTransform * pTransform,_double fDeltaTime, _float TurnMixingRate)
{
	iWanderCount = 0;
	while (!(Check_Movable_Terrain(pTransform, m_vLookDir.XMVector(), 0.1f)))
	{
		_float RandFloat = GetSingle(CUtilityMgr)->RandomFloat(-1, 1);
		_Vector NewLook = pTransform->Get_MatrixState(CTransform::STATE_LOOK) * -1;
		if (RandFloat < 0)
		{
			_Vector Left = pTransform->Get_MatrixState(CTransform::STATE_RIGHT)* -1;

			NewLook = XMVector3Normalize(pTransform->Get_MatrixState(CTransform::STATE_LOOK)* (1 + RandFloat) + (Left * -RandFloat));
		}
		else
		{
			_Vector vRight = pTransform->Get_MatrixState(CTransform::STATE_RIGHT);

			NewLook = XMVector3Normalize(pTransform->Get_MatrixState(CTransform::STATE_LOOK)* (1 - RandFloat) + (vRight  * RandFloat));
		}

		m_vLookDir = NewLook;
		if (iWanderCount > 5) break;
		iWanderCount++;
	}

	pTransform->LookDir(m_vLookDir.XMVector()*(TurnMixingRate) + pTransform->Get_MatrixState(CTransform::STATE_LOOK) * (1- TurnMixingRate));
	pTransform->MovetoDir(m_vLookDir.XMVector(),fDeltaTime);



	return S_OK;
}

_float CMonster::Distance_BetweenPlayer(CTransform * pTransform)
{
	return	XMVectorGetX(XMVector3Length(pTransform->Get_MatrixState(CTransform::STATE_POS) - m_pPlayerTransfrom->Get_MatrixState(CTransform::STATE_POS)));
}

void CMonster::Free()
{
	__super::Free();
	Safe_Release(m_pPlayer);

}
