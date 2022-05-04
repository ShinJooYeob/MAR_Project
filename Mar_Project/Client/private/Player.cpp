#include "stdafx.h"
#include "..\public\Player.h"
#include "Weapon.h"
#include "Camera.h"
#include "Terrain.h"
#include "GamePlayUI.h"



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


	return S_OK;
}

_int CPlayer::Update(_double fDeltaTime)
{
	if (m_eNowSceneNum == SCENE_LOADING)
		return 0;

	if (__super::Update(fDeltaTime) < 0)return -1;
	m_pColliderCom->Update_ConflictPassedTime(fDeltaTime);



	FAILED_CHECK(Manage_CoolTime(fDeltaTime));
	FAILED_CHECK(Input_Keyboard(fDeltaTime));



	if (g_pGameInstance->Get_DIKeyState(DIK_1)&DIS_Down)
		Add_Dmg_to_Player(1);
	if (g_pGameInstance->Get_DIKeyState(DIK_2)&DIS_Down)
		Add_Dmg_to_Player(2);
	if (g_pGameInstance->Get_DIKeyState(DIK_3)&DIS_Down)
		Add_Dmg_to_Player(3);
	if (g_pGameInstance->Get_DIKeyState(DIK_4)&DIS_Down)
		Add_Dmg_to_Player(4);
	if (g_pGameInstance->Get_DIKeyState(DIK_5)&DIS_Down)
		Add_Dmg_to_Player(5);
	if (g_pGameInstance->Get_DIKeyState(DIK_6)&DIS_Down)
		Add_Dmg_to_Player(6);
	//if (g_pGameInstance->Get_DIKeyState(DIK_7)&DIS_Down)
	//	m_pModel->Change_AnimIndex(6);
	//if (g_pGameInstance->Get_DIKeyState(DIK_8)&DIS_Down)
	//	m_pModel->Change_AnimIndex(7);
	//if (g_pGameInstance->Get_DIKeyState(DIK_9)&DIS_Down)
	//	m_pModel->Change_AnimIndex(8);
	//

	//for (_uint i = 0; i < m_pColliderCom->Get_NumColliderBuffer(); i++)
	//{
	//	m_pColliderCom->Update_Transform(i, m_pTransformCom->Get_WorldMatrix());
	//}
	if (m_iWeaponModelIndex != 10)
	{
		m_vecWeapon[m_iWeaponModelIndex]->Update(fDeltaTime);
		m_vBulletFirePos = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS) + (m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK) * 0.6f)
			+ (m_pTransformCom->Get_MatrixState(CTransform::STATE_UP)) + (m_pTransformCom->Get_MatrixState(CTransform::STATE_RIGHT)*0.1f);
	}



	_uint iNowAnimIndex  = m_pModel->Get_NowAnimIndex();
	if (iNowAnimIndex ==  15|| iNowAnimIndex == 20||
		(iNowAnimIndex >= Weapon_Knife + 8 && iNowAnimIndex <= Weapon_Knife + 16) ||
		(iNowAnimIndex >= Weapon_Horse + 8 && iNowAnimIndex <= Weapon_Horse + 14))
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

	g_pGameInstance->Add_CollisionGroup(CollisionType_Player, this, m_pColliderCom);

	return _int();
}

_int CPlayer::LateUpdate(_double fDeltaTime)
{
	if (m_eNowSceneNum == SCENE_LOADING)
		return 0;

	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;

	static bool test = false;
	if (g_pGameInstance->Get_DIKeyState(DIK_L)&DIS_Down)
		test = !test;

	if (!test)
	{
		FAILED_CHECK(Set_Player_On_Terrain());
	}
	else {
		FAILED_CHECK(Set_Player_On_Terrain());

		//FAILED_CHECK(Set_Player_On_Slieder(fDeltaTime));
	}
	
	FAILED_CHECK(Set_Camera_On_Player(fDeltaTime));




	if (!m_fDashPassedTime)
	{
		FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));
		if (m_iWeaponModelIndex != 10 && !m_LevitationTime)
		{
			m_vecWeapon[m_iWeaponModelIndex]->LateUpdate(fDeltaTime);
		}


	}

	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
	return _int();
}

