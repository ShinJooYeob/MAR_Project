#include "stdafx.h"
#include "..\public\Instance_Particle.h"


CInstance_Particle::CInstance_Particle(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CInstance_Particle::CInstance_Particle(const CInstance_Particle & rhs)
	: CGameObject(rhs)
{
}

HRESULT CInstance_Particle::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype(nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT CInstance_Particle::Initialize_Clone(void * pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	FAILED_CHECK(SetUp_Components());
	FAILED_CHECK(SetUp_ParticleAttribute());

	if (pArg != nullptr)
	{
		_float3 vPos = *((_float3*)pArg);
		m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, vPos);
	}

	return S_OK;
}

_int CInstance_Particle::Update(_double TimeDelta)
{
	if (0 > __super::Update(TimeDelta))
		return -1;

	m_PassedTime += TimeDelta;

	if (m_PassedTime > 15)
	{
		Set_IsDead();
		return 0 ;
	}


	FAILED_CHECK(Update_ParticleAttribute(TimeDelta));


	D3D11_MAPPED_SUBRESOURCE SubResource;
	CUtilityMgr* pUtil = GetSingle(CUtilityMgr);

	FAILED_CHECK(m_pVIBufferCom->Lock(&SubResource));



	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		//
		XMStoreFloat4(&(((VTXINSTMAT*)SubResource.pData)[i].vRight), ((_float3*)(&m_vecParticleAttribute[i]._LocalMatirx.m[0]))->XMVector() * m_vecParticleAttribute[i]._size.x);
		XMStoreFloat4(&(((VTXINSTMAT*)SubResource.pData)[i].vUp), ((_float3*)(&m_vecParticleAttribute[i]._LocalMatirx.m[1]))->XMVector()* m_vecParticleAttribute[i]._size.y);
		XMStoreFloat4(&(((VTXINSTMAT*)SubResource.pData)[i].vLook), ((_float3*)(&m_vecParticleAttribute[i]._LocalMatirx.m[2]))->XMVector()* m_vecParticleAttribute[i]._size.z);
		XMStoreFloat4(&(((VTXINSTMAT*)SubResource.pData)[i].vTranslation), ((_float3*)(&m_vecParticleAttribute[i]._LocalMatirx.m[3]))->XMVector());
		((VTXINSTMAT*)SubResource.pData)[i].vTranslation.w = 1;
	}



	m_pVIBufferCom->UnLock();
	return _int();
}

_int CInstance_Particle::LateUpdate(_double TimeDelta)
{
	if (0 > __super::LateUpdate(TimeDelta))
		return -1;




	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
	return _int();
}

_int CInstance_Particle::Render()
{

	NULL_CHECK_RETURN(m_pVIBufferCom, E_FAIL);

	FAILED_CHECK(SetUp_ConstantTable());

	FAILED_CHECK(m_pVIBufferCom->Render(m_pShaderCom, 0));

	return _int();
}

_int CInstance_Particle::LateRender()
{
	return _int();
}

HRESULT CInstance_Particle::SetUp_Components()
{

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VTXPOINTINST), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_VIBuffer_Point_Instance), TAG_COM(Com_VIBuffer), (CComponent**)&m_pVIBufferCom));

	m_iNumInstance = m_pVIBufferCom->Get_NumInstance();

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Texture_InstanceParticleTex), TAG_COM(Com_Texture), (CComponent**)&m_pTextureCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom));


	return S_OK;
}

