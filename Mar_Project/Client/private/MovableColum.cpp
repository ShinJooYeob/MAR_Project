#include "stdafx.h"
#include "..\public\MovableColum.h"
#include "Player.h"
#include "Terrain.h"



CMovableColum::CMovableColum(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CMapObject(pDevice,pDeviceContext)
{
}

CMovableColum::CMovableColum(const CMovableColum & rhs)
	: CMapObject(rhs)
{
}

HRESULT CMovableColum::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));
	return S_OK;
}

HRESULT CMovableColum::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	if (pArg != nullptr)
	{
		memcpy(&m_tDesc, pArg, sizeof(CLOCKCOLUMDESC));
	}

	FAILED_CHECK(SetUp_Components());

	FAILED_CHECK(Set_TerrainTileKindsAToB(Tile_Movable, Tile_DynamicNoneTile));
	//FAILED_CHECK(Set_TerrainTileKinds());

	return S_OK;
}

_int CMovableColum::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;

	static _uint i = 0;

	//if (g_pGameInstance->Get_DIKeyState(DIK_2)&DIS_Down)
	//{
	//	Start_Turning_Colum(i);		
	//	i++;
	//	if (i > 2) i = 0;
	//}



	FAILED_CHECK(Update_Turning(fDeltaTime));

	m_bIsOnScreen = g_pGameInstance->IsNeedToRender(m_pTransformCom->Get_MatrixState(CTransform::STATE_POS) ,50);



	return _int();
}

_int CMovableColum::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;


	if (m_bIsOnScreen)
		FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));
	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);

	return _int();
}

_int CMovableColum::Render()
{
	if (__super::Render() < 0)
		return -1;



	NULL_CHECK_RETURN(m_pModel, E_FAIL);
	NULL_CHECK_RETURN(m_pModel, E_FAIL);

	FAILED_CHECK(__super::SetUp_ConstTable(m_pShaderCom));


	FAILED_CHECK(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
	_uint NumMaterial  = m_pModel->Get_NumMaterial();
	for (_uint i = 0; i < NumMaterial; i++)
	{

		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
			FAILED_CHECK(m_pModel->Bind_OnShader(m_pShaderCom, i, j, MODLETEXTYPE(j)));

		FAILED_CHECK(m_pModel->Render(m_pShaderCom, 1, i));
	}



	return _int();
}

_int CMovableColum::LightRender()
{
	if (__super::LightRender() < 0)
		return -1;
	NULL_CHECK_RETURN(m_pModel, E_FAIL);


	FAILED_CHECK(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));



	_uint NumMaterial = m_pModel->Get_NumMaterial();

	for (_uint i = 0; i < NumMaterial; i++)
	{
		FAILED_CHECK(m_pModel->Render(m_pShaderCom, 9, i));
	}

	return _int();

}

_bool CMovableColum::Start_Turning_Colum(_uint iKinds)
{
	if (m_bIsTurning || m_iTuringKinds == iKinds) return false;
	

	//{
	//	SOUNDDESC tSoundDesc;

	//	tSoundDesc.vPosition = g_pGameInstance->Get_TargetPostion_float4(PLV_PLAYER);
	//	tSoundDesc.vMinMax = _float2(0, 25);
	//	tSoundDesc.fTargetSound = 0.3f;
	//	wstring SoundTrack = L"";
	//	SoundTrack = L"c1w2_amb_f_locked_close.ogg";

	//	//SoundTrack = L"MapObject_shrinkflower_open.ogg";

	//	g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);
	//}
	//


	m_PassedTime = 0;
	m_fStartAngle = m_tDesc.vAngle.y;
	(Set_TerrainTileKindsAToB(Tile_DynamicNoneTile, Tile_Movable));
	switch (iKinds)
	{
	case 0:
		m_fTargetAngle = 90;
		m_iTuringKinds = 0;
		break;
	case 1:
		m_fTargetAngle = 180;
		m_iTuringKinds = 1;
		break;
	case 2:
		m_fTargetAngle = 270;
		m_iTuringKinds = 2;
		break;

	default:
		break;
	}


	m_bIsTurning = true;
	GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_CamShaking, 5, _float4(0.5));

	return _bool();
}

