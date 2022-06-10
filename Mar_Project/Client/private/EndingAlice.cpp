#include "stdafx.h"
#include "..\public\EndingAlice.h"
#include "Terrain.h"

CEndingAlice::CEndingAlice(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CMonster(pDevice,pDeviceContext)
{
}

CEndingAlice::CEndingAlice(const CEndingAlice & rhs)
	: CMonster(rhs)
{
}

HRESULT CEndingAlice::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));


	return S_OK;
}

HRESULT CEndingAlice::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	FAILED_CHECK(SetUp_Components());


	m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, _float3(21.631f, 20.f, 20.f));
	m_pTransformCom->LookAt(_float3(21.631f, 20.f, 18.f).XMVector());


	m_fHP = m_fMaxHP = 64;

	m_pModel->Change_AnimIndex(2);

	/*
		21.631f, 20.f, 20 ->z = 18
		17.725f, 20.f, 12
		17.725f, 20.f, 9.4f
	*/

	m_bSpwanAnimStart = false;
	m_SpwanPassedTime = 0;
	return S_OK;
}

_int CEndingAlice::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)return -1;

	if (GetSingle(CGameInstance)->Get_DIKeyState(DIK_9)&DIS_Down)
	{
		m_bSpwanAnimStart = true;
		m_SpwanPassedTime = 0;
		m_pModel->Change_AnimIndex(1);
	}
	if (GetSingle(CGameInstance)->Get_DIKeyState(DIK_0)&DIS_Down)
	{
		FAILED_CHECK(m_pModel->Change_AnimIndex(1));
	}

	if (m_bSpwanAnimStart)
	{
		m_SpwanPassedTime += fDeltaTime;

		_float3 EasedPos;

		if (m_SpwanPassedTime < 4.f)
		{
			EasedPos = g_pGameInstance->Easing_Vector(TYPE_Linear, _float3(21.631f, 20.f, 20), _float3(17.725f, 20.f, 12), (_float)m_SpwanPassedTime, 4.f);
			if (m_SpwanPassedTime > 3.85f)m_pModel->Change_AnimIndex(0);
		}
		else if (m_SpwanPassedTime < 5.f)
		{
			EasedPos = _float3(17.725f, 20.f, 12);

			if (m_SpwanPassedTime > 4.85f)m_pModel->Change_AnimIndex(1);
		}
		else if (m_SpwanPassedTime < 7.f)
		{
			EasedPos = g_pGameInstance->Easing_Vector(TYPE_Linear, _float3(17.725f, 20.f, 12), _float3(17.725f, 20.f, 9.4f), (_float)m_SpwanPassedTime - 5, 2.f);
		}
		else
		{
			EasedPos = _float3(17.725f, 20.f, 9.4f);
			m_pModel->Change_AnimIndex(0);
		}

		m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, EasedPos);

	}

	m_bIsOnScreen = true;
	FAILED_CHECK(m_pModel->Update_AnimationClip(fDeltaTime, m_bIsOnScreen));
	FAILED_CHECK(Adjust_AnimMovedTransform(fDeltaTime));


	return _int();
} 

_int CEndingAlice::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)return -1;


	FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));
	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
	return _int();
}

_int CEndingAlice::Render()
{
	if (__super::Render() < 0)		return -1;


	NULL_CHECK_RETURN(m_pModel, E_FAIL);


	FAILED_CHECK(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));

	CGameInstance* pInstance = GetSingle(CGameInstance);

	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_VIEW), sizeof(_float4x4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_PROJ), sizeof(_float4x4)));


	_uint NumMaterial = m_pModel->Get_NumMaterial();


	for (_uint i = 0; i < NumMaterial; i++)
	{
		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
			FAILED_CHECK(m_pModel->Bind_OnShader(m_pShaderCom, i, j, MODLETEXTYPE(j)));

		FAILED_CHECK(m_pModel->Render(m_pShaderCom, 1, i, "g_BoneMatrices"));
	}

	return _int();
}

_int CEndingAlice::LightRender()
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



HRESULT CEndingAlice::SetUp_Components()
{

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(Prototype_Mesh_PlayerEnding), TAG_COM(Com_Model), (CComponent**)&m_pModel));
	FAILED_CHECK(m_pModel->Change_AnimIndex(0));



	CTransform::TRANSFORMDESC tDesc = {};
	tDesc.fMovePerSec = 2.8f;
	tDesc.fRotationPerSec = XMConvertToRadians(60);
	tDesc.fScalingPerSec = 1;
	tDesc.vPivot = _float3(0, 0, 0);

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom, &tDesc));
	__super::SetUp_WanderLook(m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK));


	return S_OK;
}

HRESULT CEndingAlice::Set_Monster_On_Terrain(CTransform * pTransform, _double fDeltaTime)
{
	CGameInstance* pInstance = GetSingle(CGameInstance);

	m_LevitationTime += fDeltaTime;
	_float fGravity = _float((m_LevitationTime) * (m_LevitationTime)* -29.4f);

	pTransform->MovetoDir_bySpeed(XMVectorSet(0, 1.f, 0, 0), fGravity, fDeltaTime);


	CTerrain* pTerrain = (CTerrain*)(pInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Terrain)));

	_bool bIsOn = false;
	_uint eTileKinds = Tile_End;

	_float3 CaculatedPos = pTerrain->PutOnTerrain(&bIsOn, pTransform->Get_MatrixState(CTransform::STATE_POS), m_vOldPos.XMVector(),nullptr,&eTileKinds);

	if (bIsOn)
	{
		m_LevitationTime = 0;
		pTransform->Set_MatrixState(CTransform::STATE_POS, CaculatedPos);

	}

	return S_OK;
}

HRESULT CEndingAlice::Adjust_AnimMovedTransform(_double fDeltatime)
{
	if (!m_bSpwanAnimFinished) return S_FALSE;

	_uint iNowAnimIndex = m_pModel->Get_NowAnimIndex();
	_double PlayRate = m_pModel->Get_PlayRate();

	if (iNowAnimIndex != m_iOldAnimIndex || PlayRate > 0.95)
		m_iAdjMovedIndex = 0;


	if (PlayRate <= 0.95)
	{
		switch (iNowAnimIndex)
		{
		case 1:
		
			break;
		default:
			break;
		}
	}





	m_iOldAnimIndex = iNowAnimIndex;
	return S_OK;
}

CEndingAlice * CEndingAlice::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEndingAlice*	pInstance = new CEndingAlice(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CEndingAlice");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEndingAlice::Clone(void * pArg)
{
	CEndingAlice*	pInstance = new CEndingAlice(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CEndingAlice");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEndingAlice::Free()
{
	__super::Free();


	

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModel);

}