HRESULT CInstance_Particle::SetUp_ParticleAttribute()
{
	m_vecParticleAttribute.reserve(m_iNumInstance);

	CUtilityMgr* pUtil = GetSingle(CUtilityMgr);

	for (_uint i = 0 ; i < m_iNumInstance; i++)
	{
		INSTPARTICLEATT tDesc;

		_Vector vRight, vUp, vLook, vTrans;

		vUp = pUtil->RandomFloat3(-999, 999).Get_Nomalize();
		vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0, 1, 0, 0), vUp));
		vLook = XMVector3Normalize(XMVector3Cross(vRight, vUp));
		vTrans = XMVectorSetY(pUtil->RandomFloat3(-0.3f, 0.3f).XMVector(),1.0f);


		tDesc._LocalMatirx = XMMatrixIdentity();
		XMStoreFloat4(((_float4*)(&tDesc._LocalMatirx.m[0])), vRight);
		XMStoreFloat4(((_float4*)(&tDesc._LocalMatirx.m[1])), vUp);
		XMStoreFloat4(((_float4*)(&tDesc._LocalMatirx.m[2])), vLook);
		XMStoreFloat4(((_float4*)(&tDesc._LocalMatirx.m[3])), vTrans);

		//tDesc._position = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS) + pUtil->RandomFloat3(-5, 5).XMVector();
		tDesc._NowForce = tDesc._Defaultforce = pUtil->RandomFloat(0.05f, 0.08f);

		tDesc._velocity = vUp;
		tDesc._age = 0;
		tDesc._isAlive = true;

		tDesc._lifeTime = pUtil->RandomFloat(1, 2.5f);
		tDesc._size = _float3(0.5f, 2.f, 1);
		

		m_vecParticleAttribute.push_back(tDesc);
	}


	return S_OK;
}

HRESULT CInstance_Particle::SetUp_ConstantTable()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	FAILED_CHECK(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));

	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_Transform_Float4x4_TP(PLM_VIEW), sizeof(_float4x4)));

	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_Transform_Float4x4_TP(PLM_PROJ), sizeof(_float4x4)));

	FAILED_CHECK(m_pTextureCom->Bind_OnShader(m_pShaderCom, "g_DiffuseTexture"));

	_float4		vCamPosition = pGameInstance->Get_TargetPostion_float4(PLV_CAMERA);


	m_pShaderCom->Set_RawValue("g_vCamPosition", &vCamPosition, sizeof(_float4));

	RELEASE_INSTANCE(CGameInstance);


	return S_OK;
}

HRESULT CInstance_Particle::Update_ParticleAttribute(_double fDeltaTime)
{

	CGameInstance* pInstance = g_pGameInstance;

	_uint iDeadCount = 0;

	for (_uint i = 0 ; i< m_iNumInstance; i++)
	{
		m_vecParticleAttribute[i]._age += fDeltaTime;

		if (m_vecParticleAttribute[i]._age < m_vecParticleAttribute[i]._lifeTime)
		{
			m_vecParticleAttribute[i]._NowForce = pInstance->Easing(TYPE_Linear, m_vecParticleAttribute[i]._Defaultforce, 0, (_float)m_vecParticleAttribute[i]._age, (_float)m_vecParticleAttribute[i]._lifeTime);
			m_vecParticleAttribute[i]._size = pInstance->Easing_Vector(TYPE_ExpoOut, _float3(0.05f, 0.2f, 1), _float3(0.01f), (_float)m_vecParticleAttribute[i]._age, (_float)m_vecParticleAttribute[i]._lifeTime);

			_float3 NewPos = *((_float3*)(&m_vecParticleAttribute[i]._LocalMatirx.m[3]));
			/*if (NewPos.Get_Lenth() < 7)
			{*/
				NewPos = NewPos.XMVector() + m_vecParticleAttribute[i]._velocity.XMVector() * m_vecParticleAttribute[i]._NowForce
					+ XMVectorSet(0, m_vecParticleAttribute[i]._Defaultforce,0,0) *_float((m_vecParticleAttribute[i]._lifeTime * 0.5f - m_vecParticleAttribute[i]._age )/ m_vecParticleAttribute[i]._lifeTime);
				memcpy(&m_vecParticleAttribute[i]._LocalMatirx.m[3], &NewPos, sizeof(_float3));
			//}
		}
		else
		{
			iDeadCount++;
			m_vecParticleAttribute[i]._isAlive = false;
			m_vecParticleAttribute[i]._size = _float3(0);
		}
	}

	if (iDeadCount == m_iNumInstance - 1)
	{
		Set_IsDead();
	}


	return S_OK;
}

CInstance_Particle * CInstance_Particle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CInstance_Particle*	pInstance = new CInstance_Particle(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSGBOX("Failed to Created CInstance_Particle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CInstance_Particle::Clone(void * pArg)
{
	CInstance_Particle*	pInstance = new CInstance_Particle(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CInstance_Particle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CInstance_Particle::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTransformCom);
	
}
