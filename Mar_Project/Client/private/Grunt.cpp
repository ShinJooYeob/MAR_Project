#include "stdafx.h"
#include "..\public\Grunt.h"



CGrunt::CGrunt(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CMonster(pDevice, pDeviceContext)
{
}

CGrunt::CGrunt(const CGrunt & rhs)
	: CMonster(rhs)
{
}

HRESULT CGrunt::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));


	return S_OK;
}

HRESULT CGrunt::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	FAILED_CHECK(SetUp_Components());

	if (pArg != nullptr)
		m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, *((_float3*)pArg));

	ZeroMemory(m_bIsDmgAnimUpdated, sizeof(_bool) * 3);
	m_fHP = m_fMaxHP = 32;

	return S_OK;
}

_int CGrunt::Update(_double fDeltaTime)
{


	/*
	static float testFloat = 1.;
	if (g_pGameInstance->Get_DIKeyState(DIK_1)&DIS_Down)
	m_pModel->Change_AnimIndex(2);
	if (g_pGameInstance->Get_DIKeyState(DIK_2)&DIS_Down)
	m_pModel->Change_AnimIndex(6);
	if (g_pGameInstance->Get_DIKeyState(DIK_3)&DIS_Down)
	m_pModel->Change_AnimIndex(17);


	if (m_pInstance->Get_DIKeyState(DIK_UP) & DIS_Down)
	{
	testFloat += 0.1f;
	m_pTransformCom->Set_MoveSpeed(testFloat);


	string ttszLog = "Monster Speed: " + to_string(testFloat) + "\n";
	wstring ttDebugLog;
	ttDebugLog.assign(ttszLog.begin(), ttszLog.end());

	OutputDebugStringW(ttDebugLog.c_str());
	}
	else if (m_pInstance->Get_DIKeyState(DIK_DOWN) & DIS_Down)
	{
	testFloat -= 0.1f;
	m_pTransformCom->Set_MoveSpeed(testFloat);

	string ttszLog = "Monster Speed: " + to_string(testFloat) + "\n";
	wstring ttDebugLog;
	ttDebugLog.assign(ttszLog.begin(), ttszLog.end());

	OutputDebugStringW(ttDebugLog.c_str());

	if (g_pGameInstance->Get_DIKeyState(DIK_C)&DIS_Down)
	Add_Dmg_to_Monster(1);

	if (g_pGameInstance->Get_DIKeyState(DIK_V)&DIS_Down)
	{
	Add_Force(m_pTransformCom, m_pTransformCom->Get_MatrixState(CTransform::STATE_UP), 30);
	}

	}
	*/





	//_uint AnimIndex = m_pModel->Get_NowAnimIndex();

	//if (AnimIndex == 1)
	//	m_pModel->Set_NextAnim_Must(2);

	//if (!(AnimIndex>= 23 && AnimIndex <=25 || AnimIndex == 1))
	//{

	//	if (!m_bIsPatternFinished || Distance_BetweenPlayer(m_pTransformCom) < 3)
	//	{
	//		Update_Pattern(fDeltaTime);
	//	}
	//	else
	//	{
	//		if (!m_pModel->Get_IsUntillPlay())
	//		{
	//			m_pModel->Change_AnimIndex(2);
	//			FAILED_CHECK(__super::Update_WanderAround(m_pTransformCom, fDeltaTime, 0.05f));
	//		}
	//	}
	//}


	Update_DmgCalculate(fDeltaTime);

	m_bIsOnScreen = g_pGameInstance->IsNeedToRender(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS));

	for (_uint i =0; i< m_pColliderCom->Get_NumColliderBuffer(); i++)
		m_pColliderCom->Update_Transform(i, m_pTransformCom->Get_WorldMatrix());
	

	FAILED_CHECK(m_pModel->Update_AnimationClip(fDeltaTime, m_bIsOnScreen));
	return _int();
}

_int CGrunt::LateUpdate(_double fDeltaTime)
{

	FAILED_CHECK(__super::Set_Monster_On_Terrain(m_pTransformCom, fDeltaTime));

	m_pColliderCom->Inspect_Collision((CCollider*)(m_pPlayer->Get_Component(TAG_COM(Com_Collider))));


	if (m_bIsOnScreen)
		FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));


	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
	return _int();
}

_int CGrunt::Render()
{
	if (__super::Render() < 0)
		return -1;


	NULL_CHECK_RETURN(m_pModel, E_FAIL);

#ifdef _DEBUG
	m_pColliderCom->Render();
#endif // _DEBUG


	FAILED_CHECK(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));

	CGameInstance* pInstance = GetSingle(CGameInstance);

	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_VIEW), sizeof(_float4x4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_PROJ), sizeof(_float4x4)));


	_uint NumMaterial = m_pModel->Get_NumMaterial();


	for (_uint i = 0; i < NumMaterial; i++)
	{
		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
			FAILED_CHECK(m_pModel->Bind_OnShader(m_pShaderCom, i, j, MODLETEXTYPE(j)));

		FAILED_CHECK(m_pModel->Render(m_pShaderCom, 0, i, "g_BoneMatrices"));
	}

	return _int();
}

_int CGrunt::LateRender()
{
	return _int();
}

