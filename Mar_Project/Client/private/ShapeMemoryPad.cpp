#include "stdafx.h"
#include "..\public\ShapeMemoryPad.h"
#include "Player.h"
#include "Terrain.h"



CShapeMemoryPad::CShapeMemoryPad(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CMapObject(pDevice,pDeviceContext)
{
}

CShapeMemoryPad::CShapeMemoryPad(const CShapeMemoryPad & rhs)
	: CMapObject(rhs)
{
}

HRESULT CShapeMemoryPad::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));
	return S_OK;
}

HRESULT CShapeMemoryPad::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	if (pArg != nullptr)
	{
		memcpy(&m_tDesc, pArg, sizeof(SHAPEMEMDESC));
	}

	FAILED_CHECK(SetUp_Components());


	FAILED_CHECK(Ready_Piece());
	

	return S_OK;
}

_int CShapeMemoryPad::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;



	FAILED_CHECK(Update_PieceAngle(fDeltaTime));

	if (g_pGameInstance->Get_DIKeyState(DIK_1)&DIS_Down)
	{
		Let_ReturntoShape(!m_bReturnToShape);
	}




	m_bIsOnScreen = g_pGameInstance->IsNeedToRender(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS),8);


	return _int();
}

_int CShapeMemoryPad::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;



	if (m_bIsOnScreen)
		FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));

	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
	return _int();
}

_int CShapeMemoryPad::Render()
{
	if (__super::Render() < 0)
		return -1;



	_float4x4 OldMat = m_pTransformCom->Get_WorldMatrix();
	FAILED_CHECK(__super::SetUp_ConstTable(m_pShaderCom));



	for (auto& PieceDesc : m_vecPiece)
	{

		m_pTransformCom->Set_Pivot(PieceDesc.vPosition);
		m_pTransformCom->Rotation_CW(PieceDesc.vTurnAxis.XMVector(), PieceDesc.fEasedAngle);
		FAILED_CHECK(m_pTransformCom->Bind_OnShader_ApplyPivot(m_pShaderCom, "g_WorldMatrix"));

		_uint NumMaterial = (m_ModelArr)[PieceDesc.iKindsOfMesh]->Get_NumMaterial();

		for (_uint i = 0; i < NumMaterial; i++)
		{

			for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
				FAILED_CHECK((m_ModelArr)[PieceDesc.iKindsOfMesh]->Bind_OnShader(m_pShaderCom, i, j, MODLETEXTYPE(j)));

			FAILED_CHECK((m_ModelArr)[PieceDesc.iKindsOfMesh]->Render(m_pShaderCom, 2, i));
		}

	}
	
	m_pTransformCom->Set_Matrix(OldMat);

	return _int();
}

_int CShapeMemoryPad::LateRender()
{
	if (__super::LateRender() < 0)
		return -1;

	return _int();
}

HRESULT CShapeMemoryPad::Set_TerrainTileKinds()
{
	CTerrain* pTerrain = (CTerrain*)(g_pGameInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Terrain)));
	NULL_CHECK_RETURN(pTerrain, E_FAIL);



	_Matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();


	_Matrix RectMatrix = XMMatrixIdentity();

	RectMatrix.r[0] = m_NevRectPoint[0].XMVector();
	RectMatrix.r[1] = m_NevRectPoint[1].XMVector();
	RectMatrix.r[2] = m_NevRectPoint[2].XMVector();
	RectMatrix.r[3] = m_NevRectPoint[3].XMVector();



	FAILED_CHECK(pTerrain->Chage_TileKindsNHeight(RectMatrix * WorldMatrix));



	return S_OK;
}

HRESULT CShapeMemoryPad::Reset_TerrainTileKindsMovableNHeightZero()
{
	CTerrain* pTerrain = (CTerrain*)(g_pGameInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Terrain)));
	NULL_CHECK_RETURN(pTerrain, E_FAIL);


	_Matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();


	_Matrix RectMatrix = XMMatrixIdentity();

	RectMatrix.r[0] = m_NevRectPoint[0].XMVector();
	RectMatrix.r[1] = m_NevRectPoint[1].XMVector();
	RectMatrix.r[2] = m_NevRectPoint[2].XMVector();
	RectMatrix.r[3] = m_NevRectPoint[3].XMVector();



	FAILED_CHECK(pTerrain->Chage_TileKindsMovableNZero(RectMatrix * WorldMatrix));


	return S_OK;
}

