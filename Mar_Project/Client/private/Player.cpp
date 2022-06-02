#include "stdafx.h"
#include "..\public\Player.h"
#include "Weapon.h"
#include "Camera_Main.h"
#include "Terrain.h"
#include "GamePlayUI.h"
#include "ClockBomb.h"

#include "DustWind.h"
#include "Knife.h"
#include "Horse.h"
#include "CircleTornado.h"
#include "Umbrella.h"

#include "TransparencyBall.h"


_uint CALLBACK Add_Force_Thread(void* _Prameter)
{
	THREADARG tThreadArg{};
	memcpy(&tThreadArg, _Prameter, sizeof(THREADARG));
	delete _Prameter;

	CPlayer* pPlayer = (CPlayer*)(tThreadArg.pArg);
	FAILED_CHECK(pPlayer->Calculate_Force(tThreadArg.IsClientQuit, tThreadArg.CriSec));
	return 0;
}


CPlayer::CPlayer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext) 
	: CGameObject(pDevice, pDeviceContext)
{
}

CPlayer::CPlayer(const CPlayer & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPlayer::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));


	return S_OK;
}

HRESULT CPlayer::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	FAILED_CHECK(SetUp_Components());

	FAILED_CHECK(Ready_ParticleDesc());

	if (pArg != nullptr)
		m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, *((_float3*)pArg));


	m_szNameTag = L"Alice";



	return S_OK;
}

_int CPlayer::Update(_double fDeltaTime)
{
	if (m_eNowSceneNum == SCENE_LOADING)
		return 0;

	if (__super::Update(fDeltaTime) < 0)return -1;
	m_pColliderCom->Update_ConflictPassedTime(fDeltaTime);


	if (m_bEattingProteinStart)
	{
		FAILED_CHECK(Update_EattingProtein(fDeltaTime));
	}
	else
	{
		FAILED_CHECK(Update_SpwanNDeathAnim(fDeltaTime));
		FAILED_CHECK(Manage_CoolTime(fDeltaTime));
		FAILED_CHECK(Input_Keyboard(fDeltaTime));
	}




	//if (g_pGameInstance->Get_DIKeyState(DIK_1)&DIS_Down)
	//{
	//
	//	Eat_Protain();
	//}
	//if (g_pGameInstance->Get_DIKeyState(DIK_1)&DIS_Down)
	//{
	//
	//	Eat_Protain();
	//}









	if (m_iWeaponModelIndex != 10)
	{
		m_vecWeapon[m_iWeaponModelIndex]->Update(fDeltaTime);
		m_vBulletFirePos = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS) + (m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK) * 0.6f)
			+ (m_pTransformCom->Get_MatrixState(CTransform::STATE_UP)) + (m_pTransformCom->Get_MatrixState(CTransform::STATE_RIGHT)*0.1f);
	}



	_uint iNowAnimIndex  = m_pModel->Get_NowAnimIndex();
	if (iNowAnimIndex ==  15|| iNowAnimIndex == 20|| iNowAnimIndex == 14 || iNowAnimIndex == 10 || iNowAnimIndex  == 37 || iNowAnimIndex == Weapon_Giant + 4 ||
		(iNowAnimIndex >= Weapon_Knife + 8 && iNowAnimIndex <= Weapon_Knife + 16) ||
		(iNowAnimIndex >= Weapon_Horse + 8 && iNowAnimIndex <= Weapon_Horse + 14) )
	{
		FAILED_CHECK(m_pModel->Update_AnimationClip(fDeltaTime * 1.5));
	}
	else
	{
		FAILED_CHECK(m_pModel->Update_AnimationClip(fDeltaTime ));

	}

	m_pColliderCom->Update_Transform(0, m_pTransformCom->Get_WorldMatrix());
	for (_uint i = 1; i < m_pColliderCom->Get_NumColliderBuffer(); i++)
	{
		_Matrix			TransformMatrix = XMLoadFloat4x4(m_tCollisionAttachPtr[i - 1].pUpdatedNodeMat) * XMLoadFloat4x4(m_tCollisionAttachPtr[i - 1].pDefaultPivotMat);
		TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
		TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
		TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);
		m_pColliderCom->Update_Transform(i, TransformMatrix * m_pTransformCom->Get_WorldMatrix());
	}

	if (!m_bIsAttached && !m_bAliceSpwanAnimStart && !m_bAliceDeathAnimStart)
	{
		g_pGameInstance->Add_CollisionGroup(CollisionType_Player, this, m_pColliderCom);
#ifdef _DEBUG
		FAILED_CHECK(m_pRendererCom->Add_DebugGroup(m_pColliderCom));
#endif // _DEBUG
	}

	return _int();
}

_int CPlayer::LateUpdate(_double fDeltaTime)
{
	if (m_eNowSceneNum == SCENE_LOADING)
		return 0;

	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;

	if (g_pGameInstance->Get_DIKeyState(DIK_L)&DIS_Down)
		m_bSlide = !m_bSlide;

	if (!m_bSlide)
	{
		if (m_bIsGiant)
		{
			FAILED_CHECK(Set_Player_On_Terrain_IgnoreTile());
		}
		else if (m_eNowSceneNum == SCENE_STAGE2)
		{
			FAILED_CHECK(Set_Player_On_Terrain_DontPutonJumpMovable());
		}
		else
		{
			FAILED_CHECK(Set_Player_On_Terrain());
		}
	}
	else 
	{
		FAILED_CHECK(Set_Player_On_Slieder(fDeltaTime));
	}
	
	FAILED_CHECK(Set_Camera_On_Player(fDeltaTime));





	if (!m_fDashPassedTime && !m_bTrappedbyFlower && !m_bPlayerNotRender)
	{
		FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));

		if (m_iWeaponModelIndex != 10 && !m_LevitationTime)
		{
			m_vecWeapon[m_iWeaponModelIndex]->LateUpdate(fDeltaTime);
		}


	}

	if (m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS).y < 1)
	{
		m_iJumpCount = 0;
		m_LevitationTime = 0;
		m_fJumpPower = -1.f;
		Set_PlayerDeadAnimStart();
		return 0;
	}

	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
	return _int();
}

_int CPlayer::Render()
{
	if (__super::Render() < 0)
		return -1;


	NULL_CHECK_RETURN(m_pModel, E_FAIL);





	FAILED_CHECK(SetUp_ConstTable());



	FAILED_CHECK(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
	
	_uint NumMaterial = m_pModel->Get_NumMaterial();

	for (_uint i = 0; i < NumMaterial; i++)
	{
		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
			FAILED_CHECK(m_pModel->Bind_OnShader(m_pShaderCom, i, j, MODLETEXTYPE(j)));

		if (i <= 1 )
		{
			FAILED_CHECK(m_pModel->Render(m_pShaderCom, 3, i, "g_BoneMatrices"));
		}
		else
		{
			FAILED_CHECK(m_pModel->Render(m_pShaderCom, 2, i, "g_BoneMatrices"));
		}

	}



	
	return _int();
}

_int CPlayer::LightRender()
{
	if (__super::LightRender() < 0)
		return -1;

	NULL_CHECK_RETURN(m_pModel, E_FAIL);

	FAILED_CHECK(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));

	_uint NumMaterial = m_pModel->Get_NumMaterial();

	for (_uint i = 0; i < NumMaterial; i++)
	{
		FAILED_CHECK(m_pModel->Render(m_pShaderCom, 10, i, "g_BoneMatrices"));

	}


	return _int();
}

void CPlayer::Add_Dmg_to_Player(_uint iDmgAmount)
{
	if (!iDmgAmount || m_bAliceSpwanAnimStart || m_bAliceDeathAnimStart ) return;

	if (m_iHP <= iDmgAmount)
	{
		Set_PlayerDeadAnimStart();
		return;
	}
	m_iHP -= iDmgAmount;


	((CGamePlayUI*)(g_pGameInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_UI_GamePlay))))->Add_Dmg_to_Player(m_iHP,iDmgAmount);
	GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_HitEffect, 0.2, { 1,0,0,0.4f });

	ZeroMemory(m_bAtkMoveMentChecker, sizeof(_bool) * 3);
	m_bIsAttackClicked = false;
	m_iAttackCount = 0;

	if (m_bIsAttached || m_bSlide )return;

	switch (m_eNowWeapon)
	{
	case Client::CPlayer::Weapon_None:
	{
		switch (iDmgAmount)
		{
		case 1:
		case 2:
			m_pModel->Change_AnimIndex_ReturnTo_Must(rand() % 2 + 38, 0, 0.1, true);
			break;

		case 3:
			m_pModel->Change_AnimIndex_ReturnTo_Must(40, 0, 0.1, true);
			Add_Force(m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK)*-1, 10);
			break;


		case 4:
			m_pModel->Change_AnimIndex_ReturnTo_Must(41, 0, 0.1, true);
			Add_Force(m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK)*-1, 15);
			break;


		default:
			m_pModel->Change_AnimIndex_ReturnTo_Must(rand() % 2 + 42, 0, 0.1, true);
			break;
		}
	}
		break;



	case Client::CPlayer::Weapon_Knife:
	{

		switch (iDmgAmount)
		{
		case 1:
		case 2:
			m_pModel->Change_AnimIndex_ReturnTo_Must(Weapon_Knife + rand() % 2 + 18, Weapon_Knife + 0, 0.1, true);
			Add_Force(m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK)*-1, 10);
			break;

		case 3:
		case 4:
			m_pModel->Change_AnimIndex_ReturnTo_Must(Weapon_Knife + 20, Weapon_Knife + 0, 0.1, true);
			Add_Force(m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK)*-1, 15);
			break;


		default:
			m_pModel->Change_AnimIndex_ReturnTo_Must(Weapon_Knife + 21 +rand() % 2, Weapon_Knife + 0, 0.1, true);
			break;
		}
	}
		break;
	case Client::CPlayer::Weapon_Grinder:

	{

		switch (iDmgAmount)
		{
		case 1:
		case 2:
			if (m_bIsZoom)
				m_pModel->Change_AnimIndex_ReturnTo_Must(Weapon_Grinder + rand() % 2 + 11, Weapon_Grinder + 3, 0.1, true);
			else
				m_pModel->Change_AnimIndex_ReturnTo_Must(Weapon_Grinder + rand() % 2 + 11, Weapon_Grinder + 0, 0.1, true);

			break;


		default:
			if (m_bIsZoom)
				m_pModel->Change_AnimIndex_ReturnTo_Must(Weapon_Grinder + 13, Weapon_Grinder + 3, 0.1, true);
			else
				m_pModel->Change_AnimIndex_ReturnTo_Must(Weapon_Grinder + 13, Weapon_Grinder + 0, 0.1, true);

			Add_Force(m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK)*-1, 15);

			break;
		}
	}


		break;
	case Client::CPlayer::Weapon_Horse:

	{

		switch (iDmgAmount)
		{
		case 1:
			m_pModel->Change_AnimIndex_ReturnTo_Must(Weapon_Horse + 16, Weapon_Horse + 0, 0.1, true);
		case 2:
		case 3:
			m_pModel->Change_AnimIndex_ReturnTo_Must(Weapon_Horse + rand() % 2 + 17, Weapon_Horse + 0, 0.1, true);
			break;

		case 4:
			m_pModel->Change_AnimIndex_ReturnTo_Must(Weapon_Horse + 19, Weapon_Horse + 0, 0.1, true);
			Add_Force(m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK)*-1, 15);
			break;

		default:
			m_pModel->Change_AnimIndex_ReturnTo_Must(Weapon_Horse + 20 + rand() % 2, Weapon_Horse + 0, 0.1, true);
			break;
		}

	}
		break;
	case Client::CPlayer::Weapon_Teapot:

	{

		switch (iDmgAmount)
		{
		case 1:
		case 2:
			if (m_bIsZoom)
			{
				if (m_fCharedGauge > 0.4)
					m_pModel->Change_AnimIndex_ReturnTo_Must(Weapon_Teapot + rand() % 2 + 17, Weapon_Teapot + 3, 0.1, true);
				else
					m_pModel->Change_AnimIndex_ReturnTo_Must(Weapon_Teapot + rand() % 2 + 17, Weapon_Teapot + 10, 0.1, true);

			}
			else
				m_pModel->Change_AnimIndex_ReturnTo_Must(Weapon_Teapot + rand() % 2 + 17, Weapon_Teapot + 0, 0.1, true);

			break;


		default:
			if (m_bIsZoom)
				m_pModel->Change_AnimIndex_ReturnTo_Must(Weapon_Teapot + 19, Weapon_Teapot + 3, 0.1, true);
			else
				m_pModel->Change_AnimIndex_ReturnTo_Must(Weapon_Teapot + 19, Weapon_Teapot + 0, 0.1, true);

			Add_Force(m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK)*-1, 15);

			break;
		}
	}



		break;
	case Client::CPlayer::Weapon_Umbrella:
		break;


	default:
		break;
	}



}

void CPlayer::Heal_to_Player(_uint iDmgAmount)
{
	if (!iDmgAmount) return;

	m_iHP += iDmgAmount;

	if (m_iHP >= PlayerMaxHP) m_iHP = PlayerMaxHP;


	((CGamePlayUI*)(g_pGameInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_UI_GamePlay))))->Add_Dmg_to_Player(m_iHP, 0);
}

void CPlayer::Change_Weapon(_uint WeaponIndex)
{


	switch (WeaponIndex)
	{
	case 0:

		m_iWeaponModelIndex = 0;
		m_eNowWeapon = CPlayer::Weapon_Knife;
		m_pModel->Change_AnimIndex(Weapon_Knife + 0);
		ZeroMemory(m_bAtkMoveMentChecker, sizeof(_bool) * 3);
		m_pModel->Set_BlockAnim(false);

		m_bIsAttackClicked = false;
		m_iAttackCount = 0;
		m_bIsZoom = false;
		m_bIsCharged = false;
		m_fCharedGauge = 0;
		m_bIsCoolTime = false;
		m_fUmbrellaIntro = 0;

		break;
	case 1:
		m_iWeaponModelIndex = 1;

		m_eNowWeapon = CPlayer::Weapon_Grinder;
		m_pModel->Change_AnimIndex(Weapon_Grinder + 0);
		ZeroMemory(m_bAtkMoveMentChecker, sizeof(_bool) * 3);
		m_pModel->Set_BlockAnim(false);
		m_bIsAttackClicked = false;
		m_iAttackCount = 0;
		m_bIsZoom = false;
		m_bIsCharged = false;
		m_fCharedGauge = 0;
		m_bIsCoolTime = false;
		m_fUmbrellaIntro = 0;
		break;
	case 2:
		m_iWeaponModelIndex = 2;

		m_eNowWeapon = CPlayer::Weapon_Horse;
		m_pModel->Change_AnimIndex(Weapon_Horse + 0);

		ZeroMemory(m_bAtkMoveMentChecker, sizeof(_bool) * 3);
		m_bIsAttackClicked = false;
		m_iAttackCount = 0;
		m_bIsZoom = false;
		m_bIsCharged = false;
		m_fCharedGauge = 0;
		m_bIsCoolTime = false;
		m_fUmbrellaIntro = 0;
		m_pModel->Set_BlockAnim(false);

		break;
	case 3:

		m_iWeaponModelIndex = 3;
		m_eNowWeapon = CPlayer::Weapon_Teapot;

		m_pModel->Change_AnimIndex(Weapon_Teapot + 0);
		ZeroMemory(m_bAtkMoveMentChecker, sizeof(_bool) * 3);
		m_bIsAttackClicked = false;
		m_iAttackCount = 0;
		m_bIsZoom = false;
		m_bIsCharged = false;
		m_fCharedGauge = 0;
		m_bIsCoolTime = false;
		m_fUmbrellaIntro = 0;
		m_pModel->Set_BlockAnim(false);

		break;
	case 4:
		m_iWeaponModelIndex = 4;
		m_eNowWeapon = CPlayer::Weapon_Umbrella;
		ZeroMemory(m_bAtkMoveMentChecker, sizeof(_bool) * 3);
		m_bIsAttackClicked = false;
		m_iAttackCount = 0;
		m_bIsZoom = false;
		m_bIsCharged = false;
		m_fCharedGauge = 0;
		m_bIsCoolTime = false;
		m_bUmbrellaReflected = false;
		m_fUmbrellaIntro = 0;
		m_vecWeapon[m_iWeaponModelIndex]->Set_AttackAble(true);
		m_pModel->Change_AnimIndex_UntilTo(Weapon_Umbrella, Weapon_Umbrella + 1);
		m_pModel->Set_BlockAnim(true);
		break;
	case 5:
		m_iWeaponModelIndex = 10;
		m_eNowWeapon = CPlayer::Weapon_None;
		ZeroMemory(m_bAtkMoveMentChecker, sizeof(_bool) * 3);
		m_bIsAttackClicked = false;
		m_iAttackCount = 0;
		m_bIsZoom = false;
		m_bIsCharged = false;
		m_fCharedGauge = 0;
		m_bIsCoolTime = false;
		m_fUmbrellaIntro = 0;
		m_pModel->Change_AnimIndex(0);
		m_pModel->Set_BlockAnim(false);
		break;

	default:
		break;
	}





}

void CPlayer::Add_JumpForce(_float JumpPower, _uint IsKindsOfJump)
{
	m_fJumpPower = m_fMaxJumpPower = JumpPower;
	m_LevitationTime = 0.0000000001f;

	if (IsKindsOfJump)
	{
		ZeroMemory(m_bAtkMoveMentChecker, sizeof(_bool) * 3);
		m_bIsAttackClicked = false;
		m_iAttackCount = 0;
		m_bIsZoom = false;
		m_bIsCharged = false;
		m_fCharedGauge = 0;
		m_bIsCoolTime = false;
		m_fUmbrellaIntro = 0;
		m_pModel->Set_BlockAnim(false);

		m_pModel->Change_AnimIndex(29);

	}
}

void CPlayer::Add_Force(_float3 vDir, _float Power)
{
	m_vAddedForce = (vDir.Get_Nomalize() * Power) + m_vAddedForce.XMVector();

	if (!m_bIsActived)
	{
		m_bIsActived = true;
		g_pGameInstance->PlayThread(Add_Force_Thread, this);
	}

}

HRESULT CPlayer::Calculate_Force(_bool * _IsClientQuit, CRITICAL_SECTION * _CriSec)
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
		m_pTransformCom->MovetoDir_bySpeed(vDir, Power , g_fDeltaTime);
		m_vAddedForce = vDir * Power * 0.78f;
		LeaveCriticalSection(_CriSec);


	}

	EnterCriticalSection(_CriSec);
	m_bIsActived = false;
	LeaveCriticalSection(_CriSec);


	return S_OK;
}

_bool CPlayer::Set_IsAttached(_bool bBool)
{
	m_bIsAttached = bBool;
	if (m_bIsAttached)
	{
		m_pModel->Change_AnimIndex(33, 0.15, true);
	}
	else
	{
		Add_Dmg_to_Player(6);
	}

	return _bool();
}

void CPlayer::Set_NotLevitation()
{
	m_iJumpCount = 0;
	m_LevitationTime = 0;
	m_fJumpPower = -1.f;
}

void CPlayer::Let_PlayerSliding(_bool bBool)
{
	m_bSlide = bBool;


	if (m_bSlide)
	{
		m_pModel->Change_AnimIndex_UntilNReturn(10, 11,11, 0.15);
	}
	else
	{
		m_pModel->Change_AnimIndex_ReturnTo(14, 0, 0.15);



	}
}

void CPlayer::Set_TrappedFlower(_bool bBool)
{
	m_bTrappedbyFlower = bBool;
	m_fSmallPassedTime = 0;
}

void CPlayer::Set_PlayerPosition(_float3 vPos)
{
	{ m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, vPos); };

	m_vOldPos = vPos;

}

void CPlayer::Set_IsVenting(_bool bBool, _float3 vTargetLook)
{
	m_bIsVenting = bBool;
	m_vVentingTargetLook = vTargetLook;
}

void CPlayer::Set_PlayerDeadAnimStart()
{
	if (m_bAliceDeathAnimStart)return;

	m_iHP = 0;
	((CGamePlayUI*)(g_pGameInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_UI_GamePlay))))->Add_Dmg_to_Player(m_iHP, 0);
	GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_HitEffect, 0.2, { 1,0,0,0.4f }

	);

	m_pModel->Change_AnimIndex_ReturnTo_Must(37, 37, 0.15f, true);
	m_bAliceDeathAnimStart = true;
	m_DeathAnimPassedTime = 0;
}