_int CGrunt::Update_DmgCalculate(_double fDeltaTime)
{
	if (m_DmgPassedTime <= 0)
	{
		if (m_fDmgAmount > 0)
		{
			ZeroMemory(m_bIsDmgAnimUpdated, sizeof(_bool) * 3);
			m_fDmgAmount = 0;
			m_DmgPassedTime = 0;
		}
		return 0;
	}
	m_DmgPassedTime -= fDeltaTime;

	if (!m_bIsDmgAnimUpdated[0] && m_fMaxHP * 0.1 < m_fDmgAmount)
	{
		m_pModel->Change_AnimIndex_ReturnTo_Must(23, 1, 0.15, true);
		m_bIsPatternFinished = true;
		m_PatternPassedTime = 0;
		Add_Force(m_pTransformCom, m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK)* -1, 10);
		m_pTransformCom->Set_MoveSpeed(0.5);
		m_bIsDmgAnimUpdated[0] = true;
	}
	else if (!m_bIsDmgAnimUpdated[1] && m_fMaxHP * 0.2 < m_fDmgAmount)
	{
		m_pModel->Change_AnimIndex_ReturnTo_Must(24, 1, 0.15, true);
		m_bIsPatternFinished = true;
		m_PatternPassedTime = 0;
		m_pTransformCom->Set_MoveSpeed(0.5);
		Add_Force(m_pTransformCom, m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK)* -1, 20);
		m_bIsDmgAnimUpdated[1] = true;
	}



	return 0;
}

_int CGrunt::Update_Pattern(_double fDeltaTime)
{

	m_PatternPassedTime += fDeltaTime;
	if (m_bIsPatternFinished)
	{
		m_ePattern += 1;
		if (m_ePattern > 3) m_ePattern = 0;
		m_bIsPatternFinished = false;
		m_PatternPassedTime = 0;
		//m_pModel->Change_AnimIndex(2);
	}

	switch (m_ePattern)
	{
	case 0:
		if (!m_PatternPassedTime)
		{
			m_pModel->Change_AnimIndex_UntilNReturn(3, 4,0, 0.15, true);
		}
		else
		{
			if (!m_pModel->Get_IsUntillPlay())
			{
				m_bIsPatternFinished = true;
				m_PatternPassedTime = 0;
			}
		}


		break;
	case 1:
		if (!m_PatternPassedTime)
		{
			m_pModel->Change_AnimIndex_UntilTo(5, 6, 0.15, true);
			m_pTransformCom->Set_MoveSpeed(6.5);
		}
		else
		{

			if (m_PatternPassedTime < 7)
			{
				if (m_pModel->Get_NowAnimIndex() == 6)
					FAILED_CHECK(__super::Update_WanderAround(m_pTransformCom, fDeltaTime, 0.25f));
			}
			else
			{
				m_pModel->Change_AnimIndex_UntilNReturn(7, 8, 2, 0.15, true);
				Add_Force(m_pTransformCom, m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK), 10);
				m_bIsPatternFinished = true;
				m_PatternPassedTime = 0;
				m_pTransformCom->Set_MoveSpeed(0.5);
			}
		}


		break;
	case 2:
		if (!m_PatternPassedTime)
		{
			m_pModel->Change_AnimIndex_UntilNReturn(9, 11, 0,0.15, true);
		}
		else
		{
			if (!m_pModel->Get_IsUntillPlay())
			{
				m_bIsPatternFinished = true;
				m_PatternPassedTime = 0;
			}
		}

		break;
	case 3:
		if (!m_PatternPassedTime)
		{
			m_pModel->Change_AnimIndex_UntilNReturn(12, 13, 0,0.15, true);
		}
		else
		{
			if (!m_pModel->Get_IsUntillPlay())
			{
				m_bIsPatternFinished = true;
				m_PatternPassedTime = 0;
			}
		}

		break;


	default:
		break;
	}






	/*
	2	=> 0.5
	6	=>6.5
	17	=>0.3
	*/


	return _int();
}

void CGrunt::Add_Dmg_to_Monster(_float iDmgAmount)
{
	m_DmgPassedTime = MonsterDmgTime;
	m_fDmgAmount += iDmgAmount;
	m_fHP -= iDmgAmount;

}

HRESULT CGrunt::SetUp_Components()
{

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(Prototype_Mesh_Grunt), TAG_COM(Com_Model), (CComponent**)&m_pModel));
	FAILED_CHECK(m_pModel->Change_AnimIndex(2));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Collider), TAG_COM(Com_Collider), (CComponent**)&m_pColliderCom));
	
	COLLIDERDESC			ColliderDesc;
	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(COLLIDERDESC));

	ColliderDesc.vScale = _float3(5.f, 5.f, 5.f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(0.f, 0.1f, 0.f, 1.f);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));


	ColliderDesc.vScale = _float3(3.f, 3.f, 3.f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(0.f, 0.1f, 0.f, 1.f);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));
	m_pColliderCom->Set_ParantBuffer();

	CTransform::TRANSFORMDESC tDesc = {};

	tDesc.fMovePerSec = 0.5;
	tDesc.fRotationPerSec = XMConvertToRadians(60);
	tDesc.fScalingPerSec = 1;
	tDesc.vPivot = _float3(0, 0, 0);

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom, &tDesc));

	__super::SetUp_WanderLook(m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK));

	return S_OK;
}

CGrunt * CGrunt::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CGrunt*	pInstance = new CGrunt(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CGrunt");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CGrunt::Clone(void * pArg)
{
	CGrunt*	pInstance = new CGrunt(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CGrunt");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGrunt::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModel);

}