_int CPlayer::Render()
{
	if (__super::Render() < 0)
		return -1;


	NULL_CHECK_RETURN(m_pModel, E_FAIL);


#ifdef _DEBUG
	m_pColliderCom->Render();
#endif // _DEBUG



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

_int CPlayer::LateRender()
{
	if (__super::LateRender() < 0)
		return -1;




	return _int();
}

void CPlayer::Add_Dmg_to_Player(_uint iDmgAmount)
{
	if (!iDmgAmount) return;

	m_iHP -= iDmgAmount;

	if (m_iHP <= 0)m_iHP = 32;


	((CGamePlayUI*)(g_pGameInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_UI_GamePlay))))->Add_Dmg_to_Player(m_iHP,iDmgAmount);

	ZeroMemory(m_bAtkMoveMentChecker, sizeof(_bool) * 3);
	m_bIsAttackClicked = false;
	m_iAttackCount = 0;

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
		m_fUmbrellaIntro = 0;
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

	 m_tCollisionAttachPtr[0]= m_pModel->Find_AttachMatrix_InHirarchyNode("Bip01-Spine2");
	 NULL_CHECK_RETURN(m_tCollisionAttachPtr[0].pDefaultPivotMat, E_FAIL);



	 //Pivot  : 0.000000f , 0.000000f , -0.620000f , 1
	 //size  : 0.960000f , 0.860000f , 0.550000f  
	 ZeroMemory(&ColliderDesc, sizeof(COLLIDERDESC));
	 ColliderDesc.vScale = _float3(0.480000f, 0.550000f,0.860000f);
	 ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	 ColliderDesc.vPosition = _float4(0.000000f, 0.000000f, -0.620000f, 1);
	 FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_OBB, &ColliderDesc));
	 m_pColliderCom->Set_ParantBuffer();

	 m_tCollisionAttachPtr[1] = m_pModel->Find_AttachMatrix_InHirarchyNode("Bip01-Pelvis");
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

	m_pMainCamera =(CCamera*)(g_pGameInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Camera_Main)));
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


	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_CamPosition", &pInstance->Get_TargetPostion_float4(PLV_CAMERA), sizeof(_float4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_CamLookDir", &pInstance->Get_TargetPostion_float4(PLV_CAMLOOK), sizeof(_float4)));


	const LIGHTDESC* pLightDesc = pInstance->Get_LightDesc(LIGHTDESC::TYPE_DIRECTIONAL, 0);
	NULL_CHECK_RETURN(pLightDesc, -1);

	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vLightVector", &(pLightDesc->vVector), sizeof(_float4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4)));


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

HRESULT CPlayer::Input_Keyboard(_double fDeltaTime)
{

	CGameInstance* pInstance = GetSingle(CGameInstance);



	/*To All*/
	if (m_eNowWeapon != CPlayer::Weapon_Umbrella)
		FAILED_CHECK(Dash_Update(fDeltaTime, pInstance));
	FAILED_CHECK(RockOn_Update(fDeltaTime, pInstance));

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





	//FAILED_CHECK(Lunch_Bullet(fDeltaTime, pInstance));
	//FAILED_CHECK(Lunch_Grenade(fDeltaTime, pInstance));



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
		else {

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

			if (PressedChecker[0]) Dir += forword;
			if (PressedChecker[1]) Dir -= forword;
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


				if (!m_LevitationTime)
					m_pModel->Change_AnimIndex(8);
			}


		}
		else {
			if (!m_LevitationTime &&!m_fDashPassedTime)
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

	if (m_LevitationTime != fDeltaTime &&fGravity < 0)
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
		m_fDashPower = pInstance->Easing_Return(TYPE_ExpoInOut, TYPE_ExpoInOut,0, PlayerMaxDashPower, m_fDashPassedTime, TotalDashTime);
		
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
	BYTE LTabState = pInstance->Get_DIKeyState(DIK_TAB);

	if (LTabState & DIS_Down)
	{
		if (LTabState & DIS_DoubleDown)
		{
			Safe_Release(m_pRockOnMonster);

			m_bIsRockOn = false;
		}
		else 
		{
			_Vector vPlayerPos = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
			_Vector vLookVector = XMVector3Normalize(XMVectorSetY(vPlayerPos, 0)
				- XMVectorSetY(m_pMainCamera->Get_Camera_Transform()->Get_MatrixState(CTransform::STATE_POS), 0));

			//나중에 몬스터 넣으면 몬스터로 바꾸자
			list<CGameObject*>* MosterList = pInstance->Get_ObjectList_from_Layer(m_eNowSceneNum, TAG_LAY(Layer_JumpPad));

			_float	fNear = 5.f;
			_float Length = 0;
			CGameObject* pNearMonster = nullptr;
			
			_Vector vMonsterPos;

			for (auto& pMonster : *MosterList)
			{
				vMonsterPos = ((CTransform*)(pMonster->Get_Component(TAG_COM(Com_Transform))))->Get_MatrixState(CTransform::STATE_POS);


				_float fCosValue = XMVectorGetX(XMVector3Dot(vLookVector, XMVector3Normalize
				(XMVectorSetY(vMonsterPos, 0) - XMVectorSetY(vPlayerPos, 0))));

				if (fCosValue > (sqrtf(3) / 2.f) && fNear > (Length = XMVectorGetX(XMVector3Length(vPlayerPos - vMonsterPos))))
				{
					fNear = Length;
					pNearMonster = pMonster;
				}

			}

			if (pNearMonster == nullptr)
				return S_FALSE;

			Safe_Release(m_pRockOnMonster);
			m_pRockOnMonster = pNearMonster;
			Safe_AddRef(m_pRockOnMonster);
			m_bIsRockOn = true;
		}
	}








	return S_OK;
}

