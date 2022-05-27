#include "stdafx.h"
#include "..\public\JumpPad.h"
#include "Player.h"



CJumpPad::CJumpPad(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CMapObject(pDevice,pDeviceContext)
{
}

CJumpPad::CJumpPad(const CJumpPad & rhs)
	: CMapObject(rhs)
{
}

HRESULT CJumpPad::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));


	return S_OK;
}

HRESULT CJumpPad::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	if (pArg != nullptr)
	{
		memcpy(&m_tDesc, pArg, sizeof(JUMPPADDESC));
	}


	FAILED_CHECK(SetUp_Components());

	m_pModel->Change_AnimIndex(1);
	m_pModel->Change_AnimIndex(0);

	m_fRangeRadius = 0.707f * (m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_RIGHT).Get_Lenth());
	return S_OK;
}

_int CJumpPad::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;


	
// 	if (g_pGameInstance->Get_DIKeyState(DIK_UP) & DIS_Down)
// 	{
// 		m_fRangeRadius += 0.1f;
// 
// 
// 	string ttszLog = "Radius: " + to_string(m_fRangeRadius) + "\n";
// 	wstring ttDebugLog;
// 	ttDebugLog.assign(ttszLog.begin(), ttszLog.end());
// 
// 	OutputDebugStringW(ttDebugLog.c_str());
// 	}
// 	else if (g_pGameInstance->Get_DIKeyState(DIK_DOWN) & DIS_Down)
// 	{
// 		m_fRangeRadius -= 0.1f;
// 
// 	string ttszLog = "Radius: " + to_string(m_fRangeRadius) + "\n";
// 	wstring ttDebugLog;
// 	ttDebugLog.assign(ttszLog.begin(), ttszLog.end());
// 
// 	OutputDebugStringW(ttDebugLog.c_str());
// 
// 
// 	}
	



	if (XMVectorGetX(XMVector3Length(m_pTransformCom->Get_MatrixState(CTransform::STATE_POS)
		- m_pPlayerTransform->Get_MatrixState(CTransform::STATE_POS))) > m_fRangeRadius)
	{
		m_bIsPlayerCloser = false;
		//return _int();
	}
	else
	{

		m_pPlayerTransform->MovetoDir(XMVectorSet(0, 1, 0, 0), fDeltaTime);
		m_pPlayer->Add_JumpForce(m_tDesc.fPower,1);
		m_pModel->Change_AnimIndex_ReturnTo_Must(2,  0, 0.08, true);
	}





	m_bIsOnScreen = g_pGameInstance->IsNeedToRender(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS));
	FAILED_CHECK(m_pModel->Update_AnimationClip(fDeltaTime, m_bIsOnScreen));
	return _int();
}

_int CJumpPad::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;

	//if (!m_bIsPlayerCloser) return _int();


	if (m_bIsOnScreen)
		FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));

	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
	return _int();
}

_int CJumpPad::Render()
{
	if (__super::Render() < 0)
		return -1;


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

		FAILED_CHECK(m_pModel->Render(m_pShaderCom, 0, i, "g_BoneMatrices"));
	}


	return _int();
}

_int CJumpPad::LightRender()
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

HRESULT CJumpPad::SetUp_Components()
{
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom));

	_Matrix WorldMatrix = XMMatrixScaling(m_tDesc.vScale.x, m_tDesc.vScale.y, m_tDesc.vScale.z) *
		XMMatrixTranslation(m_tDesc.vPosition.x, m_tDesc.vPosition.y, m_tDesc.vPosition.z);

	m_pTransformCom->Set_Matrix(WorldMatrix);

	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(Prototype_Mesh_JumpPad), TAG_COM(Com_Model), (CComponent**)&m_pModel));



	m_pPlayer = (CPlayer*)(g_pGameInstance->Get_GameObject_By_LayerIndex(SCENE_STATIC, TAG_LAY(Layer_Player)));

	NULL_CHECK_RETURN(m_pPlayer, E_FAIL);

	m_pPlayerTransform = (CTransform*)(m_pPlayer->Get_Component(TAG_COM(Com_Transform)));

	NULL_CHECK_RETURN(m_pPlayerTransform, E_FAIL);


	return S_OK;
}

CJumpPad * CJumpPad::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CJumpPad*	pInstance = new CJumpPad(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CJumpPad");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CJumpPad::Clone(void * pArg)
{
	CJumpPad*	pInstance = new CJumpPad(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CJumpPad");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CJumpPad::Free()
{
	__super::Free();
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModel);
	Safe_Release(m_pShaderCom);
}