void CPlayer::Set_UmbrellaReflected(_bool bBool, _float3 vPosition, _float3 vLookDir)
{
	m_bUmbrellaReflected = bBool;
	m_ReflectPassedTime = 0;

	GetSingle(CUtilityMgr)->Start_InstanceParticle(m_eNowSceneNum, vPosition, 3);
	GetSingle(CUtilityMgr)->Start_InstanceParticle(m_eNowSceneNum, vPosition, 2);


	CCircleTornado::CIRCLETORNADODESC tDesc;
	tDesc.vLook = vLookDir;
	tDesc.vPosition = vPosition;
	g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerCircleTornado), &tDesc);


	tDesc.vLook = m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK);
	tDesc.vPosition = ((CUmbrella*)m_vecWeapon[m_iWeaponModelIndex])->Get_ColliderPosition().XMVector() + tDesc.vLook.XMVector() * 0.5f;
	tDesc.fSize = 1.5f;
	g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerCircleTornado), &tDesc);


	GetSingle(CUtilityMgr)->Start_InstanceParticle(m_eNowSceneNum, tDesc.vPosition, 4);
	GetSingle(CUtilityMgr)->Start_InstanceParticle(m_eNowSceneNum, tDesc.vPosition, 0);

	m_vecWeapon[m_iWeaponModelIndex]->Set_AttackAble(false);
	m_pModel->Change_AnimIndex_ReturnTo_Must(Weapon_Umbrella + 4, Weapon_Umbrella + 1, 0, true);
	m_pModel->Add_Time_To_NowAnimPlayAcc(0.15);
}

HRESULT CPlayer::Update_EattingProtein(_double fDeltatime)
{

	if (!m_bEattingProteinStart) return S_FALSE;

	m_ProteinPassedTime += fDeltatime;

	if (!m_iProteinChecker)
	{
		GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_FadeOutIn, 1, _float4(0, 0, 0, 1));
		m_iProteinChecker++;
	}
	else if(m_iProteinChecker == 1 && m_ProteinPassedTime > 0.4f)
	{
		//vector<CAMACTDESC>		 vecCamPositions;
		//vector<CAMACTDESC>		 vecLookPostions;
		//= vecLookPostions;
		//= vecCamPositions;

		//////////////////////////////////////////////////////////////////////////

		CAMERAACTION tDesc;
		CAMACTDESC Before;

		tDesc.vecCamPos;
		Before.fDuration = 0.1f;
		Before.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS)
			+ (XMVector3Normalize(m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK)) * 3.f) + XMVectorSet(0, 2, 0, 0);;
		tDesc.vecCamPos.push_back(Before);
		Before.fDuration = 6.5f;
		Before.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS)
			+ (XMVector3Normalize(m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK)) * 3.f) + XMVectorSet(0, 2, 0, 0);;
		tDesc.vecCamPos.push_back(Before);



		Before.fDuration = 0.1f;
		Before.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS) + XMVectorSet(0, 1.5f, 0, 0);
		tDesc.vecLookAt.push_back(Before);

		Before.fDuration = 6.5f;
		Before.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS) + XMVectorSet(0, 1.5f, 0, 0);
		tDesc.vecLookAt.push_back(Before);





		CAMACTDESC Return;

		Return.fDuration = 0.1f;
		Return.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS)
			+ (XMVector3Normalize(m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK)) * 10.f);
		tDesc.vecCamPos.push_back(Return);

		Return.fDuration = 1.4f;
		Return.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS)
			+ (XMVector3Normalize(m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK))* 20.f)
			+ XMVectorSet(0, 15, 0, 0);
		tDesc.vecCamPos.push_back(Return);




		Return.fDuration = 0.1f;
		Return.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS) + XMVectorSet(0, 2, 0, 0);
		tDesc.vecLookAt.push_back(Return);

		Return.fDuration = 2.5f;
		Return.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS) + XMVectorSet(0, 30, 0, 0);
		tDesc.vecLookAt.push_back(Return);


		m_pMainCamera->CamActionStart(tDesc);

		m_iProteinChecker++;
	}
	else if (m_iProteinChecker == 2 &&  m_ProteinPassedTime > 1.2f)
	{
		CWeapon::WEAPONDESC tWeaponDesc;
		tWeaponDesc.pModel = m_pModel;
		tWeaponDesc.pParantTransform = m_pTransformCom;
		tWeaponDesc.szHirarchyNodeName = "Bip01-R-Finger0";

		CWeapon* pWeapon = nullptr;
		g_pGameInstance->Add_GameObject_To_Layer(SCENE_STATIC, TAG_LAY(Layer_StaticMapObj), TAG_OP(Prototype_EatableProtein), &tWeaponDesc);
		m_iProteinChecker++;
	}
	else if (m_iProteinChecker == 3 && m_ProteinPassedTime > 6.5f)
	{
		GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_FadeOutIn, 1, _float4(0, 0, 0, 1));
		m_iProteinChecker++;

	}
	else if (m_iProteinChecker == 4 && m_ProteinPassedTime > 7.f)
	{

		Set_GettingBigger(true);

		m_pModel->Set_BlockAnim(false);
		m_bEattingProteinStart = false;
		m_iProteinChecker++;
	}




	return S_OK;
}

_bool CPlayer::Get_IsGiantAtteck()
{

	_uint iAnimIndex = m_pModel->Get_NowAnimIndex();
	if (iAnimIndex == Weapon_Giant + 2 )
	{
		_double PlayRate = m_pModel->Get_PlayRate();

		if (PlayRate > 0.28888888f &&PlayRate < 0.5555555)	return true;
		else return false;
		//0.28888888 0.5555555


	}
	else if (iAnimIndex == Weapon_Giant + 3)
	{
		_double PlayRate = m_pModel->Get_PlayRate();

		if (PlayRate > 0.3363636f &&PlayRate < 0.454545454)	return true;
		else return false;
	}
	else
	{
		return false;
	}
}


HRESULT CPlayer::SetUp_Components()
{

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Mesh_Player), TAG_COM(Com_Model), (CComponent**)&m_pModel));
	FAILED_CHECK(m_pModel->Change_AnimIndex(0));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Collider), TAG_COM(Com_Collider), (CComponent**)&m_pColliderCom));



	COLLIDERDESC			ColliderDesc;
	/* For.Com_SPHERE */
	ZeroMemory(&ColliderDesc, sizeof(COLLIDERDESC));
	ColliderDesc.vScale = _float3(2.0f, 2.f, 2.f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(0.f, 1.f, 0.f, 1.f);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));

	 //Pivot  : 0.000000f , 0.000000f , -0.620000f , 1
	 //size  : 0.960000f , 0.860000f , 0.550000f  
	 ZeroMemory(&ColliderDesc, sizeof(COLLIDERDESC));
	 ColliderDesc.vScale = _float3(0.480000f, 0.550000f,0.860000f);
	 ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	 ColliderDesc.vPosition = _float4(0.000000f, 0.000000f, -0.620000f, 1);
	 FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_OBB, &ColliderDesc));
	 m_pColliderCom->Set_ParantBuffer();

	 m_tCollisionAttachPtr[0] = m_pModel->Find_AttachMatrix_InHirarchyNode("Bip01-Pelvis");
	 NULL_CHECK_RETURN(m_tCollisionAttachPtr[0].pDefaultPivotMat, E_FAIL);

	//Pivot  : 0.000000f , 0.000000f , -1.379999f , 1
	//size  : 0.630000f , 0.740000f , 0.270001f  
	ZeroMemory(&ColliderDesc, sizeof(COLLIDERDESC));
	//(x,z,y)
	//ColliderDesc.vScale = _float3(0.630000f, 0.740000f, 0.270001f);
	ColliderDesc.vScale = _float3(0.315000f, 0.270001f,0.740000f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(0.000000f, 0.000000f, -1.379999f, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_OBB, &ColliderDesc));
	m_pColliderCom->Set_ParantBuffer();

	 m_tCollisionAttachPtr[1]= m_pModel->Find_AttachMatrix_InHirarchyNode("Bip01-Spine2");
	 NULL_CHECK_RETURN(m_tCollisionAttachPtr[1].pDefaultPivotMat, E_FAIL);








	CTransform::TRANSFORMDESC tDesc = {};

	tDesc.fMovePerSec = PlayerMoveSpeed;
	tDesc.fRotationPerSec = XMConvertToRadians(60);
	tDesc.fScalingPerSec = 1;
	tDesc.vPivot = _float3(0, 0, 0);
	m_fSmallScale = 1.f;

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom, &tDesc));

	m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, _float3(0, 5.f, 0));
	m_fJumpPower = m_fMaxJumpPower= PlayerMaxJumpPower;
	m_LevitationTime = g_fDeltaTime;

	m_pMainCamera =(CCamera_Main*)(g_pGameInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Camera_Main)));
	NULL_CHECK_RETURN(m_pMainCamera, E_FAIL);

	ZeroMemory(m_bAtkMoveMentChecker, sizeof(_bool) * 3);


	//FAILED_CHECK(SetUp_Weapon());
	return S_OK;
}

HRESULT CPlayer::SetUp_ConstTable()
{
	CGameInstance* pInstance = GetSingle(CGameInstance);
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_VIEW), sizeof(_float4x4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_PROJ), sizeof(_float4x4)));


	//FAILED_CHECK(m_pShaderCom->Set_RawValue("g_CamPosition", &pInstance->Get_TargetPostion_float4(PLV_CAMERA), sizeof(_float4)));
	//FAILED_CHECK(m_pShaderCom->Set_RawValue("g_CamLookDir", &pInstance->Get_TargetPostion_float4(PLV_CAMLOOK), sizeof(_float4)));


	//const LIGHTDESC* pLightDesc = pInstance->Get_LightDesc(LIGHTDESC::TYPE_DIRECTIONAL, 0);
	//NULL_CHECK_RETURN(pLightDesc, -1);

	//FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vLightVector", &(pLightDesc->vVector), sizeof(_float4)));
	//FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4)));
	//FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4)));
	//FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4)));


	return S_OK;
}

HRESULT CPlayer::SetUp_Weapon()
{
	m_vecWeapon.reserve(6);

	CGameInstance* pInstance = GetSingle(CGameInstance);

	NULL_CHECK_RETURN(m_pModel, E_FAIL);
	NULL_CHECK_RETURN(m_pTransformCom, E_FAIL);

	CWeapon::WEAPONDESC tWeaponDesc;
	tWeaponDesc.pModel = m_pModel;
	tWeaponDesc.pParantTransform = m_pTransformCom;
	tWeaponDesc.szHirarchyNodeName = "Bip01-Prop1";

	CWeapon* pWeapon = nullptr;
	pInstance->Add_GameObject_Out_of_Manager((CGameObject**)&pWeapon, SCENE_STATIC, TAG_OP(Prototype_WeaponKnife),&tWeaponDesc);
	NULL_CHECK_RETURN(pWeapon, E_FAIL);
	m_vecWeapon.push_back(pWeapon);

	pWeapon = nullptr;
	pInstance->Add_GameObject_Out_of_Manager((CGameObject**)&pWeapon, SCENE_STATIC, TAG_OP(Prototype_WeaponGrinder), &tWeaponDesc);
	NULL_CHECK_RETURN(pWeapon, E_FAIL);
	m_vecWeapon.push_back(pWeapon);

	pWeapon = nullptr;
	pInstance->Add_GameObject_Out_of_Manager((CGameObject**)&pWeapon, SCENE_STATIC, TAG_OP(Prototype_WeaponHorse), &tWeaponDesc);
	NULL_CHECK_RETURN(pWeapon, E_FAIL);
	m_vecWeapon.push_back(pWeapon);


	pWeapon = nullptr;
	pInstance->Add_GameObject_Out_of_Manager((CGameObject**)&pWeapon, SCENE_STATIC, TAG_OP(Prototype_WeaponTeapot), &tWeaponDesc);
	NULL_CHECK_RETURN(pWeapon, E_FAIL);
	m_vecWeapon.push_back(pWeapon);



	pWeapon = nullptr;
	pInstance->Add_GameObject_Out_of_Manager((CGameObject**)&pWeapon, SCENE_STATIC, TAG_OP(Prototype_WeaponUmbrella), &tWeaponDesc);
	NULL_CHECK_RETURN(pWeapon, E_FAIL);
	m_vecWeapon.push_back(pWeapon);

	pWeapon = nullptr;
	pInstance->Add_GameObject_Out_of_Manager((CGameObject**)&pWeapon, SCENE_STATIC, TAG_OP(Prototype_WeaponClockBomb), &tWeaponDesc);
	NULL_CHECK_RETURN(pWeapon, E_FAIL);
	m_vecWeapon.push_back(pWeapon);
	



	return S_OK;
}

HRESULT CPlayer::Renew_Player(_float3 Position , _float3 ReturnLookAt)
{


	m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, Position);

	m_vOldPos= m_vReturnPos = Position;
	m_vReturnLookAt = ReturnLookAt;

	m_CamDegreeAngle = _float3(20.f, 0, -5.f);


	_Vector LookDir = m_vReturnLookAt.XMVector() - m_vReturnPos.XMVector();
	LookDir = XMVector3Normalize(XMVectorSetY(LookDir, 0));

	_float CosAngle = XMVectorGetX(XMVector3Dot(LookDir, XMVectorSet(0, 0, 1, 0)));

	CosAngle = XMConvertToDegrees(acosf(CosAngle));
	
	if (Position.x > ReturnLookAt.x)
		CosAngle = (360) - CosAngle;
	
	m_CamDegreeAngle.y = CosAngle;


	_Vector ObjLook = ReturnLookAt.XMVector();
	ObjLook = XMVectorSetY(ObjLook, Position.y);

	m_pTransformCom->LookAt(ObjLook);

	//////////////////////////////////////////////////////////////////////////
	/*Cam Action*/

	vector<CAMACTDESC>		 vecCamPositions;
	vector<CAMACTDESC>		 vecLookPostions;

	CAMERAACTION tDesc;

	tDesc.vecCamPos = vecCamPositions;
	tDesc.vecLookAt = vecLookPostions;


	CAMACTDESC Return;

	Return.fDuration = 0.1f;
	Return.vPosition = Position.XMVector() + (m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK) * 4.f) + XMVectorSet(0, 3, 0, 0);
	tDesc.vecCamPos.push_back(Return);

	Return.fDuration = 1.5f;
	Return.vPosition = Position.XMVector() + (m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK) * 4.f) + XMVectorSet(0, 3, 0, 0);
	tDesc.vecCamPos.push_back(Return);


	_Matrix NewCamMatrix = XMMatrixTranslation(0, 1.5f * m_fSmallScale, m_CamDegreeAngle.z
		* m_fSmallScale * (1 - m_fDashPower / PlayerMaxDashPower * 0.1f))
		* XMMatrixRotationX(XMConvertToRadians(m_CamDegreeAngle.x))
		* XMMatrixRotationY(XMConvertToRadians(m_CamDegreeAngle.y))
		* XMMatrixTranslation(Position.x, Position.y, Position.z);

	Return.fDuration = 1.0f;
	Return.vPosition = NewCamMatrix.r[3];
	tDesc.vecCamPos.push_back(Return);





	Return.fDuration = 0.1f;
	Return.vPosition = Position .XMVector() + XMVectorSet(0,5,0,0);
	tDesc.vecLookAt.push_back(Return);


	Return.fDuration = 1.5f;
	Return.vPosition = Position.XMVector();
	tDesc.vecLookAt.push_back(Return);



	m_pMainCamera->CamActionStart(tDesc);


	m_bAliceSpwanAnimStart = true;
	m_SpwanAnimPassedTime = 0;
	m_pModel->Change_AnimIndex_ReturnTo_Must(36, 0, 0., true);

	m_bAliceDeathAnimStart = false;
	m_DeathAnimPassedTime = 0;
	m_bPlayerNotRender = false;

	GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_FadeIn, 1.5, { 0,0,0,1 });
	//////////////////////////////////////////////////////////////////////////




	m_bIsRockOn = false;
	Safe_Release(m_pRockOnMonster);



	m_LevitationTime = 0;
	m_iJumpCount = 0;


	m_fSmallScale = 1.f;
	m_fSmallPassedTime = FLT_MAX;
	m_fSmallVisualTime = 0;
	m_pTransformCom->Set_MoveSpeed(PlayerMoveSpeed);
	m_pTransformCom->Scaled_All(_float3(m_fSmallScale));
	m_pTransformCom->Set_MoveSpeed(PlayerMoveSpeed * m_fSmallScale);
	FAILED_CHECK(g_pGameInstance->EasingDiffuseLightDesc(LIGHTDESC::TYPE_DIRECTIONAL, 0, XMVectorSet(0.6f, 0.6f, 1, 1), m_fSmallVisualTime));

	m_vDashDir = _float3(0);
	m_fDashPower = 0;
	m_fDashPassedTime = 0;

	m_BombCoolTime = 0;

	m_BulletNormalInterver = 0;

	m_eNowWeapon = Weapon_None;

	m_bIsAttackClicked = false;
	m_iAttackCount = 0;
	ZeroMemory(m_bAtkMoveMentChecker,sizeof(_bool)*3);

	m_bIsZoom = false;
	m_bIsCharged = false;
	m_fCharedGauge = 0;
	m_bIsCoolTime = false;
	m_fUmbrellaIntro = 0;

	m_fGrinderCoolTime = 0;
	m_bNeedToGrinderCooling = false;

	m_bIsActived = false;
	m_vAddedForce = _float3(0, 0, 0);

	m_iHP = 32;
	CGamePlayUI* pGamePlayUI = ((CGamePlayUI*)(g_pGameInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_UI_GamePlay))));
	if (pGamePlayUI !=nullptr)	pGamePlayUI->Add_Dmg_to_Player(m_iHP, 0);
	

	m_iWeaponModelIndex = 10;

	m_bIsAttached = false;
	m_bSlide = false;
	m_bTrappedbyFlower = false;
	m_bGettingBigger = false;
	m_bIsGiant = false;
	m_GiantingPassedTime = FLT_MAX;
	m_bIsVenting = false;

	return S_OK;
}

void CPlayer::Set_GettingBigger(_bool bBool)
{
	if (m_GiantingPassedTime < 3) return;
	if (bBool)
	{
		m_pModel->Change_AnimIndex(Weapon_Giant + 4, 0.0f, true);
		

		GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_CamShaking, 2., _float4(0.5));
		//////////////////////////////////////////////////////////////////////////


		//vector<CAMACTDESC>		 vecCamPositions;
		//vector<CAMACTDESC>		 vecLookPostions;

		//CAMERAACTION tDesc;

		//tDesc.vecCamPos = vecCamPositions;
		//tDesc.vecLookAt = vecLookPostions;


		//CAMACTDESC Return;

		//Return.fDuration = 0.1f;
		//Return.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS)
		//	+ (XMVector3Normalize(m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK)) * 10.f);
		//tDesc.vecCamPos.push_back(Return);

		//Return.fDuration = 1.4f;
		//Return.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS)
		//	+ (XMVector3Normalize(m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK) )* 20.f)
		//	+ XMVectorSet(0, 15, 0, 0);
		//tDesc.vecCamPos.push_back(Return);




		//Return.fDuration = 0.1f;
		//Return.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS) + XMVectorSet(0, 2, 0, 0);
		//tDesc.vecLookAt.push_back(Return);

		//Return.fDuration = 2.5f;
		//Return.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS)+ XMVectorSet(0, 30, 0, 0);
		//tDesc.vecLookAt.push_back(Return);


		//m_pMainCamera->CamActionStart(tDesc);

		//////////////////////////////////////////////////////////////////////////

		m_bIsGiant = true;
		m_bGettingBigger = true;
		m_GiantingPassedTime = 0;
	}
	else
	{
		m_pModel->Change_AnimIndex(Weapon_Giant + 4, 0.08f, true);


		vector<CAMACTDESC>		 vecCamPositions;
		vector<CAMACTDESC>		 vecLookPostions;

		CAMERAACTION tDesc;

		tDesc.vecCamPos = vecCamPositions;
		tDesc.vecLookAt = vecLookPostions;


		CAMACTDESC Return;

		Return.fDuration = 0.1f;
		Return.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS)
			+ (XMVector3Normalize(m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK)) * 20.f)
			+ XMVectorSet(0, 15, 0, 0);
		tDesc.vecCamPos.push_back(Return);

		Return.fDuration = 2.5f;
		Return.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS)
			+ (XMVector3Normalize(m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK)) * 5.f)
			+ XMVectorSet(0, 2, 0, 0);
		tDesc.vecCamPos.push_back(Return);

		//Return.fDuration = 1.3f;
		//Return.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS)
		//	+ (XMVector3Normalize(m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK)) * 4.999f)
		//	+ XMVectorSet(0, 2, 0, 0);
		//tDesc.vecCamPos.push_back(Return);



		Return.fDuration = 0.1f;
		Return.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS) + XMVectorSet(0, 30, 0, 0);
		tDesc.vecLookAt.push_back(Return);

		Return.fDuration = 1.25f;
		Return.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS) + XMVectorSet(0, 2, 0, 0);
		tDesc.vecLookAt.push_back(Return);


		m_pMainCamera->CamActionStart(tDesc);



		m_bGettingBigger = false;
		m_GiantingPassedTime = 0;
	}

}

void CPlayer::Eat_Protain()
{


	m_iProteinChecker = 0;
	m_bEattingProteinStart = true;
	m_ProteinPassedTime = 0;
	m_pModel->Change_AnimIndex(5, 0, true);

}