HRESULT CShapeMemoryPad::Let_ReturntoShape(_bool bBool)
{
	if (m_PatternChangeTime < 13 || bBool == m_bReturnToShape) return S_FALSE;

	m_bReturnToShape = bBool;

	for (auto& Piece : m_vecPiece)
	{

		Piece.PassedTime = 0;
		Piece.ReturnToMem = m_bReturnToShape;
	}

	if (m_bReturnToShape)
		Set_TerrainTileKinds();

	else
		Reset_TerrainTileKindsMovableNHeightZero();
	


	return S_OK;
}



void CShapeMemoryPad::CollisionTriger(_uint iMyColliderIndex, CGameObject * pConflictedObj, CCollider * pConflictedCollider, _uint iConflictedObjColliderIndex, CollisionTypeID eConflictedObjCollisionType)
{
	switch (eConflictedObjCollisionType)
	{

	case Engine::CollisionType_Player:
	{
		//m_pPlayer->Set_LevitationTime((_float)g_fDeltaTime);
		_Vector Dir = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS) - m_vOldPos.XMVector();
		_float Lenth = XMVectorGetX(XMVector3Length(Dir));

		m_pPlayerTransform->MovetoDir_bySpeed(Dir, Lenth, 1);



		if (m_pPlayerTransform->Get_MatrixState_Float3(CTransform::STATE_POS).y < m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS).y + 2.5f)
		{
			_float3 PlayerPos = m_pPlayerTransform->Get_MatrixState_Float3(CTransform::STATE_POS);
			PlayerPos.y = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS).y + 2.5f;

			m_pPlayerTransform->Set_MatrixState(CTransform::STATE_POS, PlayerPos);
		}

		//pConflictedCollider->Set_Conflicted();
		//((CPlayer*)(pConflictedObj))->Add_Dmg_to_Player(rand() % 2 + 3);

	}
	break;
	case Engine::CollisionType_Terrain:
		break;

	default:
		break;
	}


}

HRESULT CShapeMemoryPad::SetUp_Components()
{
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VNAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom));


	m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, m_tDesc.vStartPos);
	m_pTransformCom->Scaled_All(_float3(2, 2, 2));

	ZeroMemory((m_ModelArr), sizeof(CModel*) * 5);

	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(Prototype_Mesh_HatterPlat_CheckerA), L"Com_Model_1", (CComponent**)&(m_ModelArr[0])));
	NULL_CHECK_RETURN((m_ModelArr[0]), E_FAIL);

	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(Prototype_Mesh_HatterPlat_CheckerB), L"Com_Model_2", (CComponent**)&(m_ModelArr[1])));
	NULL_CHECK_RETURN((m_ModelArr[1]), E_FAIL);

	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(Prototype_Mesh_HatterPlat_CheckerC), L"Com_Model_3", (CComponent**)&(m_ModelArr[2])));
	NULL_CHECK_RETURN((m_ModelArr[2]), E_FAIL);

	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(Prototype_Mesh_HatterPlat_CheckerE), L"Com_Model_4", (CComponent**)&(m_ModelArr[3])));
	NULL_CHECK_RETURN((m_ModelArr[3]), E_FAIL);

	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(Prototype_Mesh_HatterPlat_CheckerF), L"Com_Model_5", (CComponent**)&(m_ModelArr[4])));
	NULL_CHECK_RETURN((m_ModelArr[4]), E_FAIL);



	ZeroMemory(m_NevRectPoint, sizeof(_float4) * 4);

	m_NevRectPoint[0] = _float4(0, 0.4f, 0, 1);
	m_NevRectPoint[1] = _float4(5.12f, 0.4f, 0, 1);
	m_NevRectPoint[2] = _float4(5.12f, 0.4f, -7.68f, 1);
	m_NevRectPoint[3] = _float4(5.12f, 0.4f, -7.68f, 1);

	



	m_pPlayer = (CPlayer*)(g_pGameInstance->Get_GameObject_By_LayerIndex(SCENE_STATIC, TAG_LAY(Layer_Player)));

	NULL_CHECK_RETURN(m_pPlayer, E_FAIL);

	m_pPlayerTransform = (CTransform*)(m_pPlayer->Get_Component(TAG_COM(Com_Transform)));

	NULL_CHECK_RETURN(m_pPlayerTransform, E_FAIL);



	return S_OK;
}

