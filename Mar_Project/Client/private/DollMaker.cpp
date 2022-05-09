#include "stdafx.h"
#include "..\public\DollMaker.h"
#include "Scythe.h"
#include "Terrain.h"



CDollMaker::CDollMaker(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CMonster(pDevice,pDeviceContext)
{
}

CDollMaker::CDollMaker(const CDollMaker & rhs)
	: CMonster(rhs)
{
}

HRESULT CDollMaker::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));


	return S_OK;
}

HRESULT CDollMaker::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	FAILED_CHECK(SetUp_Components());


	if (pArg != nullptr)
		m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, *((_float3*)pArg));

	m_fHP = m_fMaxHP = 96;


	FAILED_CHECK(SetUp_Weapon());

	ZeroMemory(m_bIsDmgAnimUpdated, sizeof(_bool) * 3);
	m_pTransformCom->LookAt(XMVectorSet(60, 10, 60,0));

	return S_OK;
}

_int CDollMaker::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)return -1;
	//m_pColliderCom->Update_ConflictPassedTime(fDeltaTime);
	//Update_DmgCalculate(fDeltaTime);

	if (g_pGameInstance->Get_DIKeyState(DIK_1)&DIS_Down)
		m_pModel->Change_AnimIndex(2);

	if (g_pGameInstance->Get_DIKeyState(DIK_2)&DIS_Down)
	{
		m_pModel->Change_AnimIndex_UntilNReturn_Must(1,4,0,0.15,true);
	}
	if (g_pGameInstance->Get_DIKeyState(DIK_3)&DIS_Down)
	{
		m_pModel->Change_AnimIndex_UntilNReturn_Must(5, 8, 0, 0.15, true);
	}




	{
		_Vector TargetAt = m_pPlayerTransfrom->Get_MatrixState(CTransform::STATE_POS);

		TargetAt = XMVectorSetY(TargetAt, m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS).y);

		m_pTransformCom->LookAt(TargetAt);
	}
	



	{
		//Pivot  : -1.484999f , -0.020000f , -1.824999f , 1
		//rot  : -88.699219f , 1.200000f , 10.100002f  
		static _float3 testFloat3 = _float3(-1.484999f, -0.020000f, -1.824999f);
		static _float3 RotFloat3 = _float3(1, 1, 1);
		static _float value = 0.01f;
		static _int kind = 0;



		CGameInstance* m_pInstance = g_pGameInstance;
		if (m_pInstance->Get_DIKeyState(DIK_UP) & DIS_Press)
		{
			testFloat3.z += value;

			string ttszLog = "//Pivot  : " + to_string(testFloat3.x) + "f , " + to_string(testFloat3.y) + "f , " + to_string(testFloat3.z) + "f , 1" + "\n" +
				"//size  : " + to_string(RotFloat3.x) + "f , " + to_string(RotFloat3.y) + "f , " + to_string(RotFloat3.z) + "f  " + "\n";

			wstring ttDebugLog;
			ttDebugLog.assign(ttszLog.begin(), ttszLog.end());

			OutputDebugStringW(ttDebugLog.c_str());
		}

		else if (m_pInstance->Get_DIKeyState(DIK_DOWN) & DIS_Press)
		{
			testFloat3.z -= value;
			string ttszLog = "//Pivot  : " + to_string(testFloat3.x) + "f , " + to_string(testFloat3.y) + "f , " + to_string(testFloat3.z) + "f , 1" + "\n" +
				"//size  : " + to_string(RotFloat3.x) + "f , " + to_string(RotFloat3.y) + "f , " + to_string(RotFloat3.z) + "f  " + "\n";

			wstring ttDebugLog;
			ttDebugLog.assign(ttszLog.begin(), ttszLog.end());

			OutputDebugStringW(ttDebugLog.c_str());



		}
		else if (m_pInstance->Get_DIKeyState(DIK_LEFT) & DIS_Press)
		{
			testFloat3.x -= value;
			string ttszLog = "//Pivot  : " + to_string(testFloat3.x) + "f , " + to_string(testFloat3.y) + "f , " + to_string(testFloat3.z) + "f , 1" + "\n" +
				"//size  : " + to_string(RotFloat3.x) + "f , " + to_string(RotFloat3.y) + "f , " + to_string(RotFloat3.z) + "f  " + "\n";

			wstring ttDebugLog;
			ttDebugLog.assign(ttszLog.begin(), ttszLog.end());

			OutputDebugStringW(ttDebugLog.c_str());



		}

		else if (m_pInstance->Get_DIKeyState(DIK_RIGHT) & DIS_Press)
		{
			testFloat3.x += value;
			string ttszLog = "//Pivot  : " + to_string(testFloat3.x) + "f , " + to_string(testFloat3.y) + "f , " + to_string(testFloat3.z) + "f , 1" + "\n" +
				"//size  : " + to_string(RotFloat3.x) + "f , " + to_string(RotFloat3.y) + "f , " + to_string(RotFloat3.z) + "f  " + "\n";

			wstring ttDebugLog;
			ttDebugLog.assign(ttszLog.begin(), ttszLog.end());

			OutputDebugStringW(ttDebugLog.c_str());



		}

		else if (m_pInstance->Get_DIKeyState(DIK_DELETE) & DIS_Press)
		{
			testFloat3.y += value;

			string ttszLog = "//Pivot  : " + to_string(testFloat3.x) + "f , " + to_string(testFloat3.y) + "f , " + to_string(testFloat3.z) + "f , 1" + "\n" +
				"//size  : " + to_string(RotFloat3.x) + "f , " + to_string(RotFloat3.y) + "f , " + to_string(RotFloat3.z) + "f  " + "\n";

			wstring ttDebugLog;
			ttDebugLog.assign(ttszLog.begin(), ttszLog.end());

			OutputDebugStringW(ttDebugLog.c_str());



		}

		else if (m_pInstance->Get_DIKeyState(DIK_END) & DIS_Press)
		{
			testFloat3.y -= value;
			string ttszLog = "//Pivot  : " + to_string(testFloat3.x) + "f , " + to_string(testFloat3.y) + "f , " + to_string(testFloat3.z) + "f , 1" + "\n" +
				"//size  : " + to_string(RotFloat3.x) + "f , " + to_string(RotFloat3.y) + "f , " + to_string(RotFloat3.z) + "f  " + "\n";

			wstring ttDebugLog;
			ttDebugLog.assign(ttszLog.begin(), ttszLog.end());

			OutputDebugStringW(ttDebugLog.c_str());



		}
		else if (m_pInstance->Get_DIKeyState(DIK_PGUP) & DIS_Press)
		{
			switch (kind)
			{
			case 0:
				RotFloat3.x += value;
				break;
			case 1:
				RotFloat3.y += value;
				break;
			case 2:
				RotFloat3.z += value;
				break;

			default:
				break;
			}


			string ttszLog = "//Pivot  : " + to_string(testFloat3.x) + "f , " + to_string(testFloat3.y) + "f , " + to_string(testFloat3.z) + "f , 1" + "\n" +
				"//size  : " + to_string(RotFloat3.x) + "f , " + to_string(RotFloat3.y) + "f , " + to_string(RotFloat3.z) + "f  " + "\n";

			wstring ttDebugLog;
			ttDebugLog.assign(ttszLog.begin(), ttszLog.end());

			OutputDebugStringW(ttDebugLog.c_str());



		}
		else if (m_pInstance->Get_DIKeyState(DIK_PGDN) & DIS_Press)
		{
			switch (kind)
			{
			case 0:
				RotFloat3.x -= value;
				break;
			case 1:
				RotFloat3.y -= value;
				break;
			case 2:
				RotFloat3.z -= value;
				break;

			default:
				break;
			}

			string ttszLog = "//Pivot  : " + to_string(testFloat3.x) + "f , " + to_string(testFloat3.y) + "f , " + to_string(testFloat3.z) + "f , 1" + "\n" +
				"//size  : " + to_string(RotFloat3.x) + "f , " + to_string(RotFloat3.y) + "f , " + to_string(RotFloat3.z) + "f  " + "\n";

			wstring ttDebugLog;
			ttDebugLog.assign(ttszLog.begin(), ttszLog.end());

			OutputDebugStringW(ttDebugLog.c_str());



		}
		else if (m_pInstance->Get_DIKeyState(DIK_TAB) & DIS_Down)
		{
			kind++;
			if (kind > 2)kind = 0;

			string ttszLog = "kind  : " + to_string(kind) + "\n";

			wstring ttDebugLog;
			ttDebugLog.assign(ttszLog.begin(), ttszLog.end());

			OutputDebugStringW(ttDebugLog.c_str());
		}


		//_Matrix tt = XMMatrixRotationX(XMConvertToRadians(RotFloat3.x))*
		//	XMMatrixRotationY(XMConvertToRadians(RotFloat3.y))*
		//	XMMatrixRotationZ(XMConvertToRadians(RotFloat3.z))*
		//	XMMatrixTranslation(testFloat3.x, testFloat3.y, testFloat3.z);

		//m_pTransformCom->Set_Matrix(tt);
#define TestColliderNum 0
		_Matrix tt = XMMatrixScaling(RotFloat3.x, RotFloat3.y, RotFloat3.z) *XMMatrixTranslation(testFloat3.x, testFloat3.y, testFloat3.z);


		_Matrix			TransformMatrix = XMLoadFloat4x4(m_ArrCollisionAttach[TestColliderNum].pUpdatedNodeMat) * XMLoadFloat4x4(m_ArrCollisionAttach[TestColliderNum].pDefaultPivotMat);

		TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
		TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
		TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);


		TransformMatrix = tt*TransformMatrix * m_pTransformCom->Get_WorldMatrix();
		m_pColliderCom->Update_Transform(TestColliderNum, TransformMatrix);


	}






	Update_DmgCalculate(fDeltaTime);

	m_bIsOnScreen = g_pGameInstance->IsNeedToRender(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS));


	FAILED_CHECK(m_pModel->Update_AnimationClip(fDeltaTime, m_bIsOnScreen));
	//FAILED_CHECK(Adjust_MovedTransform_byAnim(fDeltaTime));

	if (m_bIsOnScreen)
	{
		for (_uint i = 0; i < TestColliderNum; i++)
		{
			_Matrix			TransformMatrix = XMLoadFloat4x4(m_ArrCollisionAttach[i].pUpdatedNodeMat) * XMLoadFloat4x4(m_ArrCollisionAttach[i].pDefaultPivotMat);
			TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
			TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
			TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);
			m_pColliderCom->Update_Transform(i, TransformMatrix * m_pTransformCom->Get_WorldMatrix());
		}



		g_pGameInstance->Add_CollisionGroup(CollisionType_Monster, this, m_pColliderCom);
	}



	//m_bIsOnScreen = true;
	//FAILED_CHECK(m_pModel->Update_AnimationClip(fDeltaTime , m_bIsOnScreen));
	//FAILED_CHECK(Adjust_AnimMovedTransform(fDeltaTime));
	//
	//for (_uint i = 0; i < m_pHanddyIndex; i++)
	//	m_vecWeapon[i]->Update(fDeltaTime);
	




	return _int();
}

