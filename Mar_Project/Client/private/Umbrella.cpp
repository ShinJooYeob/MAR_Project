#include "stdafx.h"
#include "..\public\Umbrella.h"




CUmbrella::CUmbrella(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CWeapon(pDevice,pDeviceContext)
{
}

CUmbrella::CUmbrella(const CUmbrella & rhs)
	: CWeapon(rhs)
{
}

HRESULT CUmbrella::Initialize_Prototype(void * pArg)
{
	__super::Initialize_Prototype(pArg);

	return S_OK;
}

HRESULT CUmbrella::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));


	FAILED_CHECK(SetUp_Components());

	
	return S_OK;
}

_int CUmbrella::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;
	if (m_bIsDead) return 0;
	

	{

	//Rot: -22.039982, 212.210632, 71.058846,

	//Pivot: -0.688000, 0.033000, -1.294000,


		static _float3 testFloat3 = _float3(-0.688000, 0.033000, -1.294000);
		static _float3 RotFloat3 = _float3(-22.039982, 212.210632, 71.058846);
		static _float value = 0.013f;
		static _int kind = 0;
		_Matrix tt = XMMatrixRotationX(XMConvertToRadians(RotFloat3.x))*
			XMMatrixRotationY(XMConvertToRadians(RotFloat3.y))*
			XMMatrixRotationZ(XMConvertToRadians(RotFloat3.z));

		tt.r[3] = XMVectorSetW(testFloat3.XMVector(), 1);
		m_pTransformCom->Set_Matrix(tt);
		CGameInstance* m_pInstance = g_pGameInstance;
		if (m_pInstance->Get_DIKeyState(DIK_UP) & DIS_Down)
		{
			testFloat3.x -= value;


			_Matrix tt = XMMatrixRotationX(XMConvertToRadians(RotFloat3.x))*
				XMMatrixRotationY(XMConvertToRadians(RotFloat3.y))*
				XMMatrixRotationZ(XMConvertToRadians(RotFloat3.z));

			tt.r[3] = XMVectorSetW(testFloat3.XMVector(), 1);
			m_pTransformCom->Set_Matrix(tt);
			string ttszLog = "Pivot  : " + to_string(testFloat3.x) + " , " + to_string(testFloat3.y) + " , " + to_string(testFloat3.z) + " , " + "\n";

			wstring ttDebugLog;
			ttDebugLog.assign(ttszLog.begin(), ttszLog.end());

			OutputDebugStringW(ttDebugLog.c_str());
		}

		else if (m_pInstance->Get_DIKeyState(DIK_DOWN) & DIS_Down)
		{
			testFloat3.x += value;

			_Matrix tt = XMMatrixRotationX(XMConvertToRadians(RotFloat3.x))*
				XMMatrixRotationY(XMConvertToRadians(RotFloat3.y))*
				XMMatrixRotationZ(XMConvertToRadians(RotFloat3.z));

			tt.r[3] = XMVectorSetW(testFloat3.XMVector(), 1);
			m_pTransformCom->Set_Matrix(tt);
			string ttszLog = "Pivot  : " + to_string(testFloat3.x) + " , " + to_string(testFloat3.y) + " , " + to_string(testFloat3.z) + " , " + "\n";

			wstring ttDebugLog;
			ttDebugLog.assign(ttszLog.begin(), ttszLog.end());

			OutputDebugStringW(ttDebugLog.c_str());



		}
		else if (m_pInstance->Get_DIKeyState(DIK_LEFT) & DIS_Down)
		{
			testFloat3.z += value;

			_Matrix tt = XMMatrixRotationX(XMConvertToRadians(RotFloat3.x))*
				XMMatrixRotationY(XMConvertToRadians(RotFloat3.y))*
				XMMatrixRotationZ(XMConvertToRadians(RotFloat3.z));

			tt.r[3] = XMVectorSetW(testFloat3.XMVector(), 1);
			m_pTransformCom->Set_Matrix(tt);
			string ttszLog = "Pivot  : " + to_string(testFloat3.x) + " , " + to_string(testFloat3.y) + " , " + to_string(testFloat3.z) + " , " + "\n";

			wstring ttDebugLog;
			ttDebugLog.assign(ttszLog.begin(), ttszLog.end());

			OutputDebugStringW(ttDebugLog.c_str());



		}

		else if (m_pInstance->Get_DIKeyState(DIK_RIGHT) & DIS_Down)
		{
			testFloat3.z -= value;

			_Matrix tt = XMMatrixRotationX(XMConvertToRadians(RotFloat3.x))*
				XMMatrixRotationY(XMConvertToRadians(RotFloat3.y))*
				XMMatrixRotationZ(XMConvertToRadians(RotFloat3.z));

			tt.r[3] = XMVectorSetW(testFloat3.XMVector(), 1);
			m_pTransformCom->Set_Matrix(tt);
			string ttszLog = "Pivot  : " + to_string(testFloat3.x) + " , " + to_string(testFloat3.y) + " , " + to_string(testFloat3.z) + " , " + "\n";

			wstring ttDebugLog;
			ttDebugLog.assign(ttszLog.begin(), ttszLog.end());

			OutputDebugStringW(ttDebugLog.c_str());



		}

		else if (m_pInstance->Get_DIKeyState(DIK_DELETE) & DIS_Down)
		{
			testFloat3.y += value;
			_Matrix tt = XMMatrixRotationX(XMConvertToRadians(RotFloat3.x))*
				XMMatrixRotationY(XMConvertToRadians(RotFloat3.y))*
				XMMatrixRotationZ(XMConvertToRadians(RotFloat3.z));

			tt.r[3] = XMVectorSetW(testFloat3.XMVector(), 1);
			m_pTransformCom->Set_Matrix(tt);
			string ttszLog = "Pivot  : " + to_string(testFloat3.x) + " , " + to_string(testFloat3.y) + " , " + to_string(testFloat3.z) + " , " + "\n";

			wstring ttDebugLog;
			ttDebugLog.assign(ttszLog.begin(), ttszLog.end());

			OutputDebugStringW(ttDebugLog.c_str());



		}

		else if (m_pInstance->Get_DIKeyState(DIK_END) & DIS_Down)
		{
			testFloat3.y -= value;
			_Matrix tt = XMMatrixRotationX(XMConvertToRadians(RotFloat3.x))*
				XMMatrixRotationY(XMConvertToRadians(RotFloat3.y))*
				XMMatrixRotationZ(XMConvertToRadians(RotFloat3.z));

			tt.r[3] = XMVectorSetW(testFloat3.XMVector(), 1);
			m_pTransformCom->Set_Matrix(tt);
			string ttszLog = "Pivot  : " + to_string(testFloat3.x) + " , " + to_string(testFloat3.y) + " , " + to_string(testFloat3.z) + " , " + "\n";

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

			_Matrix tt = XMMatrixRotationX(XMConvertToRadians(RotFloat3.x))*
				XMMatrixRotationY(XMConvertToRadians(RotFloat3.y))*
				XMMatrixRotationZ(XMConvertToRadians(RotFloat3.z));

			tt.r[3] = XMVectorSetW(testFloat3.XMVector(), 1);
			m_pTransformCom->Set_Matrix(tt);

			string ttszLog = "rot  : " + to_string(RotFloat3.x) + " , " + to_string(RotFloat3.y) + " , " + to_string(RotFloat3.z) + " , " + "\n";

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

			_Matrix tt = XMMatrixRotationX(XMConvertToRadians(RotFloat3.x))*
				XMMatrixRotationY(XMConvertToRadians(RotFloat3.y))*
				XMMatrixRotationZ(XMConvertToRadians(RotFloat3.z));

			tt.r[3] = XMVectorSetW(testFloat3.XMVector(), 1);
			m_pTransformCom->Set_Matrix(tt);

			string ttszLog = "Rot  : " + to_string(RotFloat3.x) + " , " + to_string(RotFloat3.y) + " , " + to_string(RotFloat3.z) + " , " + "\n";

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

	}



	FAILED_CHECK(m_pModel->Update_AnimationClip(fDeltaTime, m_bIsOnScreen));


	_Matrix			TransformMatrix = XMLoadFloat4x4(m_tATBMat.pUpdatedNodeMat) * XMLoadFloat4x4(m_tATBMat.pDefaultPivotMat);

	TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
	TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
	TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);

	m_BoneMatrix = TransformMatrix = m_pTransformCom->Get_WorldMatrix()* TransformMatrix * m_tWeaponDesc.pParantTransform->Get_WorldMatrix();


	for (_uint i = 0; i < m_pColliderCom->Get_NumColliderBuffer(); i++)
		m_pColliderCom->Update_Transform(i, TransformMatrix);

	m_bIsOnScreen = g_pGameInstance->IsNeedToRender(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS));

	return _int();
}