HRESULT CPlayer::Ready_ParticleDesc()
{


	m_vecParticleDesc.clear();
	m_vecParticleDesc.reserve(5);

	//GetSingle(CUtilityMgr)->Create_ParticleObject(m_eNowSceneNum, m_vecParticleDesc[0]);

	/////////0//////////////////
	PARTICLEDESC tDesc;

	tDesc.eParticleTypeID = Particle_Ball;

	tDesc.FollowingTarget = m_pTransformCom;

	tDesc.szTextureProtoTypeTag = TAG_CP(Prototype_Texture_PlayerEffect);
	tDesc.szTextureLayerTag = L"ButterFly";
	tDesc.iSimilarLayerNum = 12;

	tDesc.TextureChageFrequency = 1;
	tDesc.vTextureXYNum = _float2(8, 2);

	tDesc.TotalParticleTime = 0.5f;
	tDesc.EachParticleLifeTime = 0.34f;
	tDesc.MaxParticleCount = 30;

	tDesc.SizeChageFrequency = 0;
	tDesc.ParticleSize = _float3(0.5, 0.5, 0.5);
	tDesc.ParticleSize2 = _float3(0.5, 0.5, 0.5);

	tDesc.ColorChageFrequency = 0;
	tDesc.TargetColor = _float4(1.f, 1.f, 1.f, 1.f);
	tDesc.TargetColor2 = _float4(1.f, 1.f, 1.f, 1.f);


	tDesc.Particle_Power = 1;
	tDesc.PowerRandomRange = _float2(0.5f, 1.5f);

	tDesc.vUp = _float3(0, 1, 0);

	tDesc.MaxBoundaryRadius = 5;

	tDesc.m_bIsUI = false;
	tDesc.m_bUIDepth = 0;

	tDesc.ParticleStartRandomPosMin = _float3(-1.0f, 0.0f, -1.0f);
	tDesc.ParticleStartRandomPosMax = _float3(1.f, 1.5f, 1.f);

	tDesc.DepthTestON = true;
	tDesc.AlphaBlendON = false;

	tDesc.m_fAlphaTestValue = 0.1f;
	tDesc.m_iPassIndex = 3;

	m_vecParticleDesc.push_back(tDesc);

	///////////1/////////////////////////////////////////////////////////////

	tDesc = PARTICLEDESC();

	tDesc.eParticleTypeID = Particle_Ball;

	tDesc.FollowingTarget = m_pTransformCom;

	tDesc.szTextureProtoTypeTag = TAG_CP(Prototype_Texture_PlayerEffect);
	tDesc.szTextureLayerTag = L"Dust2";
	tDesc.iSimilarLayerNum = 1;

	tDesc.TextureChageFrequency = 2;
	tDesc.vTextureXYNum = _float2(2, 2);

	tDesc.TotalParticleTime = 0.5;
	tDesc.EachParticleLifeTime = 0.34f;
	tDesc.MaxParticleCount = 5;

	tDesc.SizeChageFrequency = 1;
	tDesc.ParticleSize = _float3(3.f, 3.f, 3.f);
	tDesc.ParticleSize2 = _float3(2.2f, 2.2f, 2.2f);

	tDesc.ColorChageFrequency = 1;
	tDesc.TargetColor = _float4(0.4156862f, 0.352941f, 0.803921f, 1.f);
	tDesc.TargetColor2 = _float4(0.1803921f, 1.f, 1.f, 1.f);


	tDesc.Particle_Power = 2;
	tDesc.PowerRandomRange = _float2(0.5f, 1.5f);

	tDesc.vUp = _float3(0, 1, 0);

	tDesc.MaxBoundaryRadius = 5;

	tDesc.m_bIsUI = false;
	tDesc.m_bUIDepth = 0;

	tDesc.ParticleStartRandomPosMin = _float3(-1.0f, 0.5f, -1.0f);
	tDesc.ParticleStartRandomPosMax = _float3(1.f, 1.f, 1.f);

	tDesc.DepthTestON = true;
	tDesc.AlphaBlendON = true;

	tDesc.m_fAlphaTestValue = 0.18f;
	tDesc.m_iPassIndex = 4;

	m_vecParticleDesc.push_back(tDesc);


	////////////2////////////////////////////////////////////////////////////	 
	tDesc = PARTICLEDESC();

	tDesc.eParticleTypeID = Particle_Ball;

	tDesc.FollowingTarget = m_pTransformCom;

	tDesc.szTextureProtoTypeTag = TAG_CP(Prototype_Texture_PlayerEffect);
	tDesc.szTextureLayerTag = L"Dust2";
	tDesc.iSimilarLayerNum = 1;

	tDesc.TextureChageFrequency = 2;
	tDesc.vTextureXYNum = _float2(2, 2);

	tDesc.TotalParticleTime = 0.1f;
	tDesc.EachParticleLifeTime = 0.34f;
	tDesc.MaxParticleCount = 10;

	tDesc.SizeChageFrequency = 0;
	tDesc.ParticleSize = _float3(0.7f, 0.7f, 0.7f);
	tDesc.ParticleSize2 = _float3(0.5, 0.5, 0.5);

	tDesc.ColorChageFrequency = 0;
	tDesc.TargetColor = _float4(1.f, 1.f, 1.f, 1.f);
	//tDesc.TargetColor = _float4(0.4156862f, 0.352941f, 0.803921f, 1.f);
	tDesc.TargetColor2 = _float4(0.1803921f, 1.f, 1.f, 1.f);


	tDesc.Particle_Power = 2;
	tDesc.PowerRandomRange = _float2(0.5f, 1.5f);

	tDesc.vUp = _float3(0, 1, 0);

	tDesc.MaxBoundaryRadius = 5;

	tDesc.m_bIsUI = false;
	tDesc.m_bUIDepth = 0;

	tDesc.ParticleStartRandomPosMin = _float3(-1.0f, -0.2f, -1.0f);
	tDesc.ParticleStartRandomPosMax = _float3(1.f, 0.2f, 1.f);

	tDesc.DepthTestON = true;
	tDesc.AlphaBlendON = true;

	tDesc.m_fAlphaTestValue = 0.2f;
	tDesc.m_iPassIndex = 4;

	m_vecParticleDesc.push_back(tDesc);

	//////////3//////////////////////////////////////////////////////////////

	tDesc = PARTICLEDESC();

	tDesc.eParticleTypeID = Particle_Ball;

	tDesc.FollowingTarget = nullptr;

	tDesc.szTextureProtoTypeTag = TAG_CP(Prototype_Texture_PlayerEffect);
	tDesc.szTextureLayerTag = L"Explosion";
	tDesc.iSimilarLayerNum = 2;

	tDesc.TextureChageFrequency = 1;
	tDesc.vTextureXYNum = _float2(5, 4);

	tDesc.TotalParticleTime = 0.1f;
	tDesc.EachParticleLifeTime = 0.34f;

	tDesc.MaxParticleCount = 1;

	tDesc.SizeChageFrequency = 1;
	tDesc.ParticleSize = _float3(0.2f, 0.2f, 0.2f);
	tDesc.ParticleSize2 = _float3(1.f, 1.f, 1.f);

	tDesc.ColorChageFrequency = 0;
	tDesc.TargetColor = _float4(1.f, 1.f, 1.f, 0.7f);
	tDesc.TargetColor2 = _float4(1.f, 1.f, 1.f, 1.f);


	tDesc.Particle_Power = 1;
	tDesc.PowerRandomRange = _float2(0.5f, 1.0f);

	tDesc.vUp = _float3(0, 1, 0);

	tDesc.MaxBoundaryRadius = 3;

	tDesc.m_bIsUI = false;
	tDesc.m_bUIDepth = 0;

	tDesc.ParticleStartRandomPosMin = _float3(-0.05f, 0.0f, -0.05f);
	tDesc.ParticleStartRandomPosMax = _float3(0.05f, 0.1f, 0.05f);

	tDesc.DepthTestON = true;
	tDesc.AlphaBlendON = true;

	tDesc.m_fAlphaTestValue = 0.1f;
	tDesc.m_iPassIndex = 3;


	m_vecParticleDesc.push_back(tDesc);


	/////////4///////////////////////////////////////////////////////////////
	tDesc = PARTICLEDESC();

	tDesc.eParticleTypeID = Particle_Ball;

	tDesc.FollowingTarget = m_pTransformCom;

	tDesc.szTextureProtoTypeTag = TAG_CP(Prototype_Texture_PlayerEffect);
	tDesc.szTextureLayerTag = L"Petal";
	tDesc.iSimilarLayerNum = 3;

	tDesc.TextureChageFrequency = 1;
	tDesc.vTextureXYNum = _float2(8, 4);

	tDesc.TotalParticleTime = 1.f;
	tDesc.EachParticleLifeTime = 0.34f;
	tDesc.MaxParticleCount = 15;

	tDesc.SizeChageFrequency = 0;
	tDesc.ParticleSize = _float3(0.15f, 0.15f, 0.15f);
	tDesc.ParticleSize2 = _float3(0.5f, 0.5f, 0.5f);

	tDesc.ColorChageFrequency = 0;
	tDesc.TargetColor = _float4(1.f, 1.f, 1.f, 1.f);
	tDesc.TargetColor2 = _float4(1.f, 1.f, 1.f, 1.f);


	tDesc.Particle_Power = 2;
	tDesc.PowerRandomRange = _float2(0.8f, 1.5f);

	tDesc.vUp = _float3(0, 1, 0);

	tDesc.MaxBoundaryRadius = 6;

	tDesc.m_bIsUI = false;
	tDesc.m_bUIDepth = 0;

	tDesc.ParticleStartRandomPosMin = _float3(-1.0f, 0.2f, -1.0f);
	tDesc.ParticleStartRandomPosMax = _float3(1.f, 0.7f, 1.f);

	tDesc.DepthTestON = true;
	tDesc.AlphaBlendON = false;

	tDesc.m_fAlphaTestValue = 0.1f;
	tDesc.m_iPassIndex = 3;


	m_vecParticleDesc.push_back(tDesc);

















	return S_OK;
}

HRESULT CPlayer::Manage_CoolTime(_double fDeltaTime)
{

	m_fGrinderCoolTime -= g_fDeltaTime;
	if (m_fGrinderCoolTime < 0)
	{
		m_fGrinderCoolTime = 0;
		m_bNeedToGrinderCooling = false;
	}

	return S_OK;
}

HRESULT CPlayer::Update_SpwanNDeathAnim(_double fDeltaTime)
{
	if (m_bAliceSpwanAnimStart)
	{
		m_SpwanAnimPassedTime += fDeltaTime;
		static _double ParticleInterver = 0;
		ParticleInterver += fDeltaTime;

		if (ParticleInterver > 0.2)
		{

			m_vecParticleDesc[2].FollowingTarget = nullptr;
			m_vecParticleDesc[4].FollowingTarget = nullptr;

			_float EasedHeight = g_pGameInstance->Easing(TYPE_Linear, 5, 0, (_float)m_SpwanAnimPassedTime, 1.5);

			m_vecParticleDesc[2].FixedTarget = m_vecParticleDesc[4].FixedTarget = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS) + XMVectorSet(0, EasedHeight, 0, 0);

			GetSingle(CUtilityMgr)->Create_ParticleObject(m_eNowSceneNum, m_vecParticleDesc[2]);
			GetSingle(CUtilityMgr)->Create_ParticleObject(m_eNowSceneNum, m_vecParticleDesc[4]);
			g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerTornado),
				&_float4(m_vecParticleDesc[2].FixedTarget, 1));
			g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerTornado),
				&_float4(m_vecParticleDesc[2].FixedTarget, 1));
			g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerTornado),
				&_float4(m_vecParticleDesc[2].FixedTarget, 1));
			ParticleInterver = 0;
		}

		if (m_SpwanAnimPassedTime > 1.5f)
		{
			m_bAliceSpwanAnimStart = false;
			m_vecParticleDesc[2].FollowingTarget = m_pTransformCom;
			m_vecParticleDesc[4].FollowingTarget = m_pTransformCom;
		}



	}
	else if (m_bAliceDeathAnimStart)
	{
		m_DeathAnimPassedTime+= fDeltaTime;

		if (m_pModel->Get_PlayRate() > 0.603773 && !m_bPlayerNotRender)
		{
			m_bPlayerNotRender = true;

			CUtilityMgr* pUtil = GetSingle(CUtilityMgr);

			pUtil->Create_ParticleObject(m_eNowSceneNum, m_vecParticleDesc[1]);
			pUtil->Create_ParticleObject(m_eNowSceneNum, m_vecParticleDesc[0]);
			pUtil->Create_ParticleObject(m_eNowSceneNum, m_vecParticleDesc[1]);
			pUtil->Create_ParticleObject(m_eNowSceneNum, m_vecParticleDesc[0]);
			pUtil->Create_ParticleObject(m_eNowSceneNum, m_vecParticleDesc[1]);
			pUtil->Create_ParticleObject(m_eNowSceneNum, m_vecParticleDesc[0]);
			pUtil->Start_ScreenEffect(CUtilityMgr::ScreenEffect_FadeOut, 1.0, { 0,0,0,1 });
		}


		if (m_DeathAnimPassedTime > 3.)
		{
			m_bAliceDeathAnimStart = false;
			Renew_Player(m_vReturnPos, m_vReturnLookAt);
		}

	}

	return S_OK;
}

HRESULT CPlayer::Input_Keyboard(_double fDeltaTime)
{
	if (m_bAliceDeathAnimStart || m_bAliceSpwanAnimStart)return S_FALSE;

	CGameInstance* pInstance = GetSingle(CGameInstance);

	if (m_bIsVenting)
	{
		if (m_pModel->Get_NowAnimIndex() != 8)
		{
			m_pModel->Change_AnimIndex(8);
		}

		_Vector vVentingTargetLook = XMVector3Normalize(m_vVentingTargetLook.XMVector());
		_Vector vOldLook = m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK);

		if (XMVectorGetX(XMVector3Length(vVentingTargetLook + vOldLook)) < 0.1f)
			m_pTransformCom->LookDir(m_pTransformCom->Get_MatrixState(CTransform::STATE_RIGHT) + vOldLook);
		else
			m_pTransformCom->LookDir((vVentingTargetLook  *0.25 + vOldLook * 0.75));



		return S_FALSE;
	}

	if (m_bIsGiant)
	{

		if (m_GiantingPassedTime < 3.0)
		{
			Giantting_Update(fDeltaTime, pInstance);
		}
		else
		{

			Move_Update_Giant(fDeltaTime, pInstance);
			Jump_Update_Giant(fDeltaTime, pInstance);
			Attack_Update_Giant(fDeltaTime, pInstance);

		}

	}
	else
	{


		/*To All*/
		if (m_eNowWeapon != CPlayer::Weapon_Umbrella)
			FAILED_CHECK(Dash_Update(fDeltaTime, pInstance));
		FAILED_CHECK(RockOn_Update(fDeltaTime, pInstance));
		FAILED_CHECK(TrappedbyFlower_Update(fDeltaTime, pInstance));

		if (m_bTrappedbyFlower)return S_FALSE;

		switch (m_eNowWeapon)
		{
		case Client::CPlayer::Weapon_None:
		{

			FAILED_CHECK(Smalling_Update(fDeltaTime, pInstance));
			FAILED_CHECK(Plant_ClockBomb(fDeltaTime, pInstance));
			//if (!m_fDashPassedTime)
			FAILED_CHECK(Move_Update(fDeltaTime, pInstance));
			FAILED_CHECK(Jump_Update(fDeltaTime, pInstance));

		}
		break;
		case Client::CPlayer::Weapon_Knife:
		{

			FAILED_CHECK(Attack_Update_Knife(fDeltaTime, pInstance));

			//if (!m_fDashPassedTime)
			FAILED_CHECK(Move_Update_Knife(fDeltaTime, pInstance));
			FAILED_CHECK(Jump_Update_Knife(fDeltaTime, pInstance));

		}
		break;
		case Client::CPlayer::Weapon_Grinder:
		{

			FAILED_CHECK(Attack_Update_Grinder(fDeltaTime, pInstance));

			//if (!m_fDashPassedTime)

			FAILED_CHECK(Move_Update_Grinder(fDeltaTime, pInstance));
			FAILED_CHECK(Jump_Update_Grinder(fDeltaTime, pInstance));
		}
		break;


		case Client::CPlayer::Weapon_Horse:
		{

			FAILED_CHECK(Attack_Update_Horse(fDeltaTime, pInstance));

			//if (!m_fDashPassedTime)

			FAILED_CHECK(Move_Update_Horse(fDeltaTime, pInstance));
			FAILED_CHECK(Jump_Update_Horse(fDeltaTime, pInstance));

		}
		break;
		case Client::CPlayer::Weapon_Teapot:
		{

			FAILED_CHECK(Attack_Update_Teapot(fDeltaTime, pInstance));

			//if (!m_fDashPassedTime)

			FAILED_CHECK(Move_Update_Teapot(fDeltaTime, pInstance));
			FAILED_CHECK(Jump_Update_Teapot(fDeltaTime, pInstance));

		}
		break;
		case Client::CPlayer::Weapon_Umbrella:
		{
			m_fUmbrellaIntro += _float(fDeltaTime);
			if (m_fUmbrellaIntro > 1) m_fUmbrellaIntro = 1;

			FAILED_CHECK(Attack_Update_Umbrella(fDeltaTime, pInstance));

			if (!m_fDashPassedTime)
				FAILED_CHECK(Move_Update_Umbrella(fDeltaTime, pInstance));
			FAILED_CHECK(Jump_Update_Umbrella(fDeltaTime, pInstance));
		}
		break;
		default:
			break;
		}



	}


	return S_OK;
}

HRESULT CPlayer::TrappedbyFlower_Update(_double fDeltaTime, CGameInstance * pInstance)
{
	if (m_bTrappedbyFlower)
	{
		if (m_fSmallPassedTime < 0.3f)
		{

			m_fSmallPassedTime += _float(fDeltaTime);
			m_fSmallScale = pInstance->Easing(TYPE_QuarticOut, 1.f, PlayerSmallingSize, m_fSmallPassedTime, 0.3f);

			if (m_fSmallPassedTime > 0.3f)
			{
				m_fSmallScale = PlayerSmallingSize;
			}

			m_fSmallVisualTime = (1 - m_fSmallScale) / (1 - PlayerSmallingSize);
			FAILED_CHECK(pInstance->EasingDiffuseLightDesc(LIGHTDESC::TYPE_DIRECTIONAL, 0, XMVectorSet(0.6f, 0.6f, 1, 1), m_fSmallVisualTime));

			m_pTransformCom->Scaled_All(_float3(m_fSmallScale));
			m_pTransformCom->Set_MoveSpeed(PlayerMoveSpeed * m_fSmallScale);
		}

		if ((pInstance->Get_DIKeyState(DIK_SPACE) & DIS_Down) || (pInstance->Get_DIKeyState(DIK_LSHIFT) & DIS_Down))
		{
			m_fSmallPassedTime = 0;
			m_bTrappedbyFlower = false;
		}

	}
	else if (!(pInstance->Get_DIKeyState(DIK_LCONTROL) & DIS_Press))
	{
		
		if (m_fSmallPassedTime < 0.3f)
		{
			m_fSmallPassedTime += _float(fDeltaTime);
			m_fSmallScale = pInstance->Easing(TYPE_QuarticOut, PlayerSmallingSize, 1.f, m_fSmallPassedTime, 0.3f);

			if (m_fSmallPassedTime > 0.3f)
			{
				m_fSmallScale = 1.f;
			}

			m_pTransformCom->Scaled_All(_float3(m_fSmallScale));
			m_pTransformCom->Set_MoveSpeed(PlayerMoveSpeed * m_fSmallScale);

		}

		if (m_fSmallVisualTime > 0.f)
		{
			m_fSmallVisualTime -= _float(fDeltaTime);
			if (m_fSmallVisualTime < 0) m_fSmallVisualTime = 0;
			FAILED_CHECK(pInstance->EasingDiffuseLightDesc(LIGHTDESC::TYPE_DIRECTIONAL, 0, XMVectorSet(0.6f, 0.6f, 1, 1), m_fSmallVisualTime));

		}
	}



	return S_OK;
}

HRESULT CPlayer::Smalling_Update(_double fDeltaTime, CGameInstance* pInstance)
{

	BYTE KeyState = pInstance->Get_DIKeyState(DIK_LCONTROL);
	if (!m_iJumpCount && (KeyState & DIS_Press))
	{
		if (KeyState & DIS_Down)
		{

			if (!m_pModel->Get_IsHavetoBlockAnimChange())
			{
				m_fSmallPassedTime = 0;
				m_pModel->Change_AnimIndex_ReturnTo(9, 0, 0.15, true);
			}
		}
		else if (KeyState & DIS_Up)
		{
			if (!m_pModel->Get_IsHavetoBlockAnimChange())
			{
				m_fSmallPassedTime = 0;
				m_pModel->Change_AnimIndex_ReturnTo(9, 0, 0.15, true);
			}
		}
		else
		{
			if (m_fSmallPassedTime < 0.3f)
			{

				m_fSmallPassedTime += _float(fDeltaTime);
				m_fSmallScale = pInstance->Easing(TYPE_QuarticOut, 1.f, PlayerSmallingSize, m_fSmallPassedTime, 0.3f);

				if (m_fSmallPassedTime > 0.3f)
				{
					m_fSmallScale = PlayerSmallingSize;
				}

				m_fSmallVisualTime = (1 - m_fSmallScale) / (1 - PlayerSmallingSize);
				FAILED_CHECK(pInstance->EasingDiffuseLightDesc(LIGHTDESC::TYPE_DIRECTIONAL, 0, XMVectorSet(0.6f, 0.6f, 1, 1), m_fSmallVisualTime));

				m_pTransformCom->Scaled_All(_float3(m_fSmallScale));
				m_pTransformCom->Set_MoveSpeed(PlayerMoveSpeed * m_fSmallScale);
			}

		}
	}

	return S_OK;
}