_int CDollMaker::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)return -1;

	//FAILED_CHECK(Set_Monster_On_Terrain(m_pTransformCom, fDeltaTime));


	if (m_bIsOnScreen)
	{
		FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));

		for (_uint i = 0; i < m_pHanddyIndex; i++)
			m_vecWeapon[i]->Update(fDeltaTime);
		//m_vecWeapon[0]->LateUpdate(fDeltaTime);
	}

	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
	return _int();
}

_int CDollMaker::Render()
{
	if (__super::Render() < 0)		return -1;


	NULL_CHECK_RETURN(m_pModel, E_FAIL);

//#ifdef _DEBUG
//	m_pColliderCom->Render();
//#endif // _DEBUG

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
	return _int();
}

_int CDollMaker::LateRender()
{
	return _int();
}

_int CDollMaker::Update_DmgCalculate(_double fDeltaTime)
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


	_uint NowAnimIndex = m_pModel->Get_NowAnimIndex();


	if (!m_bIsDmgAnimUpdated[0] && 5 < m_fDmgAmount && NowAnimIndex >= 1 && NowAnimIndex <=3 )
	{

		m_pModel->Change_AnimIndex_ReturnTo_Must(19, 4, 0.15, true);
		m_bIsDmgAnimUpdated[0] = true;
	}
	if (!m_bIsDmgAnimUpdated[0] && 5 < m_fDmgAmount && NowAnimIndex >= 5 && NowAnimIndex <= 6)
	{
		m_pModel->Change_AnimIndex_ReturnTo_Must(20, 8, 0.15, true);
		m_bIsDmgAnimUpdated[0] = true;
	}



	return 0;
}

