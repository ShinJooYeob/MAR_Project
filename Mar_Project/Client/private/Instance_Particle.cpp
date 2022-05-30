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

	if (pArg != nullptr)
	{
		memcpy(&m_tDesc, pArg, sizeof(INSTPARTICLEDESC));
	}


	FAILED_CHECK(SetUp_Components());
	FAILED_CHECK(SetUp_ParticleAttribute());

	m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, m_tDesc.vWorldPosition);

	return S_OK;
}

_int CInstance_Particle::Update(_double TimeDelta)
{
	if (0 > __super::Update(TimeDelta))
		return -1;

	//m_PassedTime += TimeDelta;

	//if (m_PassedTime > 15)
	//{
	//	Set_IsDead();
	//	return 0 ;
	//}



	return _int();
}

_int CInstance_Particle::LateUpdate(_double TimeDelta)
{
	if (0 > __super::LateUpdate(TimeDelta))
		return -1;




	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND_NOLIGHT, this);
	return _int();
}

_int CInstance_Particle::Render()
{
	NULL_CHECK_RETURN(m_pVIBufferCom, E_FAIL);
	
	FAILED_CHECK(Update_ParticleAttribute(g_fDeltaTime));
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


		((VTXINSTMAT*)SubResource.pData)[i].vColor = m_vecParticleAttribute[i]._color;
		((VTXINSTMAT*)SubResource.pData)[i].vColor.w = 1;
	}

	m_pVIBufferCom->UnLock();


	FAILED_CHECK(SetUp_ConstantTable());

	FAILED_CHECK(m_pVIBufferCom->Render(m_pShaderCom, 0));

	return _int();
}

_int CInstance_Particle::LightRender()
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
		vTrans = XMVectorSetY(pUtil->RandomFloat3(-0.3f, 0.3f).XMVector(),0.f);


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


//#define ColorRange 0.3f, 0.8f

		//switch (rand() % 7)
		//{
		//case 0:
		//	tDesc._targetcolor = _float4(pUtil->RandomFloat(ColorRange), 0, 0, 1.f);
		//	break;
		//case 1:
		//	tDesc._targetcolor = _float4(0, pUtil->RandomFloat(ColorRange), 0, 1.f);
		//	break;
		//case 2:
		//	tDesc._targetcolor = _float4(0, 0, pUtil->RandomFloat(ColorRange), 1.f);
		//	break;
		//case 3:
		//	tDesc._targetcolor = _float4(0, pUtil->RandomFloat(ColorRange), pUtil->RandomFloat(ColorRange), 1.f);
		//	break;
		//case 4:
		//	tDesc._targetcolor = _float4(pUtil->RandomFloat(ColorRange), 0, pUtil->RandomFloat(ColorRange), 1.f);
		//	break;
		//case 5:
		//	tDesc._targetcolor = _float4(pUtil->RandomFloat(ColorRange), pUtil->RandomFloat(ColorRange), 0, 1.f);
		//	break;
		//case 6:
		//	tDesc._targetcolor = _float4(pUtil->RandomFloat3(ColorRange), 1.f);
		//	break;

		//default:
		//	__debugbreak();
		//	break;
		//}


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
			m_vecParticleAttribute[i]._NowForce = pInstance->Easing(TYPE_SinIn, m_vecParticleAttribute[i]._Defaultforce, 0, (_float)m_vecParticleAttribute[i]._age, (_float)m_vecParticleAttribute[i]._lifeTime);
			
			if (m_vecParticleAttribute[i]._age < m_vecParticleAttribute[i]._lifeTime * m_tDesc.SizeChangingEndRate)
			{
				m_vecParticleAttribute[i]._size = pInstance->Easing_Vector(TYPE_ExpoOut, m_tDesc.vStartSize, m_tDesc.vTargetSize,
					(_float)m_vecParticleAttribute[i]._age, (_float)m_vecParticleAttribute[i]._lifeTime * 0.7f);

			}

			FAILED_CHECK(Update_ColorChange(&(m_vecParticleAttribute[i]),fDeltaTime));

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

	if (iDeadCount >= m_iNumInstance - 2)
	{
		Set_IsDead();
	}


	return S_OK;
}

HRESULT CInstance_Particle::Update_ColorChange(INSTPARTICLEATT * tParticleAtt, _double fDeltaTime)
{
	CGameInstance* pInstance = GetSingle(CGameInstance);


	_double TimeInterver = tParticleAtt->_lifeTime / m_tDesc.ColorChangeFrequency; //=> 15.초 파티클의 2번 진동수면 7.5가 나오는 것
	_uint iFrequencyIndex = _uint(tParticleAtt->_age / TimeInterver); //=> 7.5초 주기에 3.4초를 지나고있으면 0번째 10초를 지나고있으면 1번째를 의미함
	_double FrequencyAge = tParticleAtt->_age - (TimeInterver * iFrequencyIndex); // =>7.5초 주기에 3.4초를 지나고있으면 3.4 10초를 지나고있으면 2.5

	if (iFrequencyIndex % 2)
	{
		//_float3(1, 0.64313725f, 0.141176470f);
		tParticleAtt->_color = pInstance->Easing_Vector(TYPE_Linear, m_tDesc.vTargetColor, m_tDesc.vStartColor, _float(FrequencyAge), _float(TimeInterver));
	}
	else
	{
		tParticleAtt->_color = pInstance->Easing_Vector(TYPE_Linear, m_tDesc.vStartColor, m_tDesc.vTargetColor,  _float(FrequencyAge), _float(TimeInterver));

	}

	tParticleAtt->_color.x = min(max(tParticleAtt->_color.x, 0), 1);
	tParticleAtt->_color.y = min(max(tParticleAtt->_color.y, 0), 1);
	tParticleAtt->_color.z = min(max(tParticleAtt->_color.z, 0), 1);


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

	m_vecParticleAttribute.clear();
	
}