_int CUmbrella::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;

	if (m_bIsDead) return 0;


	//if (m_bIsOnScreen)	
		FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));
	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
	return _int();
}

_int CUmbrella::Render()
{

	NULL_CHECK_RETURN(m_pModel, E_FAIL);

#ifdef _DEBUG
	m_pColliderCom->Render();
#endif // _DEBUG



	_float4x4 ShaderMat = m_BoneMatrix.TransposeXMatrix();
	m_pShaderCom->Set_RawValue("g_AttechMatrix", &ShaderMat, sizeof(_float4x4));



	CGameInstance* pInstance = GetSingle(CGameInstance);

	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_VIEW), sizeof(_float4x4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_PROJ), sizeof(_float4x4)));


	_uint NumMaterial = m_pModel->Get_NumMaterial();

	for (_uint i = 0; i < NumMaterial; i++)
	{

		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
			FAILED_CHECK(m_pModel->Bind_OnShader(m_pShaderCom, i, j, MODLETEXTYPE(j)));

		FAILED_CHECK(m_pModel->Render(m_pShaderCom, 6, i, "g_BoneMatrices"));
	}


	return _int();
}

_int CUmbrella::LateRender()
{

	return _int();
}

HRESULT CUmbrella::SetUp_Components()
{
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Mesh_Umbrella), TAG_COM(Com_Model), (CComponent**)&m_pModel));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Collider), TAG_COM(Com_Collider), (CComponent**)&m_pColliderCom));

	COLLIDERDESC			ColliderDesc;
	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(COLLIDERDESC));

	ColliderDesc.vScale = _float3(1.4f, 1.4f, 1.4f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);	
	ColliderDesc.vPosition = _float4(-0.85f, 0.08f, -1.305f, 1.f);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));

	ColliderDesc.vScale = _float3(0.3f, 0.3f, 0.3f);
	ColliderDesc.vPosition = _float4(-0.61f, -0.01f, -1.32f, 1.f);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));
	m_pColliderCom->Set_ParantBuffer();

	ColliderDesc.vScale = _float3(0.5f, 0.5f, 0.5f);
	ColliderDesc.vPosition = _float4(-0.85f, 0.08f, -1.305f, 1.f);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));
	m_pColliderCom->Set_ParantBuffer();


	ColliderDesc.vScale = _float3(0.8f, 0.8f, 0.8f);
	ColliderDesc.vPosition = _float4(0,0,0, 1.f);
	//-0.180000, 0.210000, 0.150000,
	ColliderDesc.vPosition = _float4(-1.03f, 0.29f, -1.155f, 1.f);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));
	m_pColliderCom->Set_ParantBuffer();

	//Pivot: -0.180000, 0.120000, 0.030000,
	//Pivot: -0.480000, 0.180000, 0.030000,


	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom));


	return S_OK;
}

CUmbrella * CUmbrella::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUmbrella*	pInstance = new CUmbrella(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(nullptr)))
	{
		MSGBOX("Failed to Created CUmbrella");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUmbrella::Clone(void * pArg)
{
	CUmbrella*	pInstance = new CUmbrella(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CUmbrella");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUmbrella::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModel);
	Safe_Release(m_pColliderCom);
	
}