HRESULT CShapeMemoryPad::Ready_Piece()
{
	CGameInstance* pInstance = g_pGameInstance;
	CUtilityMgr* pUtil= GetSingle(CUtilityMgr);
	m_vecPiece.reserve(7);


	//_float3 vPosition = _float3(0);
	//_float3 vTurnAxis = _float3(0, 1, 0);
	//_float	fAngle = 0;
	//_uint	iKindsOfMesh = 0;
	//_double PassedTime = 0;
	//_double TotoalTime = 0;
	//_bool	ReturnToMem = false;

	SHAPEPIECEDESC tDesc;

	tDesc.vPosition = _float3(0);
	tDesc.vTurnAxis.x = pUtil->RandomFloat(0, 99999999.f);
	tDesc.vTurnAxis.y = 0;
	tDesc.vTurnAxis.z = pUtil->RandomFloat(-99999999.f,0);
	tDesc.vTurnAxis = tDesc.vTurnAxis.Get_Nomalize();
	tDesc.fEasedAngle = tDesc.fAngle = ((rand() % 2 + 1) * 360) + ((rand() % 2) ? pUtil->RandomFloat(45,135) : pUtil->RandomFloat(225, 315));
	tDesc.iKindsOfMesh = 2;
	tDesc.PassedTime = 0;
	tDesc.TotoalTime = pUtil->RandomFloat(0.8f, 1.8f);
	tDesc.ReturnToMem = false;
	m_vecPiece.push_back(tDesc);


	tDesc.vPosition = _float3(1.28f, 0, 1.28f);
	tDesc.vTurnAxis.x = pUtil->RandomFloat(0, 99999999.f);
	tDesc.vTurnAxis.y = 0;
	tDesc.vTurnAxis.z = pUtil->RandomFloat(-99999999.f, 0);
	tDesc.vTurnAxis = tDesc.vTurnAxis.Get_Nomalize();
	tDesc.fEasedAngle = tDesc.fAngle = ((rand() % 2 + 1) * 360) + ((rand() % 2) ? pUtil->RandomFloat(45, 135) : pUtil->RandomFloat(225, 315));
	tDesc.iKindsOfMesh = 4;
	tDesc.PassedTime = 0;
	tDesc.TotoalTime = pUtil->RandomFloat(0.8f, 1.8f);
	tDesc.ReturnToMem = false;
	m_vecPiece.push_back(tDesc);


	tDesc.vPosition = _float3(2.56f, 0, 0);
	tDesc.vTurnAxis.x = pUtil->RandomFloat(0, 99999999.f);
	tDesc.vTurnAxis.y = 0;
	tDesc.vTurnAxis.z = pUtil->RandomFloat(-99999999.f, 0);
	tDesc.vTurnAxis = tDesc.vTurnAxis.Get_Nomalize();
	tDesc.fEasedAngle = tDesc.fAngle = ((rand() % 2 + 1) * 360) + ((rand() % 2) ? pUtil->RandomFloat(45, 135) : pUtil->RandomFloat(225, 315));
	tDesc.iKindsOfMesh = 1;
	tDesc.PassedTime = 0;
	tDesc.TotoalTime = pUtil->RandomFloat(0.8f, 1.8f);
	tDesc.ReturnToMem = false;
	m_vecPiece.push_back(tDesc);


	tDesc.vPosition = _float3(2.56f, 0, -2.56f);
	tDesc.vTurnAxis.x = pUtil->RandomFloat(0, 99999999.f);
	tDesc.vTurnAxis.y = 0;
	tDesc.vTurnAxis.z = pUtil->RandomFloat(-99999999.f, 0);
	tDesc.vTurnAxis = tDesc.vTurnAxis.Get_Nomalize();
	tDesc.fEasedAngle = tDesc.fAngle = ((rand() %2 + 1) * 360) + ((rand() % 2) ? pUtil->RandomFloat(45, 135) : pUtil->RandomFloat(225, 315));
	tDesc.iKindsOfMesh = 3;
	tDesc.PassedTime = 0;
	tDesc.TotoalTime = pUtil->RandomFloat(0.8f, 1.8f);
	tDesc.ReturnToMem = false;
	m_vecPiece.push_back(tDesc);


	tDesc.vPosition = _float3(0, 0, -3.84f);
	tDesc.vTurnAxis.x = pUtil->RandomFloat(0, 99999999.f);
	tDesc.vTurnAxis.y = 0;
	tDesc.vTurnAxis.z = pUtil->RandomFloat(-99999999.f, 0);
	tDesc.vTurnAxis = tDesc.vTurnAxis.Get_Nomalize();
	tDesc.fEasedAngle = tDesc.fAngle = ((rand() % 2 + 1) * 360) + ((rand() % 2) ? pUtil->RandomFloat(45, 135) : pUtil->RandomFloat(225, 315));
	tDesc.iKindsOfMesh = 1;
	tDesc.PassedTime = 0;
	tDesc.TotoalTime = pUtil->RandomFloat(0.8f, 1.8f);
	tDesc.ReturnToMem = false;
	m_vecPiece.push_back(tDesc);


	tDesc.vPosition = _float3(0, 0, -5.12f);
	tDesc.vTurnAxis.x = pUtil->RandomFloat(0, 99999999.f);
	tDesc.vTurnAxis.y = 0;
	tDesc.vTurnAxis.z = pUtil->RandomFloat(-99999999.f, 0);
	tDesc.vTurnAxis = tDesc.vTurnAxis.Get_Nomalize();
	tDesc.fEasedAngle = tDesc.fAngle = ((rand() % 2 + 1) * 360) + ((rand() % 2) ? pUtil->RandomFloat(45, 135) : pUtil->RandomFloat(225, 315));
	tDesc.iKindsOfMesh = 4;
	tDesc.PassedTime = 0;
	tDesc.TotoalTime = pUtil->RandomFloat(0.8f, 1.8f);
	tDesc.ReturnToMem = false;
	m_vecPiece.push_back(tDesc);


	tDesc.vPosition = _float3(2.56f, 0, -6.4f);
	tDesc.vTurnAxis.x = pUtil->RandomFloat(0, 99999999.f);
	tDesc.vTurnAxis.y = 0;
	tDesc.vTurnAxis.z = pUtil->RandomFloat(-99999999.f, 0);
	tDesc.vTurnAxis = tDesc.vTurnAxis.Get_Nomalize();
	tDesc.fEasedAngle = tDesc.fAngle = ((rand() % 2 + 1) * 360) + ((rand() % 2) ? pUtil->RandomFloat(45, 135) : pUtil->RandomFloat(225, 315));
	tDesc.iKindsOfMesh = 0;
	tDesc.PassedTime = 0;
	tDesc.TotoalTime = pUtil->RandomFloat(0.8f, 1.8f);
	tDesc.ReturnToMem = false;
	m_vecPiece.push_back(tDesc);



	return S_OK;
}