HRESULT CPlayer::Plant_ClockBomb(_double fDeltaTime, CGameInstance * pInstance)
{
	if (pInstance->Get_DIKeyState(DIK_E) & DIS_Down)
	{
		m_pModel->Change_AnimIndex_ReturnTo(44, 0, 0.15, true);

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

	if (m_LevitationTime != fDeltaTime &&fGravity < 0)
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


	if (g_pGameInstance->Get_DIKeyState(DIK_Z) &DIS_Down)
		m_vecWeapon[m_iWeaponModelIndex]->Get_WeaponModel()->Change_AnimIndex_ReturnTo_Must(1, 0, 0, true);

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

	if (m_LevitationTime != fDeltaTime &&fGravity < 0)
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

		if (!m_LevitationTime)
			m_pModel->Change_AnimIndex_UntilTo(Weapon_Umbrella + 1, Weapon_Umbrella + 2);

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

	if (m_LevitationTime != fDeltaTime &&fGravity < 0)
		m_pModel->Change_AnimIndex(17);

	m_pTransformCom->MovetoDir_bySpeed(XMVectorSet(0, 1.f, 0, 0), fGravity, fDeltaTime);

	return S_OK;
}

HRESULT CPlayer::Attack_Update_Umbrella(_double fDeltaTime, CGameInstance * pInstance)
{

	if (m_fUmbrellaIntro >= 1)
	{
		if (pInstance->Get_DIMouseButtonState(CInput_Device::MBS_LBUTTON) & DIS_Down)
			m_pModel->Change_AnimIndex_ReturnTo_Must(Weapon_Umbrella + 4, Weapon_Umbrella + 1, 0.15, true);
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

	if (m_LevitationTime != fDeltaTime &&fGravity < 0)
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

	if (m_LevitationTime != fDeltaTime &&fGravity < 0)
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
				m_pModel->Change_AnimIndex_ReturnTo(19, 0, 0, true);
				Add_Force(m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK), 10);
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
	_float3 vPlaneWorldNormal;

	_float3 CaculatedPos = pTerrain->PutOnTerrain(&bIsOn, m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), m_vOldPos.XMVector(),&vPlaneWorldNormal);

	if (bIsOn)
	{
		m_LevitationTime = 0;
		m_fJumpPower = -1.f;
		m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, CaculatedPos);

		_Vector vRight;
		_Matrix matScale = m_pTransformCom->Get_MatrixScale_All();


		_Vector vUp = vPlaneWorldNormal.XMVector();

		m_pTransformCom->Set_MatrixState(CTransform::STATE_UP, vUp * matScale.r[CTransform::STATE_UP]);

		if (XMVector3Equal( _float3(0, 1, 0).XMVector(), vUp))
		{
			//__debugbreak();
			//MSGBOX("Can't Cross With Same Vector");
#ifdef _DEBUG
			OutputDebugString(TEXT("Error: Cross With Same Vector (Player)\n"));
#endif // _DEBUG


			vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.000001f, 1, 0, 0), vUp));
			m_pTransformCom->Set_MatrixState(CTransform::STATE_RIGHT, vRight * matScale.r[CTransform::STATE_RIGHT]);

		}
		else {

			vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0,1,0,0), vUp));
			m_pTransformCom->Set_MatrixState(CTransform::STATE_RIGHT, vRight * matScale.r[CTransform::STATE_RIGHT]);

		}

		m_pTransformCom->Set_MatrixState(CTransform::STATE_LOOK, XMVector3Normalize(XMVector3Cross(vRight, vUp))  * matScale.r[CTransform::STATE_LOOK]);


		m_pTransformCom->Move_Forward(fDeltatime);
	}
	return S_OK;
}

HRESULT CPlayer::Set_Camera_On_Player(_double fDeltaTime)
{
	CGameInstance* pInstance = GetSingle(CGameInstance);
#define CamLerpLate 0.2f

	if (m_eNowWeapon == CPlayer::Weapon_Grinder)
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