HRESULT CPlayer::Move_Update(_double fDeltaTime, CGameInstance* pInstance)
{

	if (!m_pModel->Get_IsHavetoBlockAnimChange())
	{

		_int PressedChecker[4];
		ZeroMemory(PressedChecker, sizeof(_bool) * 4);

		PressedChecker[0] = _int(pInstance->Get_DIKeyState(DIK_W) & DIS_Press);
		PressedChecker[1] = _int(pInstance->Get_DIKeyState(DIK_S) & DIS_Press);
		PressedChecker[2] = _int(pInstance->Get_DIKeyState(DIK_A) & DIS_Press);
		PressedChecker[3] = _int(pInstance->Get_DIKeyState(DIK_D) & DIS_Press);

		if (PressedChecker[0] || PressedChecker[1] || PressedChecker[2] || PressedChecker[3])
		{
			//m_pTransformCom->LookDir(XMVectorSetY(m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), 0)
			//	- XMVectorSetY(m_pMainCamera->Get_Camera_Transform()->Get_MatrixState(CTransform::STATE_POS), 0));

			_Vector forword = XMVector3Normalize(XMVectorSetY(m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), 0)
				- XMVectorSetY(m_pMainCamera->Get_Camera_Transform()->Get_MatrixState(CTransform::STATE_POS), 0));

			_Vector right = XMVector3Cross(XMVectorSet(0, 1, 0, 0), forword);

			_Vector Dir = XMVectorSet(0, 0, 0, 0);

			/*
			//if (PressedChecker[0]) Dir += m_pTransformCom->Get_MatrixState_Normalized(CTransform::STATE_LOOK);
			//if (PressedChecker[1]) Dir -= m_pTransformCom->Get_MatrixState_Normalized(CTransform::STATE_LOOK);
			//if (PressedChecker[2]) Dir -= m_pTransformCom->Get_MatrixState_Normalized(CTransform::STATE_RIGHT);
			//if (PressedChecker[3]) Dir += m_pTransformCom->Get_MatrixState_Normalized(CTransform::STATE_RIGHT);
			*/

			if (PressedChecker[0] && !m_bSlide) Dir += forword;
			if (PressedChecker[1] && !m_bSlide) Dir -= forword;
			if (PressedChecker[2]) Dir -= right;
			if (PressedChecker[3]) Dir += right;

			Dir = XMVector3Normalize(Dir);
			_Vector vOldLook = m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK);

			if (XMVectorGetX(XMVector3Length(Dir + vOldLook)) < 0.1f)
				m_pTransformCom->LookDir( m_pTransformCom->Get_MatrixState(CTransform::STATE_RIGHT) + vOldLook);
			else
				m_pTransformCom->LookDir((Dir *0.25 + vOldLook * 0.75));

			if (m_fDashPassedTime)
			{
				m_vDashDir = Dir;
			}
			else
			{
				m_pTransformCom->MovetoDir(XMVector3Normalize(Dir), fDeltaTime);


				if (!m_LevitationTime && !m_bSlide)
					m_pModel->Change_AnimIndex(8);
			}


		}
		else {
			if (!m_LevitationTime &&!m_fDashPassedTime && !m_bSlide)
				m_pModel->Change_AnimIndex_UntilTo(0, 5);


		}
	}
	return S_OK;
}

HRESULT CPlayer::Jump_Update(_double fDeltaTime, CGameInstance* pInstance)
{
	if (!m_pModel->Get_IsHavetoBlockAnimChange())
	{
		if (m_iJumpCount < 3 && !m_fDashPassedTime && pInstance->Get_DIKeyState(DIK_SPACE) & DIS_Down)
		{
			Add_JumpForce(PlayerMaxJumpPower * m_fSmallScale);

			_uint iJumpIndex = (m_iJumpCount) ? 1: 0;
			m_pModel->Change_AnimIndex_UntilTo(15 + iJumpIndex * 5, 17 + iJumpIndex * 5, 0.08);

			if (m_iJumpCount)
			{
				GetSingle(CUtilityMgr)->Create_ParticleObject(m_eNowSceneNum, m_vecParticleDesc[2]);
				GetSingle(CUtilityMgr)->Create_ParticleObject(m_eNowSceneNum, m_vecParticleDesc[4]);
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerTornado),
					&_float4(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS), 1));
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerTornado),
					&_float4(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS), 1));
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerTornado),
					&_float4(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS), 1));
			}
			m_iJumpCount++;

		}
	}
	m_LevitationTime += fDeltaTime;
	_float fGravity = 0;
	if (m_fJumpPower > 0)
	{
		m_fJumpPower = _float(m_fMaxJumpPower * (m_LevitationTime - 1.f)* (m_LevitationTime - 1.f));
		fGravity = m_fJumpPower;

		if (m_fJumpPower <= 1.0f)
		{
			m_fJumpPower = 0;
			m_LevitationTime = fDeltaTime;
		}
	}
	else
	{
		if (m_iJumpCount && pInstance->Get_DIKeyState(DIK_SPACE) & DIS_Press)
		{
			static _double particleTimeInterver = 0;
			particleTimeInterver += fDeltaTime;

			if (particleTimeInterver > 0.2)
			{

				GetSingle(CUtilityMgr)->Create_ParticleObject(m_eNowSceneNum, m_vecParticleDesc[2]);
				GetSingle(CUtilityMgr)->Create_ParticleObject(m_eNowSceneNum, m_vecParticleDesc[4]);
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerTornado),
					&_float4(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS), 1));
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerTornado),
					&_float4(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS), 1));
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerTornado),
					&_float4(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS), 1));
				particleTimeInterver = 0;
			}
			m_LevitationTime -= fDeltaTime*0.8;
			//fGravity = _float((m_LevitationTime) * -2.4f);

		}


		if (m_LevitationTime == fDeltaTime)
			fGravity = _float(-2.94f);
		else
			fGravity = _float((m_LevitationTime) * (m_LevitationTime)* -29.4f);



	}

	_uint iNowAnimIndex = m_pModel->Get_NowAnimIndex();
	if (m_LevitationTime != fDeltaTime && fGravity < 0 && !m_bIsAttached && iNowAnimIndex !=42 && iNowAnimIndex !=43)
		m_pModel->Change_AnimIndex(17);

	m_pTransformCom->MovetoDir_bySpeed(XMVectorSet(0, 1.f, 0, 0), fGravity, fDeltaTime);

	return S_OK;
}

HRESULT CPlayer::Dash_Update(_double fDeltaTime, CGameInstance* pInstance, _float TotalDashTime)
{
	if (!m_fDashPassedTime && pInstance->Get_DIKeyState(DIK_LSHIFT) & DIS_Down)
	{

		GetSingle(CUtilityMgr)->Create_ParticleObject(m_eNowSceneNum, m_vecParticleDesc[1]);
		GetSingle(CUtilityMgr)->Create_ParticleObject(m_eNowSceneNum, m_vecParticleDesc[0]);

	
		switch (m_eNowWeapon)
		{
		case Client::CPlayer::Weapon_None:
			m_pModel->Change_AnimIndex(8);
			break;
		case Client::CPlayer::Weapon_Knife:
			m_pModel->Change_AnimIndex(m_eNowWeapon + 2);
			break;
		case Client::CPlayer::Weapon_Grinder:
			m_pModel->Change_AnimIndex(m_eNowWeapon + 4);
			break;
		case Client::CPlayer::Weapon_Horse:
			m_pModel->Change_AnimIndex(m_eNowWeapon + 2);
			break;
		case Client::CPlayer::Weapon_Teapot:
			m_pModel->Change_AnimIndex(m_eNowWeapon + 2);
			break;
		default:
			break;
		}


		ZeroMemory(m_bAtkMoveMentChecker, sizeof(_bool) * 3);
		m_pModel->Set_BlockAnim(false);
		m_bIsAttackClicked = false;
		m_iAttackCount = 0;
		m_bIsZoom = false;
		m_bIsCharged = false;
		m_fCharedGauge = 0;
		m_bIsCoolTime = false;
		m_fUmbrellaIntro = 0;

		//m_vDashDir = m_pMainCamera->Get_Camera_Transform()->Get_MatrixState_Float3(CTransform::STATE_LOOK);
		if (m_bIsRockOn)
		{
			m_vDashDir = XMVector3Normalize(((CTransform*)(m_pRockOnMonster->Get_Component(TAG_COM(Com_Transform))))->Get_MatrixState(CTransform::STATE_POS) 
				- m_pTransformCom->Get_MatrixState(CTransform::STATE_POS));
		}
		else {
			m_vDashDir = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_LOOK);
		}
		m_vDashDir.y += m_pMainCamera->Get_Camera_Transform()->Get_MatrixState_Float3(CTransform::STATE_LOOK).y * 1.5f;
		m_fDashPassedTime =_float(fDeltaTime);
		m_fDashPower = PlayerMaxDashPower;
	}

	if (m_fDashPassedTime)
	{
		m_fDashPassedTime += _float(fDeltaTime);

		m_fDashPower = pInstance->Easing_Return(TYPE_Linear, TYPE_Linear, 0, PlayerMaxDashPower, m_fDashPassedTime, TotalDashTime);
		
		if (m_fDashPassedTime > TotalDashTime)
		{
			m_fDashPower = 0;
			m_fDashPassedTime = 0;
		}

		m_pTransformCom->MovetoDir_bySpeed(m_vDashDir.XMVector(), m_fDashPower, fDeltaTime);
	}



	return S_OK;
}

HRESULT CPlayer::RockOn_Update(_double fDeltaTime, CGameInstance * pInstance)
{
	//BYTE LTabState = pInstance->Get_DIKeyState(DIK_TAB);

	//if (LTabState & DIS_Down)
	//{
	//	if (LTabState & DIS_DoubleDown)
	//	{
	//		Safe_Release(m_pRockOnMonster);

	//		m_bIsRockOn = false;
	//	}
	//	else 
	//	{
	//		_Vector vPlayerPos = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
	//		_Vector vLookVector = XMVector3Normalize(XMVectorSetY(vPlayerPos, 0)
	//			- XMVectorSetY(m_pMainCamera->Get_Camera_Transform()->Get_MatrixState(CTransform::STATE_POS), 0));

	//		//나중에 몬스터 넣으면 몬스터로 바꾸자
	//		list<CGameObject*>* MosterList = pInstance->Get_ObjectList_from_Layer(m_eNowSceneNum, TAG_LAY(Layer_JumpPad));

	//		_float	fNear = 5.f;
	//		_float Length = 0;
	//		CGameObject* pNearMonster = nullptr;
	//		
	//		_Vector vMonsterPos;

	//		for (auto& pMonster : *MosterList)
	//		{
	//			vMonsterPos = ((CTransform*)(pMonster->Get_Component(TAG_COM(Com_Transform))))->Get_MatrixState(CTransform::STATE_POS);


	//			_float fCosValue = XMVectorGetX(XMVector3Dot(vLookVector, XMVector3Normalize
	//			(XMVectorSetY(vMonsterPos, 0) - XMVectorSetY(vPlayerPos, 0))));

	//			if (fCosValue > (sqrtf(3) / 2.f) && fNear > (Length = XMVectorGetX(XMVector3Length(vPlayerPos - vMonsterPos))))
	//			{
	//				fNear = Length;
	//				pNearMonster = pMonster;
	//			}

	//		}

	//		if (pNearMonster == nullptr)
	//			return S_FALSE;

	//		Safe_Release(m_pRockOnMonster);
	//		m_pRockOnMonster = pNearMonster;
	//		Safe_AddRef(m_pRockOnMonster);
	//		m_bIsRockOn = true;
	//	}
	//}








	return S_OK;
}

HRESULT CPlayer::Plant_ClockBomb(_double fDeltaTime, CGameInstance * pInstance)
{

	if (m_BombCoolTime > 0)
	{
		m_BombCoolTime -= fDeltaTime;

	}
	else
	{
		if (pInstance->Get_DIKeyState(DIK_E) & DIS_Down)
		{
			m_pModel->Change_AnimIndex_ReturnTo(44, 0, 0.15, true);

			g_pGameInstance->Add_GameObject_To_Layer_By_Parameter(m_eNowSceneNum, TAG_LAY(Layer_ClockBomb), ((CClockBomb*)(m_vecWeapon[5]))->Clone2Clone(nullptr));

			m_BombCoolTime = 10;
		}

	}



	return S_OK;
}

HRESULT CPlayer::Lunch_Bullet(_double fDeltaTime, CGameInstance * pInstance)
{
	
	if (!m_bNeedToGrinderCooling  && pInstance->Get_DIMouseButtonState(CInput_Device::MBS_LBUTTON) & DIS_Press)
	{

		m_pModel->Change_AnimIndex(Weapon_Grinder + 8,0.15, true);
		m_vecWeapon[m_iWeaponModelIndex]->Get_WeaponModel()->Change_AnimIndex(1,0.15);



		m_BulletNormalInterver -=fDeltaTime;
		if (pInstance->Get_DIMouseButtonState(CInput_Device::MBS_LBUTTON) & DIS_Up)
		{
			m_vecWeapon[m_iWeaponModelIndex]->Get_WeaponModel()->Change_AnimIndex_ReturnTo_Must(3, 0,0.15,true);

			m_BulletNormalInterver = 0;
		}

		if (m_BulletNormalInterver < 0)
		{
			//_Vector vPlayerLook = m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK);

			//_Vector vPlayerPos = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);

			//CTransform* pCamTransform = m_pMainCamera->Get_Camera_Transform();
			//_Matrix	CamInverce = pCamTransform->Get_InverseWorldMatrix();
			//_Vector PlayerInVewSpace = XMVector3TransformCoord(vPlayerPos, CamInverce);

			//_float3 vBulletDir = XMVector3Normalize(XMVector3TransformNormal(XMVectorSet(0, 0, 300, 0) - PlayerInVewSpace, pCamTransform->Get_WorldMatrix()));
			//_float3 vBulletDir = XMVector3Normalize(vPlayerLook + XMVectorSet(0, -m_CamDegreeAngle.z * 0.1f, 0 , 0));

			CTransform* pCamTransform = m_pMainCamera->Get_Camera_Transform();

			
			_float3 vBulletDir = XMVector3Normalize(XMVector3TransformNormal(
				XMVectorSet(0, 0, 10, 0) - m_vBulletFirePos.Multiply_Matrix_AsPosVector(pCamTransform->Get_InverseWorldMatrix()), 
				pCamTransform->Get_WorldMatrix()));

			m_vecParticleDesc[3].FixedTarget = m_vBulletFirePos;
			GetSingle(CUtilityMgr)->Create_ParticleObject(m_eNowSceneNum, m_vecParticleDesc[3]);

			pInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Bullet), TAG_OP(Prototype_Bullet_Normal),	&vBulletDir);

			
			 m_BulletNormalInterver = GetSingle(CUtilityMgr)->RandomFloat(0.15f, 0.2f);
			 m_fGrinderCoolTime += m_BulletNormalInterver*PlayerGrinderCoolTime*0.8;


			 if (m_fGrinderCoolTime > PlayerGrinderCoolTime)
			 {
				 m_vecWeapon[m_iWeaponModelIndex]->Get_WeaponModel()->Change_AnimIndex_ReturnTo_Must(3, 0, 0.15, true);
				 m_fGrinderCoolTime = PlayerGrinderCoolTime;
				 m_bNeedToGrinderCooling = true;
			 }
		}
	}
	else
	{
		if (m_pModel->Get_NowAnimIndex() == Weapon_Grinder + 8)
		{
			m_pModel->Change_AnimIndex(Weapon_Grinder + 3);

		}

	}

	return S_OK;
}

HRESULT CPlayer::Move_Update_Horse(_double fDeltaTime, CGameInstance * pInstance)
{
	if (!m_pModel->Get_IsHavetoBlockAnimChange())
	{

		_int PressedChecker[4];
		ZeroMemory(PressedChecker, sizeof(_bool) * 4);

		PressedChecker[0] = _int(pInstance->Get_DIKeyState(DIK_W) & DIS_Press);
		PressedChecker[1] = _int(pInstance->Get_DIKeyState(DIK_S) & DIS_Press);
		PressedChecker[2] = _int(pInstance->Get_DIKeyState(DIK_A) & DIS_Press);
		PressedChecker[3] = _int(pInstance->Get_DIKeyState(DIK_D) & DIS_Press);

		if (PressedChecker[0] || PressedChecker[1] || PressedChecker[2] || PressedChecker[3])
		{
			//m_pTransformCom->LookDir(XMVectorSetY(m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), 0)
			//	- XMVectorSetY(m_pMainCamera->Get_Camera_Transform()->Get_MatrixState(CTransform::STATE_POS), 0));

			_Vector forword = XMVector3Normalize(XMVectorSetY(m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), 0)
				- XMVectorSetY(m_pMainCamera->Get_Camera_Transform()->Get_MatrixState(CTransform::STATE_POS), 0));

			_Vector right = XMVector3Cross(XMVectorSet(0, 1, 0, 0), forword);

			_Vector Dir = XMVectorSet(0, 0, 0, 0);


			if (PressedChecker[0]) Dir += forword;
			if (PressedChecker[1]) Dir -= forword;
			if (PressedChecker[2]) Dir -= right;
			if (PressedChecker[3]) Dir += right;

			Dir = XMVector3Normalize(Dir);
			_Vector vOldLook = m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK);

			if (XMVectorGetX(XMVector3Length(Dir + vOldLook)) < 0.1f)
				m_pTransformCom->LookDir(m_pTransformCom->Get_MatrixState(CTransform::STATE_RIGHT) + vOldLook);
			else
				m_pTransformCom->LookDir((Dir *0.25 + vOldLook * 0.75));




			if (m_fDashPassedTime)
			{
				m_vDashDir = Dir;
			}
			else
			{

				m_pTransformCom->MovetoDir(XMVector3Normalize(Dir), fDeltaTime);


				if (!m_LevitationTime)
					m_pModel->Change_AnimIndex(Weapon_Horse + 2);


			}

		}
		else {
			if (!m_LevitationTime && !m_fDashPassedTime)
				m_pModel->Change_AnimIndex(Weapon_Horse + 0);


		}
	}
	return S_OK;
}

HRESULT CPlayer::Jump_Update_Horse(_double fDeltaTime, CGameInstance * pInstance)
{
	if (!m_pModel->Get_IsHavetoBlockAnimChange())
	{
		if (m_iJumpCount < 3 && !m_fDashPassedTime&& pInstance->Get_DIKeyState(DIK_SPACE) & DIS_Down)
		{
			Add_JumpForce(PlayerMaxJumpPower * m_fSmallScale);

			_uint iJumpIndex = (m_iJumpCount) ? 1 : 0;
			m_pModel->Change_AnimIndex_UntilTo(15 + iJumpIndex * 5, 17 + iJumpIndex * 5, 0.08);


			if (m_iJumpCount)
			{
				GetSingle(CUtilityMgr)->Create_ParticleObject(m_eNowSceneNum, m_vecParticleDesc[2]);
				GetSingle(CUtilityMgr)->Create_ParticleObject(m_eNowSceneNum, m_vecParticleDesc[4]);
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerTornado),
					&_float4(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS), 1));
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerTornado),
					&_float4(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS), 1));
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerTornado),
					&_float4(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS), 1));
			}
			m_iJumpCount++;

		}
	}
	m_LevitationTime += fDeltaTime;
	_float fGravity = 0;
	if (m_fJumpPower > 0)
	{
		m_fJumpPower = _float(m_fMaxJumpPower * (m_LevitationTime - 1.f)* (m_LevitationTime - 1.f));
		fGravity = m_fJumpPower;

		if (m_fJumpPower <= 1.0f)
		{
			m_fJumpPower = 0;
			m_LevitationTime = fDeltaTime;
		}
	}
	else
	{
		if (m_iJumpCount && pInstance->Get_DIKeyState(DIK_SPACE) & DIS_Press)
		{
			static _double particleTimeInterver = 0;
			particleTimeInterver += fDeltaTime;

			if (particleTimeInterver > 0.2)
			{

				GetSingle(CUtilityMgr)->Create_ParticleObject(m_eNowSceneNum, m_vecParticleDesc[2]);
				GetSingle(CUtilityMgr)->Create_ParticleObject(m_eNowSceneNum, m_vecParticleDesc[4]);
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerTornado),
					&_float4(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS), 1));
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerTornado),
					&_float4(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS), 1));
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerTornado),
					&_float4(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS), 1));
				particleTimeInterver = 0;
			}
			m_LevitationTime -= fDeltaTime*0.8;
			//fGravity = _float((m_LevitationTime) * -2.4f);

		}

		if (m_LevitationTime == fDeltaTime)
			fGravity = _float(-2.94f);
		else
			fGravity = _float((m_LevitationTime) * (m_LevitationTime)* -29.4f);
	}

	_uint iNowAnimIndex = m_pModel->Get_NowAnimIndex();
	if (m_LevitationTime != fDeltaTime && fGravity < 0 && !m_bIsAttached && iNowAnimIndex != 42 && iNowAnimIndex != 43)
		m_pModel->Change_AnimIndex(17);

	m_pTransformCom->MovetoDir_bySpeed(XMVectorSet(0, 1.f, 0, 0), fGravity, fDeltaTime);
	return S_OK;
}

