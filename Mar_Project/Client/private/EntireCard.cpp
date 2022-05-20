#include "stdafx.h"
#include "..\public\EntireCard.h"
#include "Player.h"
#include "Terrain.h"
#include "CardPeice.h"



CEntireCard::CEntireCard(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CMonsterWeapon(pDevice,pDeviceContext)
{
}

CEntireCard::CEntireCard(const CEntireCard & rhs)
	: CMonsterWeapon(rhs)
{
}

HRESULT CEntireCard::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));
	return S_OK;
}

HRESULT CEntireCard::Initialize_Clone(void * pArg)
{
	//FAILED_CHECK(__super::Initialize_Clone(pArg));

	if (pArg != nullptr)
	{
		m_pPlayer = (CPlayer*)(g_pGameInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Player)));

		NULL_CHECK_RETURN(m_pPlayer, E_FAIL);


		memcpy(&m_tDesc, pArg, sizeof(ENTIRECARDDESC));
	}

	FAILED_CHECK(SetUp_Components());

	m_PatternChangeTime = 0;
	


	m_vTurningVector = XMVector3Normalize(XMVectorSetY(m_tDesc.vMonsterPos.XMVector(), 0) - XMVectorSetY(m_tDesc.vStartPos.XMVector(), 0));

	m_pTransformCom->LookDir(m_vTurningVector.XMVector());

	return S_OK;
}

_int CEntireCard::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;

	m_pTransformCom->Turn_CW(m_vTurningVector.XMVector(), fDeltaTime);



	m_PatternChangeTime += fDeltaTime;
	_float EasedPos = g_pGameInstance->Easing_Return(TYPE_QuadOut, TYPE_QuadIn, m_tDesc.vStartPos.y - 5, m_tDesc.vStartPos.y + 5, (_float)m_PatternChangeTime, 2);


	if (m_PatternChangeTime > 1.25)
	{
		Spout_Piece();
	}

	m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, XMVectorSet(m_tDesc.vStartPos.x, EasedPos,m_tDesc.vStartPos.z,1));



	m_bIsOnScreen = g_pGameInstance->IsNeedToRender(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS),8);


	return _int();
}

_int CEntireCard::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;



	if (m_bIsOnScreen)
		FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));

	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
	return _int();
}

_int CEntireCard::Render()
{
	if (__super::Render() < 0)
		return -1;

	FAILED_CHECK(m_pTransformCom->Bind_OnShader_ApplyPivot(m_pShaderCom, "g_WorldMatrix"));

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



	FAILED_CHECK(m_pTransformCom->Bind_OnShader_ApplyPivot(m_pShaderCom, "g_WorldMatrix"));

	for (_uint k  = 0 ; k< 4 ; k++)
	{
		_uint NumMaterial = (m_ModelArr)[k]->Get_NumMaterial();

		for (_uint i = 0; i < NumMaterial; i++)
		{

			for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
				FAILED_CHECK(m_ModelArr[k]->Bind_OnShader(m_pShaderCom, i, j, MODLETEXTYPE(j)));

			FAILED_CHECK(m_ModelArr[k]->Render(m_pShaderCom, 1, i));
		}

	}
	
	

	return _int();
}

_int CEntireCard::LateRender()
{
	if (__super::LateRender() < 0)
		return -1;

	return _int();
}

void CEntireCard::Spout_Piece()
{
	CGameInstance* pInstance = g_pGameInstance;

	GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_CamShaking, 0.2f, _float4(0.3f));

	for (_uint i = Prototype_Mesh_CardPiece1; i<= Prototype_Mesh_CardPiece4; i++)
	{
		CCardPiece::CARDPIECEDESC tDesc;

		tDesc.vPosition = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
		tDesc.MeshKinds = i;
		tDesc.MoveDir = m_vTurningVector.XMVector() * -1;

		pInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet), TAG_OP(Prototype_CardPiece), &tDesc);

	}




	Set_IsDead();
}





HRESULT CEntireCard::SetUp_Components()
{
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VNAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom));


	m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, m_tDesc.vStartPos);
	m_pTransformCom->Set_TurnSpeed(XMConvertToRadians(360));

	ZeroMemory((m_ModelArr), sizeof(CModel*) * 4);

	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(Prototype_Mesh_CardPiece1), L"Com_Model_1", (CComponent**)&(m_ModelArr[0])));
	NULL_CHECK_RETURN((m_ModelArr[0]), E_FAIL);

	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(Prototype_Mesh_CardPiece2), L"Com_Model_2", (CComponent**)&(m_ModelArr[1])));
	NULL_CHECK_RETURN((m_ModelArr[1]), E_FAIL);

	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(Prototype_Mesh_CardPiece3), L"Com_Model_3", (CComponent**)&(m_ModelArr[2])));
	NULL_CHECK_RETURN((m_ModelArr[2]), E_FAIL);

	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(Prototype_Mesh_CardPiece4), L"Com_Model_4", (CComponent**)&(m_ModelArr[3])));
	NULL_CHECK_RETURN((m_ModelArr[3]), E_FAIL);




	m_pPlayer = (CPlayer*)(g_pGameInstance->Get_GameObject_By_LayerIndex(SCENE_STATIC, TAG_LAY(Layer_Player)));

	NULL_CHECK_RETURN(m_pPlayer, E_FAIL);

	m_pPlayerTransform = (CTransform*)(m_pPlayer->Get_Component(TAG_COM(Com_Transform)));

	NULL_CHECK_RETURN(m_pPlayerTransform, E_FAIL);



	return S_OK;
}



CEntireCard * CEntireCard::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEntireCard*	pInstance = new CEntireCard(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CEntireCard");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEntireCard::Clone(void * pArg)
{
	CEntireCard*	pInstance = new CEntireCard(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CEntireCard");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEntireCard::Free()
{
	__super::Free();

	for (auto& pModel : m_ModelArr)
		Safe_Release(pModel);


	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);


}