_int CDollMaker::Update_Pattern(_double fDeltaTime)
{

	m_PatternPassedTime += fDeltaTime;




	


	return _int();
}

void CDollMaker::Add_Dmg_to_Monster(_float iDmgAmount)
{
	m_DmgPassedTime = MonsterDmgTime;
	m_fDmgAmount += iDmgAmount;
	m_fHP -= iDmgAmount;

	if (m_fHP < m_fMaxHP * 0.8f && m_pHanddyIndex == 0)
	{
		m_pHanddyIndex = 1;
		m_pModel->Change_AnimIndex_UntilNReturn_Must(9, 12, 0, 0.15, true);
	}

	if (m_fHP < m_fMaxHP * 0.6f && m_pHanddyIndex == 1)
	{
		m_pHanddyIndex = 2;		
		m_pModel->Change_AnimIndex_UntilNReturn_Must(14, 17, 0, 0.15, true);

	}
}

HRESULT CDollMaker::SetUp_Components()
{

	CTransform::TRANSFORMDESC tDesc = {};
	tDesc.fMovePerSec = 2.8f;
	tDesc.fRotationPerSec = XMConvertToRadians(60);
	tDesc.fScalingPerSec = 1;
	tDesc.vPivot = _float3(0, 0, 0);

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom, &tDesc));
	__super::SetUp_WanderLook(m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK));


	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(Prototype_Mesh_DollMaker), TAG_COM(Com_Model), (CComponent**)&m_pModel));
	FAILED_CHECK(m_pModel->Change_AnimIndex(0));




	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Collider), TAG_COM(Com_Collider), (CComponent**)&m_pColliderCom));

	COLLIDERDESC			ColliderDesc;
	///* For.Com_AABB */
	//ZeroMemory(&ColliderDesc, sizeof(COLLIDERDESC));


	//Pivot  : -0.160000f , 0.000000f , -4.440007f , 1
	//size  : 7.600080f , 1.000000f , 1.000000f  
	ColliderDesc.vScale = _float3(1.);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(0,0,0, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));
	m_ArrCollisionAttach[0] = m_pModel->Find_AttachMatrix_InHirarchyNode("Bone_Tough_Long_04");
	NULL_CHECK_RETURN(m_ArrCollisionAttach[0].pDefaultPivotMat, E_FAIL);


	////Pivot  : 0.000000f , 0.310000f , -2.019999f , 1
	////size  : 2.039999f , 0.450001f , 1.180000f  
	////(x,z,y)
	////size  : 4.910018f , 1.699999f , 3.499998f  

	//ColliderDesc.vScale = _float3(3.499998f, 2.699999f, 4.910018f);
	//ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);	
	//ColliderDesc.vPosition = _float4(-0.160000f, 0.000000f, -4.440007f, 1);
	//FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_OBB, &ColliderDesc));
	//m_pColliderCom->Set_ParantBuffer();




	return S_OK;
}