HRESULT CPlayer::Attack_Update_Horse(_double fDeltaTime, CGameInstance * pInstance)
{

	_uint iNowAnimIndex = m_pModel->Get_NowAnimIndex();
	_double PlayRate = m_pModel->Get_PlayRate();

	if ((PlayRate < 0.8 && PlayRate > 0.5)|| !m_iAttackCount)
	{
		if (pInstance->Get_DIMouseButtonState(CInput_Device::MBS_LBUTTON) & DIS_Down)
			m_bIsAttackClicked = true;
	}
	if (PlayRate < 0.1 && iNowAnimIndex >= Weapon_Horse + 8 && iNowAnimIndex <= Weapon_Horse + 14 )
	{
		_int PressedChecker[4];
		ZeroMemory(PressedChecker, sizeof(_bool) * 4);

		PressedChecker[0] = _int(pInstance->Get_DIKeyState(DIK_W) & DIS_Press);
		PressedChecker[1] = _int(pInstance->Get_DIKeyState(DIK_S) & DIS_Press);
		PressedChecker[2] = _int(pInstance->Get_DIKeyState(DIK_A) & DIS_Press);
		PressedChecker[3] = _int(pInstance->Get_DIKeyState(DIK_D) & DIS_Press);

		if (PressedChecker[0] || PressedChecker[1] || PressedChecker[2] || PressedChecker[3])
		{
			//m_pTransformCom->LookDir(XMVectorSetY(m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), 0)
			//	- XMVectorSetY(m_pMainCamera->Get_Camera_Transform()->Get_MatrixState(CTransform::STATE_POS), 0));

			_Vector forword = XMVector3Normalize(XMVectorSetY(m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), 0)
				- XMVectorSetY(m_pMainCamera->Get_Camera_Transform()->Get_MatrixState(CTransform::STATE_POS), 0));

			_Vector right = XMVector3Cross(XMVectorSet(0, 1, 0, 0), forword);

			_Vector Dir = XMVectorSet(0, 0, 0, 0);


			if (PressedChecker[0]) Dir += forword;
			if (PressedChecker[1]) Dir -= forword;
			if (PressedChecker[2]) Dir -= right;
			if (PressedChecker[3]) Dir += right;

			Dir = XMVector3Normalize(Dir);
			_Vector vOldLook = m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK);

			if (XMVectorGetX(XMVector3Length(Dir + vOldLook)) < 0.1f)
				m_pTransformCom->LookDir(m_pTransformCom->Get_MatrixState(CTransform::STATE_RIGHT) + vOldLook);
			else
				m_pTransformCom->LookDir((Dir *0.25 + vOldLook * 0.75));

		}
	}


	if (m_bIsAttackClicked && m_iAttackCount < 4)
	{

		if (iNowAnimIndex >= Weapon_Horse)
			//if (iNowAnimIndex >= Weapon_Knife + 3)
		{
			if (iNowAnimIndex < Weapon_Horse + 8)
			{
				m_pModel->Change_AnimIndex(Weapon_Horse + 8, 0.15, true);
				m_iAttackCount++;
				m_bIsAttackClicked = false;
				ZeroMemory(m_bAtkMoveMentChecker, sizeof(_bool) * 3);

			}
			else if (iNowAnimIndex >= Weapon_Horse + 8 && iNowAnimIndex <= Weapon_Horse + 14 && iNowAnimIndex % 2 == Weapon_Horse % 2)
			{


				if (PlayRate > 0.8)
				{
					m_pModel->Change_AnimIndex(Weapon_Horse + 8 + m_iAttackCount * 2, 0.15, true);

					ZeroMemory(m_bAtkMoveMentChecker, sizeof(_bool) * 3);

					m_iAttackCount++;
					if (m_iAttackCount > 5) m_iAttackCount = 5;
					m_bIsAttackClicked = false;
				}

			}

		}
	}
	else
	{


		if (m_iAttackCount && iNowAnimIndex % 2 == Weapon_Horse % 2)
			//if (iNowAnimIndex >= Weapon_Knife + 8 && iNowAnimIndex <= Weapon_Knife + 16 && iNowAnimIndex % 2 == Weapon_Knife % 2)
		{
			if (PlayRate > 0.8)
			{
				m_pModel->Change_AnimIndex_ReturnTo(Weapon_Horse + 8 + (m_iAttackCount - 1) * 2 + 1, Weapon_Horse + 3, 0.15, true);
				m_iAttackCount = 0;
				m_bIsAttackClicked = false;
				ZeroMemory(m_bAtkMoveMentChecker, sizeof(_bool) * 3);

			}

		}


	}

	PlayRate = m_pModel->Get_PlayRate();

	switch (m_iAttackCount)
	{
	case 1:
		if (PlayRate > 0.45)
		{
			if (!m_bAtkMoveMentChecker[0])
			{
				m_bAtkMoveMentChecker[0] = true;
				Add_Force(m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK), 10);

			}

			m_vecWeapon[m_iWeaponModelIndex]->Set_AttackAble(true);
		}
		else
		{
			m_vecWeapon[m_iWeaponModelIndex]->Set_AttackAble(false);
		}


		break;
	case 2:
		if (PlayRate > 0.43)
		{
			if (!m_bAtkMoveMentChecker[0])
			{
				m_bAtkMoveMentChecker[0] = true;
				Add_Force(m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK), 10);

			}

			m_vecWeapon[m_iWeaponModelIndex]->Set_AttackAble(true);
		}
		else
		{
			m_vecWeapon[m_iWeaponModelIndex]->Set_AttackAble(false);
		}
		break;
	case 3:
		if (PlayRate > 0.33)
		{
			if (!m_bAtkMoveMentChecker[0])
			{
				m_bAtkMoveMentChecker[0] = true;
				Add_Force(m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK), 10);		

			}

			m_vecWeapon[m_iWeaponModelIndex]->Set_AttackAble(true);
		}
		else
		{
			m_vecWeapon[m_iWeaponModelIndex]->Set_AttackAble(false);
		}
		break;
	case 4:
		if (PlayRate > 0.06)
		{
			if (!m_bAtkMoveMentChecker[0])
			{
				m_bAtkMoveMentChecker[0] = true;
				Add_Force(m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK), 30);
			}

			if (PlayRate > 0.5)
				m_vecWeapon[m_iWeaponModelIndex]->Set_AttackAble(true);
		}
		else
		{
			m_vecWeapon[m_iWeaponModelIndex]->Set_AttackAble(false);
		}

		break;

	default:
		m_vecWeapon[m_iWeaponModelIndex]->Set_AttackAble(false);
		break;
	}

	if (iNowAnimIndex >= Weapon_Horse + 8 && iNowAnimIndex <= Weapon_Horse + 14)
	{
		((CHorse*)(m_vecWeapon[2]))->Set_TrailOn(true);
	}
	else
	{
		((CHorse*)(m_vecWeapon[2]))->Set_TrailOn(false);
	}

	//if (g_pGameInstance->Get_DIKeyState(DIK_Z) &DIS_Down)
	//	m_vecWeapon[m_iWeaponModelIndex]->Get_WeaponModel()->Change_AnimIndex_ReturnTo_Must(1, 0, 0, true);

	return S_OK;
}

HRESULT CPlayer::Move_Update_Teapot(_double fDeltaTime, CGameInstance * pInstance)
{
	if (m_bIsZoom && !m_fDashPassedTime)
	{
		_Vector NewLook = XMVector3Normalize(XMVectorSetY(m_pMainCamera->Get_Camera_Transform()->Get_MatrixState(CTransform::STATE_LOOK) * 0.75
			+ m_pMainCamera->Get_Camera_Transform()->Get_MatrixState(CTransform::STATE_RIGHT) * 0.25, 0));

		m_pTransformCom->LookDir(NewLook);



		if (!m_pModel->Get_IsHavetoBlockAnimChange())
		{

			_int PressedChecker[4];
			ZeroMemory(PressedChecker, sizeof(_bool) * 4);

			PressedChecker[0] = _int(pInstance->Get_DIKeyState(DIK_W) & DIS_Press);
			PressedChecker[1] = _int(pInstance->Get_DIKeyState(DIK_S) & DIS_Press);
			PressedChecker[2] = _int(pInstance->Get_DIKeyState(DIK_A) & DIS_Press);
			PressedChecker[3] = _int(pInstance->Get_DIKeyState(DIK_D) & DIS_Press);

			if (PressedChecker[0] || PressedChecker[1] || PressedChecker[2] || PressedChecker[3])
			{



				_Vector forword = XMVector3Normalize(XMVectorSetY(m_pMainCamera->Get_Camera_Transform()->Get_MatrixState(CTransform::STATE_LOOK), 0));


				m_pTransformCom->LookDir(forword);

				_float MoveSpeed = 1;
				//_float MoveSpeed = _float(-0.45 * m_fCharedGauge + 0.6);

				if (m_fCharedGauge > 0.4f)
					MoveSpeed = 0.15f;
				else MoveSpeed = 0.4f;


				if (PressedChecker[0])
					m_pTransformCom->Move_Forward(fDeltaTime * MoveSpeed);

				else if (PressedChecker[1])
					m_pTransformCom->Move_Backward(fDeltaTime * MoveSpeed);


				else if (PressedChecker[2])
					m_pTransformCom->Move_Left(fDeltaTime * MoveSpeed);

				else
					m_pTransformCom->Move_Right(fDeltaTime * MoveSpeed);


				m_pTransformCom->LookDir(NewLook);



				if (!m_LevitationTime)
				{
					if (PressedChecker[0])
					{
						if (m_fCharedGauge < 0.4f)
							m_pModel->Change_AnimIndex(Weapon_Teapot + 4);
						else
							m_pModel->Change_AnimIndex(Weapon_Teapot + 12);


					}
					else if (PressedChecker[1])
					{
						if (m_fCharedGauge < 0.4f)
							m_pModel->Change_AnimIndex(Weapon_Teapot + 5);
						else
							m_pModel->Change_AnimIndex(Weapon_Teapot + 13);

					}

					else if (PressedChecker[2])
					{
						if (m_fCharedGauge < 0.4f)
							m_pModel->Change_AnimIndex(Weapon_Teapot + 6);
						else
							m_pModel->Change_AnimIndex(Weapon_Teapot + 14);

					}

					else  if (PressedChecker[3])
					{
						if (m_fCharedGauge < 0.4f)
							m_pModel->Change_AnimIndex(Weapon_Teapot + 7);
						else
							m_pModel->Change_AnimIndex(Weapon_Teapot + 15);

					}
				}

			}
			else {




				if (!m_LevitationTime)
					m_pModel->Change_AnimIndex(Weapon_Teapot + 3);


			}
		}

	}
	else {


		if (!m_pModel->Get_IsHavetoBlockAnimChange())
		{

			_int PressedChecker[4];
			ZeroMemory(PressedChecker, sizeof(_bool) * 4);

			PressedChecker[0] = _int(pInstance->Get_DIKeyState(DIK_W) & DIS_Press);
			PressedChecker[1] = _int(pInstance->Get_DIKeyState(DIK_S) & DIS_Press);
			PressedChecker[2] = _int(pInstance->Get_DIKeyState(DIK_A) & DIS_Press);
			PressedChecker[3] = _int(pInstance->Get_DIKeyState(DIK_D) & DIS_Press);

			if (PressedChecker[0] || PressedChecker[1] || PressedChecker[2] || PressedChecker[3])
			{
				//m_pTransformCom->LookDir(XMVectorSetY(m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), 0)
				//	- XMVectorSetY(m_pMainCamera->Get_Camera_Transform()->Get_MatrixState(CTransform::STATE_POS), 0));

				_Vector forword = XMVector3Normalize(XMVectorSetY(m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), 0)
					- XMVectorSetY(m_pMainCamera->Get_Camera_Transform()->Get_MatrixState(CTransform::STATE_POS), 0));

				_Vector right = XMVector3Cross(XMVectorSet(0, 1, 0, 0), forword);

				_Vector Dir = XMVectorSet(0, 0, 0, 0);


				if (PressedChecker[0]) Dir += forword;
				if (PressedChecker[1]) Dir -= forword;
				if (PressedChecker[2]) Dir -= right;
				if (PressedChecker[3]) Dir += right;

				Dir = XMVector3Normalize(Dir);
				_Vector vOldLook = m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK);

				if (XMVectorGetX(XMVector3Length(Dir + vOldLook)) < 0.1f)
					m_pTransformCom->LookDir(m_pTransformCom->Get_MatrixState(CTransform::STATE_RIGHT) + vOldLook);
				else
					m_pTransformCom->LookDir((Dir *0.25 + vOldLook * 0.75));




				if (m_fDashPassedTime)
				{
					m_vDashDir = Dir;
				}
				else
				{

					m_pTransformCom->MovetoDir(XMVector3Normalize(Dir), fDeltaTime);


					if (!m_LevitationTime)
						m_pModel->Change_AnimIndex(Weapon_Teapot + 2);


				}


			}
			else {
				if (!m_LevitationTime && !m_fDashPassedTime)
					m_pModel->Change_AnimIndex(Weapon_Teapot + 0);


			}
		}
	}
	return S_OK;
}

HRESULT CPlayer::Jump_Update_Teapot(_double fDeltaTime, CGameInstance * pInstance)
{
	if (!m_pModel->Get_IsHavetoBlockAnimChange())
	{
		if (m_iJumpCount < 3 && !m_fDashPassedTime && pInstance->Get_DIKeyState(DIK_SPACE) & DIS_Down)
		{
			Add_JumpForce(PlayerMaxJumpPower * m_fSmallScale);

			_uint iJumpIndex = (m_iJumpCount) ? 1 : 0;
			m_pModel->Change_AnimIndex_UntilTo(15 + iJumpIndex * 5, 17 + iJumpIndex * 5, 0.08);

			if (m_iJumpCount )
			{
				GetSingle(CUtilityMgr)->Create_ParticleObject(m_eNowSceneNum, m_vecParticleDesc[2]);
				GetSingle(CUtilityMgr)->Create_ParticleObject(m_eNowSceneNum, m_vecParticleDesc[4]);
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerTornado),
					&_float4(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS), 1));
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerTornado),
					&_float4(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS), 1));
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerTornado),
					&_float4(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS), 1));
			}

			m_iJumpCount++;

		}
	}
	m_LevitationTime += fDeltaTime;
	_float fGravity = 0;
	if (m_fJumpPower > 0)
	{
		m_fJumpPower = _float(m_fMaxJumpPower * (m_LevitationTime - 1.f)* (m_LevitationTime - 1.f));
		fGravity = m_fJumpPower;

		if (m_fJumpPower <= 1.0f)
		{
			m_fJumpPower = 0;
			m_LevitationTime = fDeltaTime;
		}
	}
	else
	{
		if (m_iJumpCount && pInstance->Get_DIKeyState(DIK_SPACE) & DIS_Press)
		{
			static _double particleTimeInterver = 0;
			particleTimeInterver += fDeltaTime;

			if (particleTimeInterver > 0.2)
			{

				GetSingle(CUtilityMgr)->Create_ParticleObject(m_eNowSceneNum, m_vecParticleDesc[2]);
				GetSingle(CUtilityMgr)->Create_ParticleObject(m_eNowSceneNum, m_vecParticleDesc[4]);
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerTornado),
					&_float4(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS), 1));
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerTornado),
					&_float4(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS), 1));
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerTornado),
					&_float4(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS), 1));
				particleTimeInterver = 0;
			}
			m_LevitationTime -= fDeltaTime*0.8;
			//fGravity = _float((m_LevitationTime) * -2.4f);

		}
		if (m_pModel->Get_NowAnimIndex() == Weapon_Teapot + 8 || m_pModel->Get_NowAnimIndex() == Weapon_Teapot + 16)
		{
			fGravity = _float(-8.82f);
		}
		else
		{
			if (m_LevitationTime == fDeltaTime)
				fGravity = _float(-2.94f);
			else
				fGravity = _float((m_LevitationTime) * (m_LevitationTime)* -29.4f);
		}
	}

	_uint iNowAnimIndex = m_pModel->Get_NowAnimIndex();
	if (m_LevitationTime != fDeltaTime && fGravity < 0 && !m_bIsAttached && iNowAnimIndex != 42 && iNowAnimIndex != 43)
		m_pModel->Change_AnimIndex(17);

	m_pTransformCom->MovetoDir_bySpeed(XMVectorSet(0, 1.f, 0, 0), fGravity, fDeltaTime);
	return S_OK;
}

HRESULT CPlayer::Attack_Update_Teapot(_double fDeltaTime, CGameInstance * pInstance)
{

	if (pInstance->Get_DIMouseButtonState(CInput_Device::MBS_RBUTTON) & DIS_DoubleDown)
	{
		m_bIsZoom = !m_bIsZoom;

		if (!m_bIsZoom)
		{
			m_pModel->Set_BlockAnim(false);
			m_fCharedGauge = 0;

		}

		m_bIsCoolTime = false;
	}


	if (m_bIsZoom)
	{

		if (m_bIsCoolTime)
		{


			if (m_pModel->Get_NowAnimIndex() == Weapon_Teapot + 8)
			{
				if (!m_bAtkMoveMentChecker[0] && m_pModel->Get_PlayRate() > 0.4)
				{
					m_bAtkMoveMentChecker[0] = true;
					Add_Force(m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK) * -1, 10);

					CTransform* pCamTransform = m_pMainCamera->Get_Camera_Transform();
					_float3 vBulletDir = XMVector3Normalize(XMVector3TransformNormal(
						XMVectorSet(0, 0, 10, 0) - m_vBulletFirePos.Multiply_Matrix_AsPosVector(pCamTransform->Get_InverseWorldMatrix()),
						pCamTransform->Get_WorldMatrix()));

					FAILED_CHECK(pInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Bullet), TAG_OP(Prototype_Bullet_Grenade),&vBulletDir));

				}

			}
			else if (m_pModel->Get_NowAnimIndex() == Weapon_Teapot + 16)
			{
				if (!m_bAtkMoveMentChecker[0] && m_pModel->Get_PlayRate() > 0.1)
				{
					m_bAtkMoveMentChecker[0] = true;

					CTransform* pCamTransform = m_pMainCamera->Get_Camera_Transform();
					_float3 vBulletDir = XMVector3Normalize(XMVector3TransformNormal(
						XMVectorSet(0, 0, 10, 0) - m_vBulletFirePos.Multiply_Matrix_AsPosVector(pCamTransform->Get_InverseWorldMatrix()),
						pCamTransform->Get_WorldMatrix()));
					FAILED_CHECK(pInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Bullet), TAG_OP(Prototype_Bullet_Grenade),&vBulletDir));


					Add_Force(m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK) * -1, 25);
				}

			}
			else
			{
				m_fCharedGauge -= _float(fDeltaTime * 0.33f);
				if (m_fCharedGauge < 0)
				{
					m_fCharedGauge = 0;
					m_bIsCoolTime = false;

					ZeroMemory(m_bAtkMoveMentChecker, sizeof(_bool) * 3);

				}
			}



		}
		else
		{


			if (pInstance->Get_DIMouseButtonState(CInput_Device::MBS_RBUTTON) & DIS_Press)
			{
				m_fCharedGauge += _float(fDeltaTime * 0.33f);
				if (m_fCharedGauge > 1)m_fCharedGauge = 1;


				if (m_fCharedGauge < 0.4f)
				{
					m_vecWeapon[m_iWeaponModelIndex]->Get_WeaponModel()->Change_AnimIndex_ReturnTo_Must(1, 0, 0.15, true);


					m_pModel->Change_AnimIndex(Weapon_Teapot + 9, 0.15, true);
				}
				else
				{

					m_vecWeapon[m_iWeaponModelIndex]->Get_WeaponModel()->Change_AnimIndex_ReturnTo(1, 2, 0.15, true);
					m_pModel->Change_AnimIndex(Weapon_Teapot + 11, 0.15, true);
				}

			}
			else
			{
				if (m_pModel->Get_NowAnimIndex() == Weapon_Teapot + 9)
					m_pModel->Change_AnimIndex(Weapon_Teapot + 3);

				else if (m_pModel->Get_NowAnimIndex() == Weapon_Teapot + 11)
					m_pModel->Change_AnimIndex(Weapon_Teapot + 10);

			}


			FAILED_CHECK(Lunch_Grenade(fDeltaTime, pInstance));

		}
	}




	return S_OK;
}