HRESULT CMovableColum::Set_TerrainTileKindsAToB(_uint iSour, _uint iDest)
{
	CTerrain* pTerrain = (CTerrain*)(g_pGameInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Terrain)));
	NULL_CHECK_RETURN(pTerrain, E_FAIL);



	_Matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();


	_Matrix RectMatrix = XMMatrixIdentity();

	RectMatrix.r[0] = m_NevRectPoint[0].XMVector();
	RectMatrix.r[1] = m_NevRectPoint[1].XMVector();
	RectMatrix.r[2] = m_NevRectPoint[2].XMVector();
	RectMatrix.r[3] = m_NevRectPoint[3].XMVector();



	FAILED_CHECK(pTerrain->Chage_SourTileKind_To_DestTileKind(RectMatrix * WorldMatrix, iSour, iDest));
	return S_OK;
}




HRESULT CMovableColum::SetUp_Components()
{
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VNAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom));

	_Matrix WorldMatrix = XMMatrixRotationX(XMConvertToRadians(m_tDesc.vAngle.x))*
		XMMatrixRotationY(XMConvertToRadians(m_tDesc.vAngle.y))*
		XMMatrixRotationZ(XMConvertToRadians(m_tDesc.vAngle.z))*
		XMMatrixTranslation(m_tDesc.vPosition.x, m_tDesc.vPosition.y, m_tDesc.vPosition.z);

	m_fStartAngle = m_tDesc.vAngle.y;

	m_pTransformCom->Set_Matrix(WorldMatrix);


	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(Prototype_Mesh_HatterPillarsB_column), TAG_COM(Com_Model), (CComponent**)&m_pModel));


	ZeroMemory(m_NevRectPoint, sizeof(_float4) * 4);



	m_NevRectPoint[0] = _float4(-8.f, 0.f, 2.f, 1);
	m_NevRectPoint[1] = _float4(-8.f, 0.f, -2.f, 1);
	m_NevRectPoint[2] = _float4(-49.f, 0.f, -2.f, 1);
	m_NevRectPoint[3] = _float4(-49.f, 0.f, 2.f, 1);




	m_pPlayer = (CPlayer*)(g_pGameInstance->Get_GameObject_By_LayerIndex(SCENE_STATIC, TAG_LAY(Layer_Player)));

	NULL_CHECK_RETURN(m_pPlayer, E_FAIL);

	m_pPlayerTransform = (CTransform*)(m_pPlayer->Get_Component(TAG_COM(Com_Transform)));

	NULL_CHECK_RETURN(m_pPlayerTransform, E_FAIL);



	return S_OK;
}

HRESULT CMovableColum::Update_Turning(_double fDeltaTime)
{

	if (!m_bIsTurning)
	{
		m_bSoundChekcer = false;
		return S_FALSE;
	}
	m_PassedTime += fDeltaTime;


	_float EasedAngle = g_pGameInstance->Easing(TYPE_SinInOut, m_fStartAngle, m_fTargetAngle, (_float)m_PassedTime, 5);




	if(!m_bSoundChekcer  && m_PassedTime > 0.1f)
	{
		m_bSoundChekcer = true;

		SOUNDDESC tSoundDesc;

		tSoundDesc.vPosition = g_pGameInstance->Get_TargetPostion_float4(PLV_PLAYER);
		tSoundDesc.vMinMax = _float2(0, 25);
		tSoundDesc.fTargetSound = 0.8f;
		wstring SoundTrack = L"";
		SoundTrack = L"c3w1_stonedoor_slide01.ogg";

		//SoundTrack = L"MapObject_shrinkflower_open.ogg";

		g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);
	}


	if (m_PassedTime > 5)
	{
		m_PassedTime = 0;
		EasedAngle = m_fTargetAngle;
		m_tDesc.vAngle.y = m_fTargetAngle;
		m_bIsTurning = false;
		m_pTransformCom->Rotation_CW(XMVectorSet(0, 1, 0, 0), XMConvertToRadians(EasedAngle));
		FAILED_CHECK(Set_TerrainTileKindsAToB(Tile_Movable, Tile_DynamicNoneTile));
	}


	m_pTransformCom->Rotation_CW(XMVectorSet(0, 1, 0, 0), XMConvertToRadians(EasedAngle));
	

	 _Matrix LightPos= XMMatrixTranslation(128.f, 0.f, 0) * XMMatrixRotationY(XMConvertToRadians(EasedAngle)) *
		 XMMatrixTranslation(128.f, 256.f, 128.f);


	 g_pGameInstance->Relocate_LightDesc(tagLightDesc::TYPE_DIRECTIONAL, 0, LightPos.r[3]);

	return S_OK;
}



CMovableColum * CMovableColum::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CMovableColum*	pInstance = new CMovableColum(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CMovableColum");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CMovableColum::Clone(void * pArg)
{
	CMovableColum*	pInstance = new CMovableColum(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CMovableColum");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMovableColum::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModel);
	Safe_Release(m_pShaderCom);
}