HRESULT CDollMaker::SetUp_Weapon()
{

	CGameInstance* pInstance = GetSingle(CGameInstance);

	NULL_CHECK_RETURN(m_pModel, E_FAIL);
	NULL_CHECK_RETURN(m_pTransformCom, E_FAIL);



	CMonsterWeapon* pWeapon = nullptr;
	FAILED_CHECK(pInstance->Add_GameObject_Out_of_Manager((CGameObject**)&pWeapon, m_eNowSceneNum, TAG_OP(Prototype_HandyBoy), &_float3(76, 10, 63)));
	NULL_CHECK_RETURN(pWeapon, E_FAIL);
	m_vecWeapon.push_back(pWeapon);

	pWeapon = nullptr;
	FAILED_CHECK(pInstance->Add_GameObject_Out_of_Manager((CGameObject**)&pWeapon, m_eNowSceneNum, TAG_OP(Prototype_HandyGirl), &_float3(67, 10, 78)));
	NULL_CHECK_RETURN(pWeapon, E_FAIL);
	m_vecWeapon.push_back(pWeapon);

	return S_OK;
}

HRESULT CDollMaker::Set_Monster_On_Terrain(CTransform * pTransform, _double fDeltaTime)
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

		if (m_bIsJumping && m_pModel->Get_NowAnimIndex() != 4 && m_pModel->Get_NowAnimIndex() != 5)
		{
			m_bIsJumping = false;
			m_PatternDelayTime = 3;
			m_pModel->Change_AnimIndex_ReturnTo_Must(7, 0, 0, true);
		}
	}

	return S_OK;
}

HRESULT CDollMaker::Adjust_AnimMovedTransform(_double fDeltatime)
{
	_uint iNowAnimIndex = m_pModel->Get_NowAnimIndex();
	_double PlayRate = m_pModel->Get_PlayRate();

	if (iNowAnimIndex != m_iOldAnimIndex || PlayRate > 0.95)
		m_iAdjMovedIndex = 0;


	if (PlayRate <= 0.95)
	{
		switch (iNowAnimIndex)
		{
		case 0:
			break;
	
		default:
			break;
		}
	}
	else
	{
		switch (iNowAnimIndex)
		{
		case 4:
			m_pModel->Change_AnimIndex(0, 0.15f, true);
			break;

		case 8:
			m_pModel->Change_AnimIndex(0, 0.15f, true);
			break;

		default:
			break;
		}

	}





	m_iOldAnimIndex = iNowAnimIndex;
	return S_OK;
}

CDollMaker * CDollMaker::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CDollMaker*	pInstance = new CDollMaker(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CDollMaker");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CDollMaker::Clone(void * pArg)
{
	CDollMaker*	pInstance = new CDollMaker(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CDollMaker");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDollMaker::Free()
{
	__super::Free();


	for (auto& pWeapon : m_vecWeapon)
		Safe_Release(pWeapon);
	

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModel);
	Safe_Release(m_pColliderCom);

}