HRESULT CPlayer::Lunch_Grenade(_double fDeltaTime, CGameInstance * pInstance)
{

	if (!m_bIsCoolTime && pInstance->Get_DIMouseButtonState(CInput_Device::MBS_LBUTTON) & DIS_Down)
	{


		if (m_fCharedGauge < 0.4f)
		{
			m_pModel->Change_AnimIndex_ReturnTo_Must(Weapon_Teapot + 8, Weapon_Teapot + 3,0.15, true);

			m_vecWeapon[m_iWeaponModelIndex]->Get_WeaponModel()->Change_AnimIndex_ReturnTo_Must(3, 0, 0.15, true);
			//쿨링 타임 들어가는거로 해주자
			
		}
		else
		{
			m_pModel->Change_AnimIndex_ReturnTo_Must(Weapon_Teapot + 16, Weapon_Teapot + 3, 0.15, true);
			m_vecWeapon[m_iWeaponModelIndex]->Get_WeaponModel()->Change_AnimIndex_ReturnTo_Must(4, 0, 0.15, true);


		
		}

		m_bIsCoolTime = true;

	}
	
	return S_OK;
}

HRESULT CPlayer::Move_Update_Umbrella(_double fDeltaTime, CGameInstance * pInstance)
{
	_Vector NewLook = XMVector3Normalize(XMVectorSetY(m_pMainCamera->Get_Camera_Transform()->Get_MatrixState(CTransform::STATE_LOOK) * 0.75
		+ m_pMainCamera->Get_Camera_Transform()->Get_MatrixState(CTransform::STATE_RIGHT) * 0.25, 0));

	m_pTransformCom->LookDir(NewLook);


	if (!m_pModel->Get_IsHavetoBlockAnimChange())
	{
		if (!m_LevitationTime && !m_bUmbrellaReflected)
			m_pModel->Change_AnimIndex_UntilTo(Weapon_Umbrella + 1, Weapon_Umbrella + 1);

	}
	return S_OK;
}

HRESULT CPlayer::Jump_Update_Umbrella(_double fDeltaTime, CGameInstance * pInstance)
{
	
	m_LevitationTime += fDeltaTime;
	_float fGravity = 0;
	if (m_fJumpPower > 0)
	{
		m_fJumpPower = _float(m_fMaxJumpPower * (m_LevitationTime - 1.f)* (m_LevitationTime - 1.f));
		fGravity = m_fJumpPower;

		if (m_fJumpPower <= 1.0f)
		{
			m_fJumpPower = 0;
			m_LevitationTime = fDeltaTime;
		}
	}
	else
	{
		if (m_iJumpCount && pInstance->Get_DIKeyState(DIK_SPACE) & DIS_Press)
		{
			static _double particleTimeInterver = 0;
			particleTimeInterver += fDeltaTime;

			if (particleTimeInterver > 0.2)
			{

				GetSingle(CUtilityMgr)->Create_ParticleObject(m_eNowSceneNum, m_vecParticleDesc[2]);
				GetSingle(CUtilityMgr)->Create_ParticleObject(m_eNowSceneNum, m_vecParticleDesc[4]);
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerTornado),
					&_float4(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS), 1));
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerTornado),
					&_float4(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS), 1));
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerTornado),
					&_float4(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS), 1));
				particleTimeInterver = 0;
			}
			m_LevitationTime -= fDeltaTime*0.8;
			//fGravity = _float((m_LevitationTime) * -2.4f);

		}

		if (m_LevitationTime == fDeltaTime)
			fGravity = _float(-2.94f);
		else
			fGravity = _float((m_LevitationTime) * (m_LevitationTime)* -29.4f);
	}

	_uint iNowAnimIndex = m_pModel->Get_NowAnimIndex();
	if (m_LevitationTime != fDeltaTime && fGravity < 0 && !m_bIsAttached && iNowAnimIndex != 42 && iNowAnimIndex != 43)
		m_pModel->Change_AnimIndex(17);

	m_pTransformCom->MovetoDir_bySpeed(XMVectorSet(0, 1.f, 0, 0), fGravity, fDeltaTime);

	return S_OK;
}

HRESULT CPlayer::Attack_Update_Umbrella(_double fDeltaTime, CGameInstance * pInstance)
{

	if (m_fUmbrellaIntro >= 1)
	{
		if (m_bUmbrellaReflected)
		{
			m_ReflectPassedTime += fDeltaTime;


			if (m_ReflectPassedTime > 2)
			{
				m_vecWeapon[m_iWeaponModelIndex]->Set_AttackAble(true);
			}

		}

	}


	return S_OK;
}

HRESULT CPlayer::Giantting_Update(_double fDeltaTime, CGameInstance * pInstance)
{
	static bool IsFadeOuted = false;

	if (m_bGettingBigger)
	{
		m_GiantingPassedTime += _float(fDeltaTime);
		m_fSmallScale = pInstance->Easing(TYPE_QuarticOut, 1.f, PlayerGiantSize, (_float)m_GiantingPassedTime, 3.0f);

		if (!IsFadeOuted && m_GiantingPassedTime > 2.f)
		{
			IsFadeOuted = true;
			GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_FadeOutIn, 1, _float4(0, 0, 0, 1));
		}

		if (m_GiantingPassedTime > 3.f)
		{
			m_pModel->Change_AnimIndex(Weapon_Giant + 0, 0.35f);

			m_fSmallScale = PlayerGiantSize;
			m_bGettingBigger = false;
		}

		m_pTransformCom->Scaled_All(_float3(m_fSmallScale));
		m_pTransformCom->Set_MoveSpeed(PlayerMoveSpeed *1.5f);
	}
	else
	{
		m_GiantingPassedTime += _float(fDeltaTime);
		m_fSmallScale = pInstance->Easing(TYPE_QuarticOut, PlayerGiantSize, 1.f, (_float)m_GiantingPassedTime, 3.0f);

		if (IsFadeOuted && m_GiantingPassedTime > 2.f)
		{
			IsFadeOuted = false;
			GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_FadeOutIn, 1, _float4(0, 0, 0, 1));
		}

		if (m_GiantingPassedTime > 3.f)
		{
			m_pModel->Change_AnimIndex(0, 0.35f);

			m_fSmallScale = 1.f;
			m_bIsGiant = false;
			m_bGettingBigger = true;
		}

		m_pTransformCom->Scaled_All(_float3(m_fSmallScale));
		m_pTransformCom->Set_MoveSpeed(PlayerMoveSpeed);
	}

	return S_OK;
}

HRESULT CPlayer::Move_Update_Giant(_double fDeltaTime, CGameInstance * pInstance)
{
	if (!m_pModel->Get_IsHavetoBlockAnimChange())
	{

		_int PressedChecker[4];
		ZeroMemory(PressedChecker, sizeof(_bool) * 4);

		PressedChecker[0] = _int(pInstance->Get_DIKeyState(DIK_W) & DIS_Press);
		PressedChecker[1] = _int(pInstance->Get_DIKeyState(DIK_S) & DIS_Press);
		PressedChecker[2] = _int(pInstance->Get_DIKeyState(DIK_A) & DIS_Press);
		PressedChecker[3] = _int(pInstance->Get_DIKeyState(DIK_D) & DIS_Press);

		if (PressedChecker[0] || PressedChecker[1] || PressedChecker[2] || PressedChecker[3])
		{
			//m_pTransformCom->LookDir(XMVectorSetY(m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), 0)
			//	- XMVectorSetY(m_pMainCamera->Get_Camera_Transform()->Get_MatrixState(CTransform::STATE_POS), 0));

			_Vector forword = XMVector3Normalize(XMVectorSetY(m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), 0)
				- XMVectorSetY(m_pMainCamera->Get_Camera_Transform()->Get_MatrixState(CTransform::STATE_POS), 0));

			_Vector right = XMVector3Cross(XMVectorSet(0, 1, 0, 0), forword);

			_Vector Dir = XMVectorSet(0, 0, 0, 0);

			/*
			//if (PressedChecker[0]) Dir += m_pTransformCom->Get_MatrixState_Normalized(CTransform::STATE_LOOK);
			//if (PressedChecker[1]) Dir -= m_pTransformCom->Get_MatrixState_Normalized(CTransform::STATE_LOOK);
			//if (PressedChecker[2]) Dir -= m_pTransformCom->Get_MatrixState_Normalized(CTransform::STATE_RIGHT);
			//if (PressedChecker[3]) Dir += m_pTransformCom->Get_MatrixState_Normalized(CTransform::STATE_RIGHT);
			*/

			if (PressedChecker[0] && !m_bSlide) Dir += forword;
			if (PressedChecker[1] && !m_bSlide) Dir -= forword;
			if (PressedChecker[2]) Dir -= right;
			if (PressedChecker[3]) Dir += right;

			Dir = XMVector3Normalize(Dir);
			_Vector vOldLook = m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK);

			if (XMVectorGetX(XMVector3Length(Dir + vOldLook)) < 0.1f)
				m_pTransformCom->LookDir(m_pTransformCom->Get_MatrixState(CTransform::STATE_RIGHT) + vOldLook);
			else
				m_pTransformCom->LookDir((Dir *0.85f + vOldLook * 0.15f));

			if (m_fDashPassedTime)
			{
				m_vDashDir = Dir;
			}
			else
			{
				m_pTransformCom->MovetoDir(XMVector3Normalize(Dir), fDeltaTime);


				if (!m_LevitationTime && !m_bSlide)
					m_pModel->Change_AnimIndex(Weapon_Giant + 1, 0.35f);

			}


		}
		else {
			if (!m_LevitationTime && !m_fDashPassedTime && !m_bSlide)
				m_pModel->Change_AnimIndex(Weapon_Giant + 0, 0.35f);



		}
	}

	static _uint m_iOldAnimIndex = 0;
	static _uint m_iAdjMovedIndex = 0;

	_uint iNowAnimIndex = m_pModel->Get_NowAnimIndex();
	_double PlayRate = m_pModel->Get_PlayRate();

	if (iNowAnimIndex != m_iOldAnimIndex || PlayRate > 0.95)
		m_iAdjMovedIndex = 0;



	if (PlayRate <= 0.95)
	{
		switch (iNowAnimIndex)
		{
		case Weapon_Giant + 1:
			if (m_iAdjMovedIndex == 0 && PlayRate > 0.29333)
			{
				GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_CamShaking, 0.2f, _float4(3.0f));


				CDustWind::DUSTWINDDESC tDesc;

				tDesc.vPosition = (m_pTransformCom->Get_MatrixState(CTransform::STATE_POS) 
					+ XMVector3Normalize(m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK)) * 7 + XMVector3Normalize(m_pTransformCom->Get_MatrixState(CTransform::STATE_RIGHT)) * -5);
				tDesc.ToTalLifeTime = 0.4f;
				tDesc.StartScale = _float3(3, 10, 3);
				tDesc.TargetScale = _float3(7, 0.5f, 7);
				tDesc.eEasingType = TYPE_Linear;
				tDesc.bNotAttackPlayer = true;
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet), TAG_OP(Prototype_DustWind), &(tDesc));
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet), TAG_OP(Prototype_DustWind), &(tDesc));
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet), TAG_OP(Prototype_DustWind), &(tDesc));

				m_iAdjMovedIndex++;
			}
			if (m_iAdjMovedIndex == 1 && PlayRate > 0.8)
			{
				GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_CamShaking, 0.2f, _float4(3.0f));

				CDustWind::DUSTWINDDESC tDesc;

				tDesc.vPosition = (m_pTransformCom->Get_MatrixState(CTransform::STATE_POS)
					+ XMVector3Normalize(m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK)) * 7 + XMVector3Normalize(m_pTransformCom->Get_MatrixState(CTransform::STATE_RIGHT)) * 5);
				tDesc.ToTalLifeTime = 0.4f;
				tDesc.StartScale = _float3(3, 10, 3);
				tDesc.TargetScale = _float3(7, 0.5f, 7);
				tDesc.eEasingType = TYPE_Linear;
				tDesc.bNotAttackPlayer = true;
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet), TAG_OP(Prototype_DustWind), &(tDesc));
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet), TAG_OP(Prototype_DustWind), &(tDesc));
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet), TAG_OP(Prototype_DustWind), &(tDesc));

				m_iAdjMovedIndex++;
			}

			break;

		case Weapon_Giant + 3:
			if (m_iAdjMovedIndex == 0 && PlayRate > 0.37272727)
			{
				GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_CamShaking, 0.4f, _float4(6.f));


				CDustWind::DUSTWINDDESC tDesc;

				tDesc.vPosition = (m_pTransformCom->Get_MatrixState(CTransform::STATE_POS)
					+ XMVector3Normalize(m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK)) * 7 + XMVector3Normalize(m_pTransformCom->Get_MatrixState(CTransform::STATE_RIGHT)) * -5);
				tDesc.ToTalLifeTime = 1.5f;
				tDesc.StartScale = _float3(5, 10, 5);
				tDesc.TargetScale = _float3(7, 0.5f, 7);
				tDesc.eEasingType = TYPE_ElasticIn;
				tDesc.bNotAttackPlayer = true;
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet), TAG_OP(Prototype_DustWind), &(tDesc));
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet), TAG_OP(Prototype_DustWind), &(tDesc));
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet), TAG_OP(Prototype_DustWind), &(tDesc));

				CGameInstance* pInstance = g_pGameInstance;
				CUtilityMgr* pUtil = GetSingle(CUtilityMgr);
				_Vector vTranslation = XMVectorSetW(m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), 0);

				for (_uint i = 0; i < 30; i++)
				{
					if (rand() % 2)
					{
						_float4 TargetPos = tDesc.vPosition.XMVector() + XMVectorSet(pUtil->RandomFloat(-3, 3), pUtil->RandomFloat(1, 6), pUtil->RandomFloat(-3, 3), _float(Prototype_Mesh_Gazebo_Piece01 + (rand() % 4)));
						pInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_BreakablePiece), TAG_OP(Prototype_BreakablePiece), &TargetPos);

					}
					else
					{
						_float4 TargetPos = tDesc.vPosition.XMVector() + XMVectorSet(pUtil->RandomFloat(-3, 3), pUtil->RandomFloat(1, 6), pUtil->RandomFloat(-3, 3), _float(Prototype_QBattleTowerParticleA + (rand() % 2)));
						pInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_BreakablePiece), TAG_OP(Prototype_BreakablePiece), &TargetPos);

					}
				}

				m_iAdjMovedIndex++;
			}
		

			break;

			
		default:
			break;
		}
	}

	m_iOldAnimIndex = iNowAnimIndex;


	return S_OK;
}

HRESULT CPlayer::Jump_Update_Giant(_double fDeltaTime, CGameInstance * pInstance)
{
	
	m_LevitationTime += fDeltaTime;
	_float fGravity = 0;
	if (m_fJumpPower > 0)
	{
		m_fJumpPower = _float(m_fMaxJumpPower * (m_LevitationTime - 1.f)* (m_LevitationTime - 1.f));
		fGravity = m_fJumpPower;

		if (m_fJumpPower <= 1.0f)
		{
			m_fJumpPower = 0;
			m_LevitationTime = fDeltaTime;
		}
	}
	else
	{
		if (m_LevitationTime == fDeltaTime)
			fGravity = _float(-2.94f);
		else
			fGravity = _float((m_LevitationTime) * (m_LevitationTime)* -29.4f);



	}

	m_pTransformCom->MovetoDir_bySpeed(XMVectorSet(0, 1.f, 0, 0), fGravity, fDeltaTime);

	return S_OK;
}
HRESULT CPlayer::Attack_Update_Giant(_double fDeltaTime, CGameInstance * pInstance)
{
	static _uint GiantAttack = 0;



	if (pInstance->Get_DIMouseButtonState(CInput_Device::MBS_LBUTTON) & DIS_Down)
	{
		
		m_pModel->Change_AnimIndex_ReturnTo_Must(Weapon_Giant + 3, 0, 0.35, true);
	}




	return S_OK;
}

HRESULT CPlayer::Move_Update_Knife(_double fDeltaTime, CGameInstance * pInstance)
{
	if (!m_pModel->Get_IsHavetoBlockAnimChange())
	{

		_int PressedChecker[4];
		ZeroMemory(PressedChecker, sizeof(_bool) * 4);

		PressedChecker[0] = _int(pInstance->Get_DIKeyState(DIK_W) & DIS_Press);
		PressedChecker[1] = _int(pInstance->Get_DIKeyState(DIK_S) & DIS_Press);
		PressedChecker[2] = _int(pInstance->Get_DIKeyState(DIK_A) & DIS_Press);
		PressedChecker[3] = _int(pInstance->Get_DIKeyState(DIK_D) & DIS_Press);

		if (PressedChecker[0] || PressedChecker[1] || PressedChecker[2] || PressedChecker[3])
		{
			//m_pTransformCom->LookDir(XMVectorSetY(m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), 0)
			//	- XMVectorSetY(m_pMainCamera->Get_Camera_Transform()->Get_MatrixState(CTransform::STATE_POS), 0));

			_Vector forword = XMVector3Normalize(XMVectorSetY(m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), 0)
				- XMVectorSetY(m_pMainCamera->Get_Camera_Transform()->Get_MatrixState(CTransform::STATE_POS), 0));

			_Vector right = XMVector3Cross(XMVectorSet(0, 1, 0, 0), forword);

			_Vector Dir = XMVectorSet(0, 0, 0, 0);


			if (PressedChecker[0]) Dir += forword;
			if (PressedChecker[1]) Dir -= forword;
			if (PressedChecker[2]) Dir -= right;
			if (PressedChecker[3]) Dir += right;

			Dir = XMVector3Normalize(Dir);
			_Vector vOldLook = m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK);

			if (XMVectorGetX(XMVector3Length(Dir + vOldLook)) < 0.1f)
				m_pTransformCom->LookDir(m_pTransformCom->Get_MatrixState(CTransform::STATE_RIGHT) + vOldLook);
			else
				m_pTransformCom->LookDir((Dir *0.25 + vOldLook * 0.75));




			if (m_fDashPassedTime)
			{
				m_vDashDir = Dir;
			}
			else
			{
				m_pTransformCom->MovetoDir(XMVector3Normalize(Dir), fDeltaTime);


				if (!m_LevitationTime)
					m_pModel->Change_AnimIndex(Weapon_Knife + 2);
			}



		}
		else {
			if (!m_LevitationTime && !m_fDashPassedTime)
				m_pModel->Change_AnimIndex(Weapon_Knife + 0);


		}
	}
	return S_OK;
}

HRESULT CPlayer::Jump_Update_Knife(_double fDeltaTime, CGameInstance * pInstance)
{
	if (!m_pModel->Get_IsHavetoBlockAnimChange())
	{
		if (m_iJumpCount < 3 && !m_fDashPassedTime && pInstance->Get_DIKeyState(DIK_SPACE) & DIS_Down)
		{
			Add_JumpForce(PlayerMaxJumpPower * m_fSmallScale);

			_uint iJumpIndex = (m_iJumpCount) ? 1 : 0;
			m_pModel->Change_AnimIndex_UntilTo(15 + iJumpIndex * 5, 17 + iJumpIndex * 5, 0.08);


			if (m_iJumpCount )
			{
				GetSingle(CUtilityMgr)->Create_ParticleObject(m_eNowSceneNum, m_vecParticleDesc[2]);
				GetSingle(CUtilityMgr)->Create_ParticleObject(m_eNowSceneNum, m_vecParticleDesc[4]);
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerTornado),
					&_float4(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS), 1));
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerTornado),
					&_float4(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS), 1));
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerTornado),
					&_float4(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS), 1));
			}
			m_iJumpCount++;

		}
	}
	m_LevitationTime += fDeltaTime;
	_float fGravity = 0;
	if (m_fJumpPower > 0)
	{
		m_fJumpPower = _float(m_fMaxJumpPower * (m_LevitationTime - 1.f)* (m_LevitationTime - 1.f));
		fGravity = m_fJumpPower;

		if (m_fJumpPower <= 1.0f)
		{
			m_fJumpPower = 0;
			m_LevitationTime = fDeltaTime;
		}
	}
	else
	{
		if (m_iJumpCount && pInstance->Get_DIKeyState(DIK_SPACE) & DIS_Press)
		{
			static _double particleTimeInterver = 0;
			particleTimeInterver += fDeltaTime;

			if (particleTimeInterver > 0.2)
			{

				GetSingle(CUtilityMgr)->Create_ParticleObject(m_eNowSceneNum, m_vecParticleDesc[2]);
				GetSingle(CUtilityMgr)->Create_ParticleObject(m_eNowSceneNum, m_vecParticleDesc[4]);
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerTornado),
					&_float4(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS), 1));
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerTornado),
					&_float4(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS), 1));
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerTornado),
					&_float4(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS), 1));
				particleTimeInterver = 0;
			}
			m_LevitationTime -= fDeltaTime*0.8;
			//fGravity = _float((m_LevitationTime) * -2.4f);

		}

		if (m_LevitationTime == fDeltaTime)
			fGravity = _float(-2.94f);
		else
			fGravity = _float((m_LevitationTime) * (m_LevitationTime)* -29.4f);
	}

	_uint iNowAnimIndex = m_pModel->Get_NowAnimIndex();
	if (m_LevitationTime != fDeltaTime && fGravity < 0 && !m_bIsAttached && iNowAnimIndex != 42 && iNowAnimIndex != 43)
		m_pModel->Change_AnimIndex(17);

	m_pTransformCom->MovetoDir_bySpeed(XMVectorSet(0, 1.f, 0, 0), fGravity, fDeltaTime);
	return S_OK;
}