HRESULT CShapeMemoryPad::Update_PieceAngle(_double fDeltaTime)
{
	if (m_PatternChangeTime < 13)
		m_PatternChangeTime += fDeltaTime;

	


	CGameInstance* pInstance = g_pGameInstance;

	for (auto& tPieceDesc :m_vecPiece)
	{
		if (tPieceDesc.PassedTime > tPieceDesc.TotoalTime) continue;

		tPieceDesc.PassedTime += fDeltaTime;

		if (tPieceDesc.ReturnToMem)
		{
			tPieceDesc.fEasedAngle = pInstance->Easing(TYPE_ExpoInOut, tPieceDesc.fAngle, 0, (_float)tPieceDesc.PassedTime, (_float)tPieceDesc.TotoalTime);
			if (tPieceDesc.PassedTime > tPieceDesc.TotoalTime)	tPieceDesc.fEasedAngle = 0;
		}
		else
		{
			tPieceDesc.fEasedAngle = pInstance->Easing(TYPE_ExpoInOut, 0, tPieceDesc.fAngle, (_float)tPieceDesc.PassedTime, (_float)tPieceDesc.TotoalTime);
			if (tPieceDesc.PassedTime > tPieceDesc.TotoalTime)	tPieceDesc.fEasedAngle = tPieceDesc.fAngle;
		}

		if (tPieceDesc.PassedTime < tPieceDesc.TotoalTime * 0.3) break;
	
	}


	return S_OK;
}


CShapeMemoryPad * CShapeMemoryPad::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CShapeMemoryPad*	pInstance = new CShapeMemoryPad(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CShapeMemoryPad");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CShapeMemoryPad::Clone(void * pArg)
{
	CShapeMemoryPad*	pInstance = new CShapeMemoryPad(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CShapeMemoryPad");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CShapeMemoryPad::Free()
{
	__super::Free();

	for (auto& pModel : m_ModelArr)
		Safe_Release(pModel);


	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);


}