HRESULT CPlayer::Attack_Update_Knife(_double fDeltaTime, CGameInstance * pInstance)
{

	//_bool				m_bIsAttackClicked = false;
	//_uint				m_iAttackCount = 0;


	

	_uint iNowAnimIndex = m_pModel->Get_NowAnimIndex();
	_double PlayRate = m_pModel->Get_PlayRate();

	if ((PlayRate < 0.8 && PlayRate > 0.5) || !m_iAttackCount)
	{
		if (pInstance->Get_DIMouseButtonState(CInput_Device::MBS_LBUTTON) & DIS_Down)
			m_bIsAttackClicked = true;


	}

	if (PlayRate < 0.1 && iNowAnimIndex >= Weapon_Knife + 8 && iNowAnimIndex <= Weapon_Knife + 16)
	{
		_int PressedChecker[4];
		ZeroMemory(PressedChecker, sizeof(_bool) * 4);

		PressedChecker[0] = _int(pInstance->Get_DIKeyState(DIK_W) & DIS_Press);
		PressedChecker[1] = _int(pInstance->Get_DIKeyState(DIK_S) & DIS_Press);
		PressedChecker[2] = _int(pInstance->Get_DIKeyState(DIK_A) & DIS_Press);
		PressedChecker[3] = _int(pInstance->Get_DIKeyState(DIK_D) & DIS_Press);

		if (PressedChecker[0] || PressedChecker[1] || PressedChecker[2] || PressedChecker[3])
		{
			//m_pTransformCom->LookDir(XMVectorSetY(m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), 0)
			//	- XMVectorSetY(m_pMainCamera->Get_Camera_Transform()->Get_MatrixState(CTransform::STATE_POS), 0));

			_Vector forword = XMVector3Normalize(XMVectorSetY(m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), 0)
				- XMVectorSetY(m_pMainCamera->Get_Camera_Transform()->Get_MatrixState(CTransform::STATE_POS), 0));

			_Vector right = XMVector3Cross(XMVectorSet(0, 1, 0, 0), forword);

			_Vector Dir = XMVectorSet(0, 0, 0, 0);


			if (PressedChecker[0]) Dir += forword;
			if (PressedChecker[1]) Dir -= forword;
			if (PressedChecker[2]) Dir -= right;
			if (PressedChecker[3]) Dir += right;

			Dir = XMVector3Normalize(Dir);
			_Vector vOldLook = m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK);

			if (XMVectorGetX(XMVector3Length(Dir + vOldLook)) < 0.1f)
				m_pTransformCom->LookDir(m_pTransformCom->Get_MatrixState(CTransform::STATE_RIGHT) + vOldLook);
			else
				m_pTransformCom->LookDir((Dir *0.25 + vOldLook * 0.75));

		}
	}


	if (m_bIsAttackClicked && m_iAttackCount < 5)
	{

		if (iNowAnimIndex >= Weapon_Knife)
		//if (iNowAnimIndex >= Weapon_Knife + 3)
		{
			if (iNowAnimIndex < Weapon_Knife + 8)
			{
				m_pModel->Change_AnimIndex(Weapon_Knife + 8, 0.15, true);
				m_iAttackCount++;
				m_bIsAttackClicked = false;
				ZeroMemory(m_bAtkMoveMentChecker, sizeof(_bool) * 3);

			}
			else if (iNowAnimIndex >= Weapon_Knife + 8 && iNowAnimIndex <= Weapon_Knife + 16 && iNowAnimIndex % 2 == Weapon_Knife % 2)
			{

			
				if (PlayRate > 0.8)
				{
					m_pModel->Change_AnimIndex(Weapon_Knife + 8 + m_iAttackCount * 2, 0.15, true);

					ZeroMemory(m_bAtkMoveMentChecker, sizeof(_bool) * 3);

					m_iAttackCount++;
					if (m_iAttackCount > 5) m_iAttackCount = 5;
					m_bIsAttackClicked = false;
				}

			}



	



		}
	}
	else
	{


		if (m_iAttackCount && iNowAnimIndex % 2 == Weapon_Knife % 2)
		//if (iNowAnimIndex >= Weapon_Knife + 8 && iNowAnimIndex <= Weapon_Knife + 16 && iNowAnimIndex % 2 == Weapon_Knife % 2)
		{
			if (PlayRate > 0.8)
			{
				m_pModel->Change_AnimIndex_ReturnTo(Weapon_Knife + 8 + (m_iAttackCount - 1) * 2 + 1, Weapon_Knife + 3, 0.15, true);
				m_iAttackCount = 0;
				m_bIsAttackClicked = false;
				ZeroMemory(m_bAtkMoveMentChecker, sizeof(_bool) * 3);

			}

		}


	}

	PlayRate = m_pModel->Get_PlayRate();

	switch (m_iAttackCount)
	{
	case 1:
		if (PlayRate > 0.1)
		{
			if (!m_bAtkMoveMentChecker[0])
			{
				m_bAtkMoveMentChecker[0] = true;
				Add_Force(m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK), 10);
			}

			if (PlayRate > 0.4)
				m_vecWeapon[m_iWeaponModelIndex]->Set_AttackAble(true);
		}
		else
		{
			m_vecWeapon[m_iWeaponModelIndex]->Set_AttackAble(false);
		}

		break;
	case 2:
		if (PlayRate > 0.4)
		{
			m_vecWeapon[m_iWeaponModelIndex]->Set_AttackAble(true);
		}
		else
		{
			m_vecWeapon[m_iWeaponModelIndex]->Set_AttackAble(false);
		}

		break;


	case 3:

		if (PlayRate > 0.6)
		{
			if (!m_bAtkMoveMentChecker[0])
			{
				m_bAtkMoveMentChecker[0] = true;
				Add_Force(m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK), 10);
			}
			m_vecWeapon[m_iWeaponModelIndex]->Set_AttackAble(true);
		}
		else
		{
			m_vecWeapon[m_iWeaponModelIndex]->Set_AttackAble(false);
		}

		break;
	case 4:

		if (PlayRate > 0.15)
		{
			if (!m_bAtkMoveMentChecker[0])
			{
				m_bAtkMoveMentChecker[0] = true;
				Add_Force(m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK), 10);
			}
			m_vecWeapon[m_iWeaponModelIndex]->Set_AttackAble(true);
		}
		else
		{
			m_vecWeapon[m_iWeaponModelIndex]->Set_AttackAble(false);
		}


		break;
	case 5:

		if (PlayRate > 0.28)
		{
			if (!m_bAtkMoveMentChecker[0])
			{
				m_bAtkMoveMentChecker[0] = true;
				Add_Force(m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK), 10);
			}
			m_vecWeapon[m_iWeaponModelIndex]->Set_AttackAble(true);
		}
		else
		{
			m_vecWeapon[m_iWeaponModelIndex]->Set_AttackAble(false);
		}
		break;


	default:

		m_vecWeapon[m_iWeaponModelIndex]->Set_AttackAble(false);

		break;
	}

	if (iNowAnimIndex >= Weapon_Knife + 8 && iNowAnimIndex <= Weapon_Knife + 16)
	{
		((CKnife*)(m_vecWeapon[0]))->Set_TrailOn(true);
	}
	else
	{

		((CKnife*)(m_vecWeapon[0]))->Set_TrailOn(false);
	}


	return S_OK;
}

HRESULT CPlayer::Move_Update_Grinder(_double fDeltaTime, CGameInstance * pInstance)
{

	if (m_bIsZoom && !m_fDashPassedTime)
	{
		_Vector NewLook = XMVector3Normalize(XMVectorSetY(m_pMainCamera->Get_Camera_Transform()->Get_MatrixState(CTransform::STATE_LOOK) * 0.75
			+ m_pMainCamera->Get_Camera_Transform()->Get_MatrixState(CTransform::STATE_RIGHT) * 0.25, 0));

		m_pTransformCom->LookDir(NewLook);



		if (!m_pModel->Get_IsHavetoBlockAnimChange())
		{

			_int PressedChecker[4];
			ZeroMemory(PressedChecker, sizeof(_bool) * 4);

			PressedChecker[0] = _int(pInstance->Get_DIKeyState(DIK_W) & DIS_Press);
			PressedChecker[1] = _int(pInstance->Get_DIKeyState(DIK_S) & DIS_Press);
			PressedChecker[2] = _int(pInstance->Get_DIKeyState(DIK_A) & DIS_Press);
			PressedChecker[3] = _int(pInstance->Get_DIKeyState(DIK_D) & DIS_Press);

			if (PressedChecker[0] || PressedChecker[1] || PressedChecker[2] || PressedChecker[3])
			{




				_Vector forword = XMVector3Normalize(XMVectorSetY(m_pMainCamera->Get_Camera_Transform()->Get_MatrixState(CTransform::STATE_LOOK), 0));


				m_pTransformCom->LookDir(forword);



				if (PressedChecker[0])
					m_pTransformCom->Move_Forward(fDeltaTime);

				else if (PressedChecker[1])										
					m_pTransformCom->Move_Backward(fDeltaTime);


				else if (PressedChecker[2])
					m_pTransformCom->Move_Left(fDeltaTime);

				else
					m_pTransformCom->Move_Right(fDeltaTime);


				m_pTransformCom->LookDir(NewLook);



				if (!m_LevitationTime)
				{
					if (PressedChecker[0])
					{
						m_pModel->Change_AnimIndex(Weapon_Grinder + 4);

					}
					else if (PressedChecker[1])
					{
						m_pModel->Change_AnimIndex(Weapon_Grinder + 5);

					}

					else if (PressedChecker[2])
					{
						m_pModel->Change_AnimIndex(Weapon_Grinder + 6);

					}

					else 
					{
						m_pModel->Change_AnimIndex(Weapon_Grinder + 7);

					}
				}

			}
			else {




				if (!m_LevitationTime)
					m_pModel->Change_AnimIndex(Weapon_Grinder + 3);


			}
		}

	}
	else {


		if (!m_pModel->Get_IsHavetoBlockAnimChange())
		{

			_int PressedChecker[4];
			ZeroMemory(PressedChecker, sizeof(_bool) * 4);

			PressedChecker[0] = _int(pInstance->Get_DIKeyState(DIK_W) & DIS_Press);
			PressedChecker[1] = _int(pInstance->Get_DIKeyState(DIK_S) & DIS_Press);
			PressedChecker[2] = _int(pInstance->Get_DIKeyState(DIK_A) & DIS_Press);
			PressedChecker[3] = _int(pInstance->Get_DIKeyState(DIK_D) & DIS_Press);

			if (PressedChecker[0] || PressedChecker[1] || PressedChecker[2] || PressedChecker[3])
			{
				//m_pTransformCom->LookDir(XMVectorSetY(m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), 0)
				//	- XMVectorSetY(m_pMainCamera->Get_Camera_Transform()->Get_MatrixState(CTransform::STATE_POS), 0));

				_Vector forword = XMVector3Normalize(XMVectorSetY(m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), 0)
					- XMVectorSetY(m_pMainCamera->Get_Camera_Transform()->Get_MatrixState(CTransform::STATE_POS), 0));

				_Vector right = XMVector3Cross(XMVectorSet(0, 1, 0, 0), forword);

				_Vector Dir = XMVectorSet(0, 0, 0, 0);


				if (PressedChecker[0]) Dir += forword;
				if (PressedChecker[1]) Dir -= forword;
				if (PressedChecker[2]) Dir -= right;
				if (PressedChecker[3]) Dir += right;

				Dir = XMVector3Normalize(Dir);
				_Vector vOldLook = m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK);

				if (XMVectorGetX(XMVector3Length(Dir + vOldLook)) < 0.1f)
					m_pTransformCom->LookDir(m_pTransformCom->Get_MatrixState(CTransform::STATE_RIGHT) + vOldLook);
				else
					m_pTransformCom->LookDir((Dir *0.25 + vOldLook * 0.75));



				if (m_fDashPassedTime)
				{
					m_vDashDir = Dir;
				}
				else
				{

					m_pTransformCom->MovetoDir(XMVector3Normalize(Dir), fDeltaTime);


					if (!m_LevitationTime)
						m_pModel->Change_AnimIndex(Weapon_Grinder + 2);

				}



			}
			else {
				if (!m_LevitationTime && !m_fDashPassedTime)
					m_pModel->Change_AnimIndex(Weapon_Grinder + 0);


			}
		}
	}
	return S_OK;
}

HRESULT CPlayer::Jump_Update_Grinder(_double fDeltaTime, CGameInstance * pInstance)
{
	if (!m_pModel->Get_IsHavetoBlockAnimChange())
	{
		if (m_iJumpCount < 3 && !m_fDashPassedTime && pInstance->Get_DIKeyState(DIK_SPACE) & DIS_Down)
		{
			Add_JumpForce(PlayerMaxJumpPower * m_fSmallScale);
			_uint iJumpIndex = (m_iJumpCount) ? 1 : 0;

			m_pModel->Change_AnimIndex_UntilTo(15 + iJumpIndex * 5, 17 + iJumpIndex * 5, 0.08);

			if (m_iJumpCount)
			{
				GetSingle(CUtilityMgr)->Create_ParticleObject(m_eNowSceneNum, m_vecParticleDesc[2]);
				GetSingle(CUtilityMgr)->Create_ParticleObject(m_eNowSceneNum, m_vecParticleDesc[4]);
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerTornado),
					&_float4(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS), 1));
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerTornado),
					&_float4(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS), 1));
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerTornado),
					&_float4(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS), 1));
			}

			m_iJumpCount++;

		}
	}
	m_LevitationTime += fDeltaTime;
	_float fGravity = 0;
	if (m_fJumpPower > 0)
	{
		m_fJumpPower = _float(m_fMaxJumpPower * (m_LevitationTime - 1.f)* (m_LevitationTime - 1.f));
		fGravity = m_fJumpPower;

		if (m_fJumpPower <= 1.0f)
		{
			m_fJumpPower = 0;
			m_LevitationTime = fDeltaTime;
		}
	}
	else
	{
		if (m_iJumpCount && pInstance->Get_DIKeyState(DIK_SPACE) & DIS_Press)
		{
			static _double particleTimeInterver = 0;
			particleTimeInterver += fDeltaTime;

			if (particleTimeInterver > 0.2)
			{

				GetSingle(CUtilityMgr)->Create_ParticleObject(m_eNowSceneNum, m_vecParticleDesc[2]);
				GetSingle(CUtilityMgr)->Create_ParticleObject(m_eNowSceneNum, m_vecParticleDesc[4]);
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerTornado),
					&_float4(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS), 1));
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerTornado),
					&_float4(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS), 1));
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerTornado),
					&_float4(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS), 1));
				particleTimeInterver = 0;
			}
			m_LevitationTime -= fDeltaTime*0.8;
			//fGravity = _float((m_LevitationTime) * -2.4f);

		}

		if (m_LevitationTime == fDeltaTime)
			fGravity = _float(-2.94f);
		else
			fGravity = _float((m_LevitationTime) * (m_LevitationTime)* -29.4f);
	}

	_uint iNowAnimIndex = m_pModel->Get_NowAnimIndex();
	if (m_LevitationTime != fDeltaTime && fGravity < 0 && !m_bIsAttached && iNowAnimIndex != 42 && iNowAnimIndex != 43)
		m_pModel->Change_AnimIndex(17);

	m_pTransformCom->MovetoDir_bySpeed(XMVectorSet(0, 1.f, 0, 0), fGravity, fDeltaTime);
	return S_OK;
}

HRESULT CPlayer::Attack_Update_Grinder(_double fDeltaTime, CGameInstance * pInstance)
{

	if (pInstance->Get_DIMouseButtonState(CInput_Device::MBS_RBUTTON) & DIS_Press)
	{
		m_bIsZoom = true;
		m_fCharedGauge = 1;


		FAILED_CHECK(Lunch_Bullet(fDeltaTime, pInstance));


		if (pInstance->Get_DIMouseButtonState(CInput_Device::MBS_RBUTTON) & DIS_Up)
			m_pModel->Set_BlockAnim(false);
	}
	else
	{
		m_bIsZoom = false;
		m_fCharedGauge = 0;
		
	}


	return S_OK;
}

HRESULT CPlayer::Set_Player_On_Terrain()
{
	CGameInstance* pInstance = GetSingle(CGameInstance);
	
	CTerrain* pTerrain =(CTerrain*)(pInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Terrain)));

	_bool bIsOn = false;
	_uint eNowTile = Tile_End;


	_float3 CaculatedPos = pTerrain->PutOnTerrain(&bIsOn, m_pTransformCom->Get_MatrixState(CTransform::STATE_POS),m_vOldPos.XMVector(),nullptr,&eNowTile);


	if (eNowTile == Tile_None)
	{
		m_iJumpCount = 0;
		m_LevitationTime = 0;
		m_fJumpPower = -1.f;


		m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, CaculatedPos);
	}
	else if (bIsOn)
	{
		if (m_LevitationTime > g_fDeltaTime)
		{
			if (m_LevitationTime >= 0.9f)
			{
				if (!m_bIsAttached)
				{
					m_pModel->Change_AnimIndex_ReturnTo(19, 0, 0, true);
					Add_Force(m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK), 10);
				}
			}


			ZeroMemory(m_bAtkMoveMentChecker, sizeof(_bool) * 3);
			m_bIsAttackClicked = false;
			m_iAttackCount = 0;
		}
		

		m_iJumpCount = 0;
		m_LevitationTime = 0;
		m_fJumpPower = -1.f;
		m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, CaculatedPos);
	}

	return S_OK;
}

HRESULT CPlayer::Set_Player_On_Terrain_IgnoreTile()
{

	CGameInstance* pInstance = GetSingle(CGameInstance);

	CTerrain* pTerrain = (CTerrain*)(pInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Terrain)));

	_bool bIsOn = false;
	_uint eNowTile = Tile_End;


	_float3 CaculatedPos = pTerrain->PutOnTerrain_IgnoreTile(&bIsOn, m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), m_vOldPos.XMVector(), nullptr, &eNowTile);


	if (bIsOn)
	{
		if (m_LevitationTime > g_fDeltaTime)
		{
			if (m_LevitationTime >= 0.9f)
			{
				if (!m_bIsAttached)
				{
					m_pModel->Change_AnimIndex_ReturnTo(19, 0, 0, true);
					Add_Force(m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK), 10);
				}
			}


			ZeroMemory(m_bAtkMoveMentChecker, sizeof(_bool) * 3);
			m_bIsAttackClicked = false;
			m_iAttackCount = 0;
		}


		m_iJumpCount = 0;
		m_LevitationTime = 0;
		m_fJumpPower = -1.f;
		m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, CaculatedPos);
	}

	return S_OK;
}

HRESULT CPlayer::Set_Player_On_Terrain_DontPutonJumpMovable()
{
	CGameInstance* pInstance = GetSingle(CGameInstance);

	CTerrain* pTerrain = (CTerrain*)(pInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Terrain)));

	_bool bIsOn = false;

	_uint eNowTile = Tile_End;
	_float3 CaculatedPos = pTerrain->PutOnTerrain_Stage2(&bIsOn, m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), m_vOldPos.XMVector(), nullptr, &eNowTile);



	if (eNowTile == Tile_JumpMovable)
		return S_OK;


	if (eNowTile == Tile_None)
	{
		//if (bIsOn)
		//{
		m_iJumpCount = 0;
		m_LevitationTime = 0;
		m_fJumpPower = -1.f;
		//}

		m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, CaculatedPos);
	}
	else if (bIsOn)
	{
		if (m_LevitationTime > g_fDeltaTime)
		{
			if (m_LevitationTime < 0.9f)
			{

				//_uint iJumpIndex = (m_iJumpCount) ? 1: 0;
				//m_pModel->Change_AnimIndex(0, 0.15,true);
				////m_pModel->Change_AnimIndex(18 + m_iJumpCount * 5, 0,0.15);
				//if (pInstance->Get_DIKeyState(DIK_SPACE) & DIS_Press)
				//	m_pModel->Change_AnimIndex_ReturnTo(18 + iJumpIndex * 5, 0, g_fDeltaTime, true);
				//else
				//	m_pModel->Change_AnimIndex_ReturnTo(18 + iJumpIndex * 5, 0, g_fDeltaTime,true);

			}
			else
			{
				if (!m_bIsAttached)
				{
					m_pModel->Change_AnimIndex_ReturnTo(19, 0, 0, true);
					Add_Force(m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK), 10);
				}
			}


			ZeroMemory(m_bAtkMoveMentChecker, sizeof(_bool) * 3);
			m_bIsAttackClicked = false;
			m_iAttackCount = 0;
		}


		m_iJumpCount = 0;
		m_LevitationTime = 0;
		m_fJumpPower = -1.f;
		m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, CaculatedPos);
	}


	return S_OK;
}

HRESULT CPlayer::Set_Player_On_Slieder(_double fDeltatime)
{
	CGameInstance* pInstance = GetSingle(CGameInstance);
	CTerrain* pTerrain = (CTerrain*)(pInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Terrain)));

	_bool bIsOn = false;
	_float3 vPlaneSlideDir;
	_uint	eTile = Tile_None;

	_float3 CaculatedPos = pTerrain->PutOnTerrain(&bIsOn, m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), m_vOldPos.XMVector(),&vPlaneSlideDir, &eTile);

	if (bIsOn)
	{
		m_LevitationTime = 0;
		m_fJumpPower = -1.f;
		m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, CaculatedPos);

		_Matrix matScale = m_pTransformCom->Get_MatrixScale_All();
		_Matrix NewMat = m_pTransformCom->Get_WorldMatrix();

		//_Vector OldLook = m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK);
		//_Vector NewLook = vPlaneSlideDir.Get_Nomalize();
		//m_pTransformCom->LookDir(XMVector3Normalize(XMVectorLerp(OldLook, NewLook, 0.15f)));
		

		NewMat.r[1] = vPlaneSlideDir.XMVector();


		if (XMVector3Equal(_float3(0, 1, 0).XMVector(), NewMat.r[1]))
		{
#ifdef _DEBUG
			OutputDebugString(TEXT("Error: Cross With Same Vector (Player)\n"));
#endif // _DEBUG


			NewMat.r[0] = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.000001f, 1, 0, 0), NewMat.r[1]));
		}
		else {

			NewMat.r[0] = XMVector3Normalize(XMVector3Cross(XMVectorSet(0, 1, 0, 0), NewMat.r[1]));

		}

		NewMat.r[2] = XMVector3Normalize(XMVector3Cross(NewMat.r[0], NewMat.r[1]));

		_Matrix NowWorld = m_pTransformCom->Get_WorldMatrix();

		for (_uint i = 0; i < 2; i++)
			NewMat.r[i] = XMVector3Normalize(XMVectorLerp(NowWorld.r[i], NewMat.r[i], 0.15f)) * matScale.r[i];


		m_pTransformCom->Set_Matrix(NewMat);


		m_pTransformCom->Move_Forward(fDeltatime * 1.5);
	}
	return S_OK;
}

HRESULT CPlayer::Set_Camera_On_Player(_double fDeltaTime)
{
	if (m_pMainCamera->Get_IsCamAction())return S_FALSE;


	CGameInstance* pInstance = GetSingle(CGameInstance);


#define CamLerpLate 0.2f

	if (m_bIsGiant)
	{
		if (m_GiantingPassedTime < 3.0)
		{
			_long		MouseMove = 0;
			if (MouseMove = pInstance->Get_DIMouseMoveState(CInput_Device::MMS_X))
			{
				m_CamDegreeAngle.y += _float(fDeltaTime) * 20 * MouseMove * 0.1f;

			}

			if (MouseMove = pInstance->Get_DIMouseMoveState(CInput_Device::MMS_Y))
			{
				m_CamDegreeAngle.x += _float(fDeltaTime) * 20 * MouseMove * 0.1f;

				if (m_CamDegreeAngle.x < -20.f)m_CamDegreeAngle.x = -20.f;
				else if (m_CamDegreeAngle.x > 60.f)m_CamDegreeAngle.x = 60.f;
			}


			CTransform* pCamTransform = m_pMainCamera->Get_Camera_Transform();
			_Matrix OldCamMat = pCamTransform->Get_WorldMatrix();
			_float3 PlayerPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
			_Matrix NewCamMatrix = XMMatrixTranslation(0, 1.5f * m_fSmallScale, m_CamDegreeAngle.z
				* m_fSmallScale * (1 - m_fDashPower / PlayerMaxDashPower * 0.1f))
				* XMMatrixRotationX(XMConvertToRadians(m_CamDegreeAngle.x))
				* XMMatrixRotationY(XMConvertToRadians(m_CamDegreeAngle.y))
				* XMMatrixTranslation(PlayerPos.x, PlayerPos.y, PlayerPos.z);


			NewCamMatrix.r[0] = XMVectorLerp(OldCamMat.r[0], NewCamMatrix.r[0], 0.5f);
			NewCamMatrix.r[1] = XMVectorLerp(OldCamMat.r[1], NewCamMatrix.r[1], 0.5f);
			NewCamMatrix.r[2] = XMVectorLerp(OldCamMat.r[2], NewCamMatrix.r[2], 0.5f);
			NewCamMatrix.r[3] = XMVectorLerp(OldCamMat.r[3], NewCamMatrix.r[3], 0.5f);

			pCamTransform->Set_Matrix(NewCamMatrix);

			FAILED_CHECK(m_pMainCamera->Set_ViewMatrix());
			FAILED_CHECK(m_pMainCamera->Set_ProjectMatrix());

		}
		else
		{
			_long		MouseMove = 0;
			if (MouseMove = pInstance->Get_DIMouseMoveState(CInput_Device::MMS_X))
			{
				m_CamDegreeAngle.y += _float(fDeltaTime) * 20 * MouseMove * 0.1f;

			}

			if (MouseMove = pInstance->Get_DIMouseMoveState(CInput_Device::MMS_Y))
			{
				m_CamDegreeAngle.x += _float(fDeltaTime) * 20 * MouseMove * 0.1f;

				if (m_CamDegreeAngle.x < -20.f)m_CamDegreeAngle.x = -20.f;
				else if (m_CamDegreeAngle.x > 60.f)m_CamDegreeAngle.x = 60.f;
			}


			CTransform* pCamTransform = m_pMainCamera->Get_Camera_Transform();
			_Matrix OldCamMat = pCamTransform->Get_WorldMatrix();
			_float3 PlayerPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
			_Matrix NewCamMatrix = XMMatrixTranslation(0, 1.5f * m_fSmallScale, m_CamDegreeAngle.z
				* m_fSmallScale * (1 - m_fDashPower / PlayerMaxDashPower * 0.1f))
				* XMMatrixRotationX(XMConvertToRadians(m_CamDegreeAngle.x))
				* XMMatrixRotationY(XMConvertToRadians(m_CamDegreeAngle.y))
				* XMMatrixTranslation(PlayerPos.x, PlayerPos.y, PlayerPos.z);


			NewCamMatrix.r[0] = XMVectorLerp(OldCamMat.r[0], NewCamMatrix.r[0], CamLerpLate);
			NewCamMatrix.r[1] = XMVectorLerp(OldCamMat.r[1], NewCamMatrix.r[1], CamLerpLate);
			NewCamMatrix.r[2] = XMVectorLerp(OldCamMat.r[2], NewCamMatrix.r[2], CamLerpLate);
			NewCamMatrix.r[3] = XMVectorLerp(OldCamMat.r[3], NewCamMatrix.r[3], CamLerpLate);

			pCamTransform->Set_Matrix(NewCamMatrix);

			FAILED_CHECK(m_pMainCamera->Set_ViewMatrix());
			FAILED_CHECK(m_pMainCamera->Set_ProjectMatrix());


		}


	}
	else if (m_eNowWeapon == CPlayer::Weapon_Grinder)
	{
		if (m_bIsZoom)
		{
			_long		MouseMove = 0;
			if (MouseMove = pInstance->Get_DIMouseMoveState(CInput_Device::MMS_X))
			{
				m_CamDegreeAngle.y += _float(fDeltaTime) * 20 * MouseMove * 0.1f;

			}

			if (MouseMove = pInstance->Get_DIMouseMoveState(CInput_Device::MMS_Y))
			{
				m_CamDegreeAngle.x += _float(fDeltaTime) * 20 * MouseMove * 0.1f;

				if (m_CamDegreeAngle.x < -20.f)m_CamDegreeAngle.x = -20.f;
				else if (m_CamDegreeAngle.x > 60.f)m_CamDegreeAngle.x = 60.f;
			}

			CTransform* pCamTransform = m_pMainCamera->Get_Camera_Transform();
			_Matrix OldCamMat = pCamTransform->Get_WorldMatrix();
			_float3 PlayerPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
			_Matrix	NewCamMatrix = XMMatrixTranslation(0.7f, 1.7f, m_CamDegreeAngle.z * _float(-0.7 * m_fCharedGauge + 1)
				* m_fSmallScale * (1 - m_fDashPower / PlayerMaxDashPower * 0.1f))
				* XMMatrixRotationX(XMConvertToRadians(m_CamDegreeAngle.x))
				* XMMatrixRotationY(XMConvertToRadians(m_CamDegreeAngle.y))
				* XMMatrixTranslation(PlayerPos.x, PlayerPos.y, PlayerPos.z);

			NewCamMatrix.r[0] = XMVectorLerp(OldCamMat.r[0], NewCamMatrix.r[0], CamLerpLate);
			NewCamMatrix.r[1] = XMVectorLerp(OldCamMat.r[1], NewCamMatrix.r[1], CamLerpLate);
			NewCamMatrix.r[2] = XMVectorLerp(OldCamMat.r[2], NewCamMatrix.r[2], CamLerpLate);
			NewCamMatrix.r[3] = XMVectorLerp(OldCamMat.r[3], NewCamMatrix.r[3], CamLerpLate);

			pCamTransform->Set_Matrix(NewCamMatrix);
		}
		else {

			_long		MouseMove = 0;
			if (MouseMove = pInstance->Get_DIMouseMoveState(CInput_Device::MMS_X))
			{
				m_CamDegreeAngle.y += _float(fDeltaTime) * 20 * MouseMove * 0.1f;

			}

			if (MouseMove = pInstance->Get_DIMouseMoveState(CInput_Device::MMS_Y))
			{
				m_CamDegreeAngle.x += _float(fDeltaTime) * 20 * MouseMove * 0.1f;

				if (m_CamDegreeAngle.x < -20.f)m_CamDegreeAngle.x = -20.f;
				else if (m_CamDegreeAngle.x > 60.f)m_CamDegreeAngle.x = 60.f;
			}

			CTransform* pCamTransform = m_pMainCamera->Get_Camera_Transform();
			_Matrix OldCamMat = pCamTransform->Get_WorldMatrix();
			_float3 PlayerPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
			_Matrix	NewCamMatrix = XMMatrixTranslation(0, 1.5f, m_CamDegreeAngle.z
				* m_fSmallScale * (1 - m_fDashPower / PlayerMaxDashPower * 0.1f))
				* XMMatrixRotationX(XMConvertToRadians(m_CamDegreeAngle.x))
				* XMMatrixRotationY(XMConvertToRadians(m_CamDegreeAngle.y))
				* XMMatrixTranslation(PlayerPos.x, PlayerPos.y, PlayerPos.z);




			NewCamMatrix.r[0] = XMVectorLerp(OldCamMat.r[0], NewCamMatrix.r[0], CamLerpLate);
			NewCamMatrix.r[1] = XMVectorLerp(OldCamMat.r[1], NewCamMatrix.r[1], CamLerpLate);
			NewCamMatrix.r[2] = XMVectorLerp(OldCamMat.r[2], NewCamMatrix.r[2], CamLerpLate);
			NewCamMatrix.r[3] = XMVectorLerp(OldCamMat.r[3], NewCamMatrix.r[3], CamLerpLate);

			pCamTransform->Set_Matrix(NewCamMatrix);
		}
		FAILED_CHECK(m_pMainCamera->Set_ViewMatrix());
		FAILED_CHECK(m_pMainCamera->Set_ProjectMatrix());


	}
	else if (m_eNowWeapon == CPlayer::Weapon_Teapot)
	{
		if (m_bIsZoom)
		{
			_long		MouseMove = 0;
			if (MouseMove = pInstance->Get_DIMouseMoveState(CInput_Device::MMS_X))
			{
				m_CamDegreeAngle.y += _float(fDeltaTime) * 20 * MouseMove * 0.1f;

			}

			if (MouseMove = pInstance->Get_DIMouseMoveState(CInput_Device::MMS_Y))
			{
				m_CamDegreeAngle.x += _float(fDeltaTime) * 20 * MouseMove * 0.1f;

				if (m_CamDegreeAngle.x < -20.f)m_CamDegreeAngle.x = -20.f;
				else if (m_CamDegreeAngle.x > 60.f)m_CamDegreeAngle.x = 60.f;
			}



			CTransform* pCamTransform = m_pMainCamera->Get_Camera_Transform();
			_Matrix OldCamMat = pCamTransform->Get_WorldMatrix();
			_float3 PlayerPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
			_Matrix	NewCamMatrix = XMMatrixTranslation(0.7f, 1.7f, m_CamDegreeAngle.z * _float(-0.7 * m_fCharedGauge + 1)
					* m_fSmallScale * (1 - m_fDashPower / PlayerMaxDashPower * 0.1f))
					* XMMatrixRotationX(XMConvertToRadians(m_CamDegreeAngle.x))
					* XMMatrixRotationY(XMConvertToRadians(m_CamDegreeAngle.y))
					* XMMatrixTranslation(PlayerPos.x, PlayerPos.y, PlayerPos.z);



			NewCamMatrix.r[0] = XMVectorLerp(OldCamMat.r[0], NewCamMatrix.r[0], CamLerpLate);
			NewCamMatrix.r[1] = XMVectorLerp(OldCamMat.r[1], NewCamMatrix.r[1], CamLerpLate);
			NewCamMatrix.r[2] = XMVectorLerp(OldCamMat.r[2], NewCamMatrix.r[2], CamLerpLate);
			NewCamMatrix.r[3] = XMVectorLerp(OldCamMat.r[3], NewCamMatrix.r[3], CamLerpLate);

			pCamTransform->Set_Matrix(NewCamMatrix);
		}
		else {

			_long		MouseMove = 0;
			if (MouseMove = pInstance->Get_DIMouseMoveState(CInput_Device::MMS_X))
			{
				m_CamDegreeAngle.y += _float(fDeltaTime) * 20 * MouseMove * 0.1f;

			}

			if (MouseMove = pInstance->Get_DIMouseMoveState(CInput_Device::MMS_Y))
			{
				m_CamDegreeAngle.x += _float(fDeltaTime) * 20 * MouseMove * 0.1f;

				if (m_CamDegreeAngle.x < -20.f)m_CamDegreeAngle.x = -20.f;
				else if (m_CamDegreeAngle.x > 60.f)m_CamDegreeAngle.x = 60.f;
			}



			CTransform* pCamTransform = m_pMainCamera->Get_Camera_Transform();
			_Matrix OldCamMat = pCamTransform->Get_WorldMatrix();
			_float3 PlayerPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
			_Matrix	NewCamMatrix = XMMatrixTranslation(0, 1.5f, m_CamDegreeAngle.z
				* m_fSmallScale * (1 - m_fDashPower / PlayerMaxDashPower * 0.1f))
				* XMMatrixRotationX(XMConvertToRadians(m_CamDegreeAngle.x))
				* XMMatrixRotationY(XMConvertToRadians(m_CamDegreeAngle.y))
				* XMMatrixTranslation(PlayerPos.x, PlayerPos.y, PlayerPos.z);



			NewCamMatrix.r[0] = XMVectorLerp(OldCamMat.r[0], NewCamMatrix.r[0], CamLerpLate);
			NewCamMatrix.r[1] = XMVectorLerp(OldCamMat.r[1], NewCamMatrix.r[1], CamLerpLate);
			NewCamMatrix.r[2] = XMVectorLerp(OldCamMat.r[2], NewCamMatrix.r[2], CamLerpLate);
			NewCamMatrix.r[3] = XMVectorLerp(OldCamMat.r[3], NewCamMatrix.r[3], CamLerpLate);

			pCamTransform->Set_Matrix(NewCamMatrix);
		}
		FAILED_CHECK(m_pMainCamera->Set_ViewMatrix());
		FAILED_CHECK(m_pMainCamera->Set_ProjectMatrix());


	}
	else if (m_eNowWeapon == CPlayer::Weapon_Umbrella)
	{
		_long		MouseMove = 0;
		if (MouseMove = pInstance->Get_DIMouseMoveState(CInput_Device::MMS_X))
		{
			m_CamDegreeAngle.y += _float(fDeltaTime) * 20 * MouseMove * 0.1f;

		}

		if (MouseMove = pInstance->Get_DIMouseMoveState(CInput_Device::MMS_Y))
		{
			m_CamDegreeAngle.x += _float(fDeltaTime) * 20 * MouseMove * 0.1f;

			if (m_CamDegreeAngle.x < -20.f)m_CamDegreeAngle.x = -20.f;
			else if (m_CamDegreeAngle.x > 60.f)m_CamDegreeAngle.x = 60.f;
		}


		CTransform* pCamTransform = m_pMainCamera->Get_Camera_Transform();
		_Matrix OldCamMat = pCamTransform->Get_WorldMatrix();
		_float3 PlayerPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
		_Matrix	NewCamMatrix = XMMatrixTranslation(0.7f, 1.3f, m_CamDegreeAngle.z * _float(-0.7 * m_fUmbrellaIntro + 1)
				* m_fSmallScale * (1 - m_fDashPower / PlayerMaxDashPower * 0.1f))
			* XMMatrixRotationX(XMConvertToRadians(m_CamDegreeAngle.x))
			* XMMatrixRotationY(XMConvertToRadians(m_CamDegreeAngle.y))
			* XMMatrixTranslation(PlayerPos.x, PlayerPos.y, PlayerPos.z);


		NewCamMatrix.r[0] = XMVectorLerp(OldCamMat.r[0], NewCamMatrix.r[0], CamLerpLate);
		NewCamMatrix.r[1] = XMVectorLerp(OldCamMat.r[1], NewCamMatrix.r[1], CamLerpLate);
		NewCamMatrix.r[2] = XMVectorLerp(OldCamMat.r[2], NewCamMatrix.r[2], CamLerpLate);
		NewCamMatrix.r[3] = XMVectorLerp(OldCamMat.r[3], NewCamMatrix.r[3], CamLerpLate);

		pCamTransform->Set_Matrix(NewCamMatrix);

		FAILED_CHECK(m_pMainCamera->Set_ViewMatrix());
		FAILED_CHECK(m_pMainCamera->Set_ProjectMatrix());


	}
	else
	{


		if (m_bIsRockOn)
		{
			_Vector vMonsterPos = ((CTransform*)(m_pRockOnMonster->Get_Component(TAG_COM(Com_Transform))))->Get_MatrixState(CTransform::STATE_POS);
			_Vector vPlayerPos = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);

			_float fCamDist = _float(m_CamDegreeAngle.z * m_fSmallScale * (1 - m_fDashPower / PlayerMaxDashPower * 0.1f));
			_Vector vMon2PlayerDir = XMVector3Normalize(XMVectorSetY(vPlayerPos, 0) - XMVectorSetY(vMonsterPos, 0));


			CTransform* pCamTransform = m_pMainCamera->Get_Camera_Transform();

			pCamTransform->Set_MatrixState(CTransform::STATE_POS,
				vPlayerPos + (XMVectorSet(0, 1, 0, 0) * 1) - (vMon2PlayerDir * fCamDist));

			pCamTransform->LookAt(vMonsterPos);
		}
		else {

			_long		MouseMove = 0;
			if (MouseMove = pInstance->Get_DIMouseMoveState(CInput_Device::MMS_X))
			{
				m_CamDegreeAngle.y += _float(fDeltaTime) * 20 * MouseMove * 0.1f;

			}

			if (MouseMove = pInstance->Get_DIMouseMoveState(CInput_Device::MMS_Y))
			{
				m_CamDegreeAngle.x += _float(fDeltaTime) * 20 * MouseMove * 0.1f;

				if (m_CamDegreeAngle.x < -20.f)m_CamDegreeAngle.x = -20.f;
				else if (m_CamDegreeAngle.x > 60.f)m_CamDegreeAngle.x = 60.f;
			}


			CTransform* pCamTransform = m_pMainCamera->Get_Camera_Transform();
			_Matrix OldCamMat = pCamTransform->Get_WorldMatrix();
			_float3 PlayerPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
			_Matrix NewCamMatrix = XMMatrixTranslation(0, 1.5f * m_fSmallScale, m_CamDegreeAngle.z
				* m_fSmallScale * (1 - m_fDashPower / PlayerMaxDashPower * 0.1f))
				* XMMatrixRotationX(XMConvertToRadians(m_CamDegreeAngle.x))
				* XMMatrixRotationY(XMConvertToRadians(m_CamDegreeAngle.y))
				* XMMatrixTranslation(PlayerPos.x, PlayerPos.y, PlayerPos.z);


			NewCamMatrix.r[0] = XMVectorLerp(OldCamMat.r[0], NewCamMatrix.r[0], CamLerpLate);
			NewCamMatrix.r[1] = XMVectorLerp(OldCamMat.r[1], NewCamMatrix.r[1], CamLerpLate);
			NewCamMatrix.r[2] = XMVectorLerp(OldCamMat.r[2], NewCamMatrix.r[2], CamLerpLate);
			NewCamMatrix.r[3] = XMVectorLerp(OldCamMat.r[3], NewCamMatrix.r[3], CamLerpLate);

			pCamTransform->Set_Matrix(NewCamMatrix);

		}
		FAILED_CHECK(m_pMainCamera->Set_ViewMatrix());
		FAILED_CHECK(m_pMainCamera->Set_ProjectMatrix());

	}
	return S_OK;
}

CPlayer * CPlayer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CPlayer*	pInstance = new CPlayer(pDevice,pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CPlayer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPlayer::Clone(void * pArg)
{
	CPlayer*	pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CPlayer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModel);
	Safe_Release(m_pColliderCom);
	
	Safe_Release(m_pRockOnMonster);
	
	for (auto& pWeapon : m_vecWeapon)
		Safe_Release(pWeapon);

	
}
