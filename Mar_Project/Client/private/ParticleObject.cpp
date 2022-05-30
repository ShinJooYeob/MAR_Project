#include "stdafx.h"
#include "..\public\ParticleObject.h"



CParticleObject::CParticleObject(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CParticleObject::CParticleObject(const CParticleObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT CParticleObject::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));

	m_ParticleList.clear();

	return S_OK;
}

HRESULT CParticleObject::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));


	FAILED_CHECK(SetUp_ParticleDesc(pArg));

	FAILED_CHECK(SetUp_Components());


	FAILED_CHECK(Initialize_Child_Clone());


	Safe_AddRef(m_ParticleDesc.FollowingTarget);

	return S_OK;
}


_int CParticleObject::Update(_double fTimeDelta)
{
	if (0 > __super::Update(fTimeDelta)) return -1;

	m_PassedLifeTime += fTimeDelta;



	if (m_ParticleDesc.FollowingTarget && m_ParticleDesc.FollowingTarget->Get_IsOwnerDead())
	{
		this->Set_IsDead();
		return 0;
	}

	std::list<PARTICLEATT>::iterator iter;

	if (m_PassedLifeTime < m_ParticleDesc.TotalParticleTime)
	{
		for (iter = m_ParticleList.begin(); iter != m_ParticleList.end(); iter++)
		{
			iter->_age += (fTimeDelta);
			//iter->_NowparantPos = vParentPos;



			Update_Position_by_Velocity(&(*iter), fTimeDelta);


			if ((iter->_position).Get_Distance(iter->_NowparantPos.XMVector() + m_vPivot.XMVector()) > m_ParticleDesc.MaxBoundaryRadius 
				|| (iter->_age > iter->_lifeTime))
				ResetParticle(&(*iter));

			if (m_ParticleDesc.AlphaBlendON)
				iter->_CamDist = iter->_position.Get_Distance(g_pGameInstance->Get_TargetPostion_Vector(PLV_CAMERA));

			if (m_ParticleDesc.ColorChageFrequency)
				Update_ColorChange(&(*iter), fTimeDelta);

			if (m_ParticleDesc.SizeChageFrequency)
				Update_SizeChange(&(*iter), fTimeDelta);

			Update_TextureChange(&(*iter), fTimeDelta);
		}

	}
	else
	{
		for (iter = m_ParticleList.begin(); iter != m_ParticleList.end(); )
		{
			iter->_age += fTimeDelta;
			//iter->_NowparantPos = vParentPos;

			Update_Position_by_Velocity(&(*iter), fTimeDelta);


			if (m_ParticleDesc.AlphaBlendON)
				iter->_CamDist = iter->_position.Get_Distance(g_pGameInstance->Get_TargetPostion_Vector(PLV_CAMERA));

			if (m_ParticleDesc.ColorChageFrequency)
				Update_ColorChange(&(*iter), fTimeDelta);

			if (m_ParticleDesc.SizeChageFrequency)
				Update_SizeChange(&(*iter), fTimeDelta);

			Update_TextureChange(&(*iter), fTimeDelta);

			if ((iter->_position).Get_Distance(iter->_NowparantPos.XMVector() + m_vPivot.XMVector()) > m_ParticleDesc.MaxBoundaryRadius 
				|| (iter->_age > iter->_lifeTime))
				iter = m_ParticleList.erase(iter);
			else
				iter++;


		}


		if (m_ParticleList.size() == 0)
			this->Set_IsDead();
	}


	return _int();
}

_int CParticleObject::LateUpdate(_double fTimeDelta)
{
	if (0 > __super::LateUpdate(fTimeDelta))return -1;


	if (m_ParticleDesc.m_bIsUI)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	else
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_AFTEROBJ, this);

	return _int();
}

_int CParticleObject::Render()
{
	if (0 > __super::Render())	return -1;

	CGameInstance* pInstance = GetSingle(CGameInstance);


	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_VIEW), sizeof(_float4x4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_PROJ), sizeof(_float4x4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_PROJ), sizeof(_float4x4)));

	_float2 UVSize = _float2(1 / m_ParticleDesc.vTextureXYNum.x, 1 / m_ParticleDesc.vTextureXYNum.y);

	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vUVSize", &UVSize, sizeof(_float2)));

	if(m_ParticleDesc.m_fAlphaTestValue != 0.1f)
		FAILED_CHECK(m_pShaderCom->Set_RawValue("g_fAlphaTestValue", &m_ParticleDesc.m_fAlphaTestValue, sizeof(_float)));

	if (m_ParticleDesc.m_iPassIndex == 17)
	{
		FAILED_CHECK(m_pShaderCom->Set_Texture("g_DepthTexture", g_pGameInstance->Get_SRV(L"Target_Depth")));

	}
	

	_Matrix vViewMat = pInstance->Get_Transform_Matrix(PLM_VIEW);

	if (!m_ParticleList.empty())
	{
		if (m_ParticleDesc.AlphaBlendON)
		{
			m_ParticleList.sort([](PARTICLEATT tSrc, PARTICLEATT tDest) ->_bool
			{
				return tSrc._CamDist > tDest._CamDist;
			});
		}




		for (auto iter : m_ParticleList)
		{
			if (iter._isAlive)
			{
				//if (m_ParticleDesc.ColorChageFrequency)
				//	m_pGraphicDevice->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_XRGB(_uint(iter._color.x), _uint(iter._color.y), _uint(iter._color.z)));

				

				if (m_ParticleDesc.m_bIsUI && m_ParticleDesc.FollowingTarget == nullptr)
				{
					//_float3 UIPos = { 0,0,0 };
					//UIPos.x = iter._position.x - g_iWinCX *0.5f;
					//UIPos.y = -iter._position.y + g_iWinCY *0.5f;
					//m_pParticleTransformCom->Set_MatrixState(CTransform::STATE_POS, UIPos);
				}
				else {
					m_pParticleTransformCom->Set_Matrix(XMMatrixIdentity());
					m_pParticleTransformCom->Scaled_All(iter._size);

					m_pParticleTransformCom->Set_MatrixState(CTransform::STATE_POS, iter._position);
				}

				FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vUVPos", &iter._TextureUV, sizeof(_float2)));
				FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vColor", &iter._color, sizeof(_float4)));
				FAILED_CHECK(m_pTextureCom->Bind_OnShader(m_pShaderCom, "g_DiffuseTexture", iter._TextureIndex));
				FAILED_CHECK(m_pParticleTransformCom->Bind_OnShader_BillBoard(m_pShaderCom,"g_WorldMatrix", vViewMat));
				FAILED_CHECK(m_pVIBufferCom->Render(m_pShaderCom, m_ParticleDesc.m_iPassIndex));
			}

		}
	}

	return _int();
}

_int CParticleObject::LightRender()
{
	if (0 > __super::LightRender())	return -1;


	return _int();
}

void CParticleObject::ResetParticle(PARTICLEATT * attribute)
{
	_float3 RandomPos = _float3(0, 0, 0);

	CUtilityMgr* pUtil = GetSingle(CUtilityMgr);


	//랜덤 위치에 재생성
	RandomPos = pUtil->RandomFloat3(m_ParticleDesc.ParticleStartRandomPosMin, m_ParticleDesc.ParticleStartRandomPosMax);

	if (m_ParticleDesc.FollowingTarget == nullptr)
		RandomPos = RandomPos.XMVector()+ m_vPivot.XMVector() + m_ParticleDesc.FixedTarget.XMVector();
	else
		RandomPos = RandomPos.XMVector() + m_vPivot.XMVector() + m_ParticleDesc.FollowingTarget->Get_MatrixState(CTransform::STATE_POS);

	if (m_ParticleDesc.m_bIsUI)RandomPos.z = 0;

	attribute->_position = RandomPos;


	//랜덤한 라이프타임
	attribute->_lifeTime = m_ParticleDesc.EachParticleLifeTime * pUtil->RandomFloat(0.5, 1.5);
	attribute->_age = 0;

	attribute->_color = m_ParticleDesc.TargetColor;
	attribute->_Targetforce = attribute->_force = m_ParticleDesc.Particle_Power * pUtil->RandomFloat(m_ParticleDesc.PowerRandomRange.x, m_ParticleDesc.PowerRandomRange.y);
	attribute->_color = m_ParticleDesc.TargetColor;
	attribute->_size = m_ParticleDesc.ParticleSize;
	attribute->_TextureIndex = rand() % (m_ParticleDesc.iSimilarLayerNum);

	if (m_ParticleDesc.FollowingTarget == nullptr)
		attribute->_NowparantPos = m_ParticleDesc.FixedTarget.XMVector() + m_vPivot.XMVector();
	else
		attribute->_NowparantPos = m_ParticleDesc.FollowingTarget->Get_MatrixState(CTransform::STATE_POS) + m_vPivot.XMVector();

	//자식들의 Velocity를 초기화하는 과정
	Reset_Velocity(attribute->_velocity);
}

void CParticleObject::Update_SizeChange(PARTICLEATT * tParticleAtt, _double fTimeDelta)
{
	CGameInstance* pInstance = GetSingle(CGameInstance);


	if (m_ParticleDesc.SizeChageFrequency)
	{
		_double TimeInterver = tParticleAtt->_lifeTime / m_ParticleDesc.SizeChageFrequency; //=> 15.0초 파티클의 2번 진동수면 7.5가 나오는 것
		_uint iFrequencyIndex = _uint(tParticleAtt->_age / TimeInterver); //=> 7.5초 주기에 3.4초를 지나고있으면 0번째 10초를 지나고있으면 1번째를 의미함
		_double FrequencyAge = tParticleAtt->_age - (TimeInterver * iFrequencyIndex); // =>7.5초 주기에 3.4초를 지나고있으면 3.4 10초를 지나고있으면 2.5

		if (iFrequencyIndex % 2)
			tParticleAtt->_size = pInstance->Easing_Vector(TYPE_Linear, m_ParticleDesc.ParticleSize2, m_ParticleDesc.ParticleSize, _float(FrequencyAge), _float(TimeInterver));
		
		else
			tParticleAtt->_size = pInstance->Easing_Vector(TYPE_Linear, m_ParticleDesc.ParticleSize, m_ParticleDesc.ParticleSize2, _float(FrequencyAge), _float(TimeInterver));
		

		if (tParticleAtt->_size.x < 0) tParticleAtt->_size.x = 0;
		if (tParticleAtt->_size.y < 0) tParticleAtt->_size.y = 0;
		if (tParticleAtt->_size.z < 0) tParticleAtt->_size.z = 0;
	}

		
	

}

void CParticleObject::Update_TextureChange(PARTICLEATT * tParticleAtt, _double fTimeDelta)
{
	_uint iTotalTextureNum= _uint(m_ParticleDesc.vTextureXYNum.x * m_ParticleDesc.vTextureXYNum.y);

	if (m_ParticleDesc.TextureChageFrequency)
	{
		_double TimeInterver = tParticleAtt->_lifeTime / m_ParticleDesc.TextureChageFrequency; //=> 15.초 파티클의 2번 진동수면 7.5가 나오는 것
		_uint iFrequencyIndex = _uint(tParticleAtt->_age / TimeInterver); //=> 7.5초 주기에 3.4초를 지나고있으면 0번째 10초를 지나고있으면 1번째를 의미함
		_double FrequencyAge = tParticleAtt->_age - (TimeInterver * iFrequencyIndex); // =>7.5초 주기에 3.4초를 지나고있으면 3.4 10초를 지나고있으면 2.5



		_uint iNowIndex = _uint(FrequencyAge / TimeInterver * iTotalTextureNum);

		_uint fX = (iNowIndex % _uint(m_ParticleDesc.vTextureXYNum.x));
		_uint fY = (iNowIndex / _uint(m_ParticleDesc.vTextureXYNum.x));

		tParticleAtt->_TextureUV = _float2( _float(fX) / m_ParticleDesc.vTextureXYNum.x, _float(fY) / m_ParticleDesc.vTextureXYNum.y);
	}
	else
		tParticleAtt->_TextureUV = _float2(0, 0);
	

}

void CParticleObject::Update_ColorChange(PARTICLEATT * tParticleAtt, _double fTimeDelta)
{
	CGameInstance* pInstance = GetSingle(CGameInstance);


	if (m_ParticleDesc.ColorChageFrequency)
	{
		_double TimeInterver = tParticleAtt->_lifeTime / m_ParticleDesc.ColorChageFrequency; //=> 15.초 파티클의 2번 진동수면 7.5가 나오는 것
		_uint iFrequencyIndex = _uint(tParticleAtt->_age / TimeInterver); //=> 7.5초 주기에 3.4초를 지나고있으면 0번째 10초를 지나고있으면 1번째를 의미함
		_double FrequencyAge = tParticleAtt->_age - (TimeInterver * iFrequencyIndex); // =>7.5초 주기에 3.4초를 지나고있으면 3.4 10초를 지나고있으면 2.5

		if (iFrequencyIndex % 2)
		{
			tParticleAtt->_color = pInstance->Easing_Vector(TYPE_Linear, m_ParticleDesc.TargetColor2, m_ParticleDesc.TargetColor, _float(FrequencyAge), _float(TimeInterver));
			tParticleAtt->_color.w = pInstance->Easing(TYPE_Linear, m_ParticleDesc.TargetColor2.w, m_ParticleDesc.TargetColor.w, _float(FrequencyAge), _float(TimeInterver));
		}
		else
		{
			tParticleAtt->_color = pInstance->Easing_Vector(TYPE_Linear, m_ParticleDesc.TargetColor, m_ParticleDesc.TargetColor2, _float(FrequencyAge), _float(TimeInterver));
			tParticleAtt->_color.w = pInstance->Easing(TYPE_Linear, m_ParticleDesc.TargetColor.w, m_ParticleDesc.TargetColor2.w, _float(FrequencyAge), _float(TimeInterver));

		}

		tParticleAtt->_color.x = min(max(tParticleAtt->_color.x, 0), 1);
		tParticleAtt->_color.y = min(max(tParticleAtt->_color.y, 0), 1);
		tParticleAtt->_color.z = min(max(tParticleAtt->_color.z, 0), 1);
		tParticleAtt->_color.w = min(max(tParticleAtt->_color.w, 0), 1);
	}
}



HRESULT CParticleObject::SetUp_Components()
{

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VT), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_VIBuffer_Rect), TAG_COM(Com_VIBuffer), (CComponent**)&m_pVIBufferCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, m_ParticleDesc.szTextureProtoTypeTag, TAG_COM(Com_Texture), (CComponent**)&m_pTextureCom));
	FAILED_CHECK(m_pTextureCom->Change_TextureLayer(m_ParticleDesc.szTextureLayerTag));
	

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pParentTransformCom));
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_SubTransform), (CComponent**)&m_pParticleTransformCom));

	return S_OK;
}

HRESULT CParticleObject::SetUp_Texture()
{
	return S_OK;
}

HRESULT CParticleObject::SetUp_ParticleDesc(void * pArg)
{
	NULL_CHECK_RETURN(pArg, E_FAIL);

	memcpy(&m_ParticleDesc, pArg, sizeof(PARTICLEDESC));

	m_vUp = m_ParticleDesc.vUp.Get_Nomalize();

	if (m_vUp.y != 1 && m_vUp.y != -1)
	{
		m_vRight = XMVector3Normalize(_float3(0,1,0).Get_Cross(m_vUp.XMVector()));
		//m_vRight = XMVector3Normalize(m_vUp.Get_Cross(XMVectorSet(0,1,0,0)));
		m_vLook = XMVector3Normalize(m_vRight.Get_Cross(m_vUp.XMVector()));
	}
	else
	{
		m_vRight = XMVector3Normalize(_float3(0.000001f, 1, 0).Get_Cross(m_vUp.XMVector()));
		m_vLook = XMVector3Normalize(m_vRight.Get_Cross(m_vUp.XMVector()));
	}

	if (m_ParticleDesc.PowerRandomRange.y - m_ParticleDesc.PowerRandomRange.x < 0.1f)
		m_ParticleDesc.PowerRandomRange.y += 0.1f;


	m_vPivot = (m_ParticleDesc.ParticleStartRandomPosMax.XMVector() + m_ParticleDesc.ParticleStartRandomPosMin.XMVector()) *0.5f;

	return S_OK;
}

HRESULT CParticleObject::SetUp_ConstTable()
{
	return E_NOTIMPL;
}

HRESULT CParticleObject::SetUp_OnShader()
{
	return E_NOTIMPL;
}

void CParticleObject::Free()
{
	__super::Free();

	Safe_Release(m_ParticleDesc.FollowingTarget);


	Safe_Release(m_pParentTransformCom);
	Safe_Release(m_pParticleTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}




#pragma region BallParticle
CParticleeObj_Ball::CParticleeObj_Ball(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CParticleObject(pDevice,pDeviceContext)
{
}

CParticleeObj_Ball::CParticleeObj_Ball(const CParticleeObj_Ball & rhs)
	: CParticleObject(rhs)
{
}

void CParticleeObj_Ball::Reset_Velocity(_float3 & fAttVlocity)
{
	_float3 RandomVelocity = GetSingle(CUtilityMgr)->RandomFloat3(_float3(-5, -5, -5), _float3(5, 5, 5));
	
	if (m_ParticleDesc.m_bIsUI)	RandomVelocity.z = 0;

	fAttVlocity = RandomVelocity.Get_Nomalize();
}

void CParticleeObj_Ball::Update_Position_by_Velocity(PARTICLEATT * tParticleAtt, _double fTimeDelta)
{
	tParticleAtt->_position = tParticleAtt->_position.XMVector() + (tParticleAtt->_velocity.XMVector() * tParticleAtt->_force * _float(fTimeDelta));
}

HRESULT CParticleeObj_Ball::Initialize_Child_Clone()
{
	m_ParticleList.clear();

	PARTICLEATT part;


	for (_uint i = 0; i < m_ParticleDesc.MaxParticleCount; i++)
	{
		ResetParticle(&part);
		m_ParticleList.push_front(part);
	}


	return S_OK;
}

_int CParticleeObj_Ball::Update(_double fTimeDelta)
{
	if (0 > __super::Update(fTimeDelta)) return -1;
	return _int();
}

_int CParticleeObj_Ball::LateUpdate(_double fTimeDelta)
{
	if (0 > __super::LateUpdate(fTimeDelta)) return -1;
	return _int();
}

CParticleeObj_Ball * CParticleeObj_Ball::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CParticleeObj_Ball* pInstance = new CParticleeObj_Ball(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Fail to Create CParticleeObj_Ball");
		Safe_Release(pInstance);

	}


	return pInstance;
}

CGameObject * CParticleeObj_Ball::Clone(void * pArg)
{
	CParticleeObj_Ball* pInstance = new CParticleeObj_Ball(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Fail to Create CParticleeObj_Ball");
		Safe_Release(pInstance);
	}

	return pInstance;
}
#pragma endregion

CParticleeObj_Straight::CParticleeObj_Straight(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CParticleObject(pDevice, pDeviceContext)
{
}

CParticleeObj_Straight::CParticleeObj_Straight(const CParticleeObj_Straight & rhs)
	: CParticleObject(rhs)
{
}

void CParticleeObj_Straight::Reset_Velocity(_float3 & fAttVlocity)
{

	if (m_ParticleDesc.m_bIsUI) m_vUp.z = 0;

	fAttVlocity = m_vUp.Get_Nomalize();
}

void CParticleeObj_Straight::Update_Position_by_Velocity(PARTICLEATT * tParticleAtt, _double fTimeDelta)
{
	tParticleAtt->_position = tParticleAtt->_position .XMVector() + tParticleAtt->_velocity.XMVector() * tParticleAtt->_force * _float(fTimeDelta);
}

HRESULT CParticleeObj_Straight::Initialize_Child_Clone()
{

	m_ParticleList.clear();

	PARTICLEATT part;


	for (_uint i = 0; i < m_ParticleDesc.MaxParticleCount; i++)
	{
		ResetParticle(&part);
		m_ParticleList.push_front(part);
	}

	return S_OK;
}

_int CParticleeObj_Straight::Update(_double fTimeDelta)
{
	if (0 > __super::Update(fTimeDelta)) return -1;
	return _int();
}

_int CParticleeObj_Straight::LateUpdate(_double fTimeDelta)
{
	if (0 > __super::LateUpdate(fTimeDelta)) return -1;
	return _int();
}

CParticleeObj_Straight * CParticleeObj_Straight::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CParticleeObj_Straight* pInstance = new CParticleeObj_Straight(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Fail to Create CParticleeObj_Straight");
		Safe_Release(pInstance);

	}

	return pInstance;
}

CGameObject * CParticleeObj_Straight::Clone(void * pArg)
{
	CParticleeObj_Straight* pInstance = new CParticleeObj_Straight(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Fail to Create CParticleeObj_Straight");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CParticleeObj_Cone::CParticleeObj_Cone(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CParticleObject(pDevice,pDeviceContext)
{
}

CParticleeObj_Cone::CParticleeObj_Cone(const CParticleeObj_Cone & rhs)
	: CParticleObject(rhs)
{
}

void CParticleeObj_Cone::Reset_Velocity(_float3 & fAttVlocity)
{

	if (m_ParticleDesc.m_bIsUI) m_vUp.z = 0;

	_float Rate = GetSingle(CUtilityMgr)->RandomFloat(0.f, 1.f);

	_float IsMinus1 = rand()% 2 ? 1.f:-1.f;
	_float IsMinus2 = rand() % 2 ? 1.f : -1.f;

	_Vector vRand = XMVector3Normalize(m_vRight.XMVector() *IsMinus1 * Rate + m_vLook.XMVector() *IsMinus2 * (1 - Rate) 
		+ m_vUp.Get_Nomalize() * GetSingle(CUtilityMgr)->RandomFloat(m_ParticleDesc.SubPowerRandomRange.x, m_ParticleDesc.SubPowerRandomRange.y));

	fAttVlocity = vRand;
}

void CParticleeObj_Cone::Update_Position_by_Velocity(PARTICLEATT * tParticleAtt, _double fTimeDelta)
{
	tParticleAtt->_position = tParticleAtt->_position.XMVector() + tParticleAtt->_velocity.XMVector() * tParticleAtt->_force * _float(fTimeDelta);
}

HRESULT CParticleeObj_Cone::Initialize_Child_Clone()
{

	m_ParticleList.clear();

	PARTICLEATT part;


	for (_uint i = 0; i < m_ParticleDesc.MaxParticleCount; i++)
	{
		ResetParticle(&part);
		m_ParticleList.push_front(part);
	}

	return S_OK;
}

_int CParticleeObj_Cone::Update(_double fTimeDelta)
{
	if (0 > __super::Update(fTimeDelta)) return -1;
	return _int();
}

_int CParticleeObj_Cone::LateUpdate(_double fTimeDelta)
{
	if (0 > __super::LateUpdate(fTimeDelta)) return -1;
	return _int();
}

CParticleeObj_Cone * CParticleeObj_Cone::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CParticleeObj_Cone* pInstance = new CParticleeObj_Cone(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Fail to Create CParticleeObj_Cone");
		Safe_Release(pInstance);

	}

	return pInstance;
}

CGameObject * CParticleeObj_Cone::Clone(void * pArg)
{
	CParticleeObj_Cone* pInstance = new CParticleeObj_Cone(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Fail to Create CParticleeObj_Cone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CParticleeObj_Fixed::CParticleeObj_Fixed(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CParticleObject(pDevice,pDeviceContext)
{
}

CParticleeObj_Fixed::CParticleeObj_Fixed(const CParticleeObj_Fixed & rhs)
	: CParticleObject(rhs)
{
}

void CParticleeObj_Fixed::Reset_Velocity(_float3 & fAttVlocity)
{

}

void CParticleeObj_Fixed::Update_Position_by_Velocity(PARTICLEATT * tParticleAtt, _double fTimeDelta)
{
	if (m_ParticleDesc.FollowingTarget != nullptr)
		tParticleAtt->_position = m_ParticleDesc.FollowingTarget->Get_MatrixState(CTransform::STATE_POS) + m_vPivot.XMVector();
	else
		tParticleAtt->_position = m_ParticleDesc.FixedTarget.XMVector()+ m_vPivot.XMVector();
}

HRESULT CParticleeObj_Fixed::Initialize_Child_Clone()
{

	m_ParticleList.clear();

	PARTICLEATT part;


	m_ParticleDesc.Particle_Power = 0.;
	m_ParticleDesc.PowerRandomRange = _float2(0.0f, 0.1f);
	m_ParticleDesc.SubPowerRandomRange = _float2(0, 0);
	m_ParticleDesc.vUp = _float3(0, 1, 0);
	m_ParticleDesc.MaxBoundaryRadius = 1.f;
	m_vPivot = m_ParticleDesc.ParticleStartRandomPosMin;
	m_ParticleDesc.ParticleStartRandomPosMin = _float3(-0, 0, -0);
	m_ParticleDesc.ParticleStartRandomPosMax = _float3(0, 0, 0);
	if (m_ParticleDesc.MaxParticleCount > 5)m_ParticleDesc.MaxParticleCount = 5;


	for (_uint i = 0; i < m_ParticleDesc.MaxParticleCount; i++)
	{
		ResetParticle(&part);
		m_ParticleList.push_front(part);
	}
	return S_OK;
}

_int CParticleeObj_Fixed::Update(_double fTimeDelta)
{
	if (0 > __super::Update(fTimeDelta)) return -1;
	return _int();
}

_int CParticleeObj_Fixed::LateUpdate(_double fTimeDelta)
{
	if (0 > __super::LateUpdate(fTimeDelta)) return -1;
	return _int();
}

CParticleeObj_Fixed * CParticleeObj_Fixed::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CParticleeObj_Fixed* pInstance = new CParticleeObj_Fixed(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Fail to Create CParticleeObj_Fixed");
		Safe_Release(pInstance);

	}

	return pInstance;
}

CGameObject * CParticleeObj_Fixed::Clone(void * pArg)
{
	CParticleeObj_Fixed* pInstance = new CParticleeObj_Fixed(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Fail to Create CParticleeObj_Fixed");
		Safe_Release(pInstance);
	}

	return pInstance;
}




CParticleeObj_Fixed_LookFree::CParticleeObj_Fixed_LookFree(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CParticleObject(pDevice, pDeviceContext)
{
}

CParticleeObj_Fixed_LookFree::CParticleeObj_Fixed_LookFree(const CParticleeObj_Fixed_LookFree & rhs)
	: CParticleObject(rhs)
{
}

void CParticleeObj_Fixed_LookFree::Reset_Velocity(_float3 & fAttVlocity)
{
}

void CParticleeObj_Fixed_LookFree::Update_Position_by_Velocity(PARTICLEATT * tParticleAtt, _double fTimeDelta)
{
	if (m_ParticleDesc.FollowingTarget != nullptr)
		tParticleAtt->_position = m_ParticleDesc.FollowingTarget->Get_MatrixState(CTransform::STATE_POS) + m_vPivot.XMVector();
	else
		tParticleAtt->_position = m_ParticleDesc.FixedTarget.XMVector() + m_vPivot.XMVector();

}

HRESULT CParticleeObj_Fixed_LookFree::Initialize_Child_Clone()
{
	m_ParticleList.clear();

	PARTICLEATT part;


	m_ParticleDesc.Particle_Power = 0.;
	m_ParticleDesc.PowerRandomRange = _float2(0.0f, 0.1f);
	m_ParticleDesc.SubPowerRandomRange = _float2(0, 0);
	m_ParticleDesc.vUp = _float3(0, 1, 0);
	m_ParticleDesc.MaxBoundaryRadius = 1.f;
	m_vPivot = m_ParticleDesc.ParticleStartRandomPosMin;
	m_ParticleDesc.ParticleStartRandomPosMin = _float3(-0, 0, -0);
	m_ParticleDesc.ParticleStartRandomPosMax = _float3(0, 0, 0);
	if (m_ParticleDesc.MaxParticleCount > 5)m_ParticleDesc.MaxParticleCount = 5;


	for (_uint i = 0; i < m_ParticleDesc.MaxParticleCount; i++)
	{
		ResetParticle(&part);
		part._lifeTime = m_ParticleDesc.EachParticleLifeTime;
		m_ParticleList.push_front(part);
	}
	return S_OK;
}

_int CParticleeObj_Fixed_LookFree::Update(_double fTimeDelta)
{
	if (0 > __super::Update(fTimeDelta)) return -1;
	return _int();
}

_int CParticleeObj_Fixed_LookFree::LateUpdate(_double fTimeDelta)
{
	if (0 > __super::LateUpdate(fTimeDelta)) return -1;
	return _int();
}

_int CParticleeObj_Fixed_LookFree::Render()
{

	CGameInstance* pInstance = GetSingle(CGameInstance);


	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_VIEW), sizeof(_float4x4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_PROJ), sizeof(_float4x4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_PROJ), sizeof(_float4x4)));

	_float2 UVSize = _float2(1 / m_ParticleDesc.vTextureXYNum.x, 1 / m_ParticleDesc.vTextureXYNum.y);

	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vUVSize", &UVSize, sizeof(_float2)));

	if (m_ParticleDesc.m_fAlphaTestValue != 0.1f)
		FAILED_CHECK(m_pShaderCom->Set_RawValue("g_fAlphaTestValue", &m_ParticleDesc.m_fAlphaTestValue, sizeof(_float)));



	_Matrix vViewMat = pInstance->Get_Transform_Matrix(PLM_VIEW);

	if (!m_ParticleList.empty())
	{
		if (m_ParticleDesc.AlphaBlendON)
		{
			m_ParticleList.sort([](PARTICLEATT tSrc, PARTICLEATT tDest) ->_bool
			{
				return tSrc._CamDist > tDest._CamDist;
			});
		}




		for (auto iter : m_ParticleList)
		{
			if (iter._isAlive)
			{
				//if (m_ParticleDesc.ColorChageFrequency)
				//	m_pGraphicDevice->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_XRGB(_uint(iter._color.x), _uint(iter._color.y), _uint(iter._color.z)));



				if (m_ParticleDesc.m_bIsUI && m_ParticleDesc.FollowingTarget == nullptr)
				{
					//_float3 UIPos = { 0,0,0 };
					//UIPos.x = iter._position.x - g_iWinCX *0.5f;
					//UIPos.y = -iter._position.y + g_iWinCY *0.5f;
					//m_pParticleTransformCom->Set_MatrixState(CTransform::STATE_POS, UIPos);
				}
				else {
					m_pParticleTransformCom->Set_Matrix(XMMatrixIdentity());


					m_pParticleTransformCom->Set_MatrixState(CTransform::STATE_RIGHT, m_vRight);
					m_pParticleTransformCom->Set_MatrixState(CTransform::STATE_UP, m_vUp);
					m_pParticleTransformCom->Set_MatrixState(CTransform::STATE_LOOK, m_vLook);

					m_pParticleTransformCom->Scaled_All(iter._size);

					m_pParticleTransformCom->Set_MatrixState(CTransform::STATE_POS, iter._position);
				}

				FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vUVPos", &iter._TextureUV, sizeof(_float2)));
				FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vColor", &iter._color, sizeof(_float4)));
				FAILED_CHECK(m_pTextureCom->Bind_OnShader(m_pShaderCom, "g_DiffuseTexture", iter._TextureIndex));
				FAILED_CHECK(m_pParticleTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
				FAILED_CHECK(m_pVIBufferCom->Render(m_pShaderCom, m_ParticleDesc.m_iPassIndex));
			}

		}
	}

	return _int();
}

CParticleeObj_Fixed_LookFree * CParticleeObj_Fixed_LookFree::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CParticleeObj_Fixed_LookFree* pInstance = new CParticleeObj_Fixed_LookFree(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Fail to Create CParticleeObj_Fixed_LookFree");
		Safe_Release(pInstance);

	}

	return pInstance;
}

CGameObject * CParticleeObj_Fixed_LookFree::Clone(void * pArg)
{
	CParticleeObj_Fixed_LookFree* pInstance = new CParticleeObj_Fixed_LookFree(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Fail to Create CParticleeObj_Fixed_LookFree");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CParticleeObj_Spread::CParticleeObj_Spread(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CParticleObject(pDevice, pDeviceContext)
{
}

CParticleeObj_Spread::CParticleeObj_Spread(const CParticleeObj_Spread & rhs)
	: CParticleObject(rhs)
{
}

void CParticleeObj_Spread::Reset_Velocity(_float3 & fAttVlocity)
{
	_float RightRate = GetSingle(CUtilityMgr)->RandomFloat(-1, 1);
	_float LookRate = GetSingle(CUtilityMgr)->RandomFloat(-1, 1);

	_float3 ResetedVelocity = m_vRight.XMVector() * RightRate + m_vLook.XMVector() * LookRate;

	//if (m_ParticleDesc.m_bIsUI)	RandomVelocity.z = 0;

	fAttVlocity = ResetedVelocity.Get_Nomalize();
}

void CParticleeObj_Spread::Update_Position_by_Velocity(PARTICLEATT * tParticleAtt, _double fTimeDelta)
{
	tParticleAtt->_force = g_pGameInstance->Easing(TYPE_QuadIn, tParticleAtt->_Targetforce, 0, (_float)tParticleAtt->_age, (_float)tParticleAtt->_lifeTime);

	tParticleAtt->_position = tParticleAtt->_position.XMVector() + (tParticleAtt->_velocity.XMVector() * tParticleAtt->_force * _float(fTimeDelta));
}

HRESULT CParticleeObj_Spread::Initialize_Child_Clone()
{
	m_ParticleList.clear();

	PARTICLEATT part;


	for (_uint i = 0; i < m_ParticleDesc.MaxParticleCount; i++)
	{
		ResetParticle(&part);
		m_ParticleList.push_front(part);
	}


	return S_OK;
}

_int CParticleeObj_Spread::Update(_double fTimeDelta)
{
	if (0 > __super::Update(fTimeDelta)) return -1;
	return _int();
}

_int CParticleeObj_Spread::LateUpdate(_double fTimeDelta)
{
	if (0 > __super::LateUpdate(fTimeDelta)) return -1;
	return _int();
}

CParticleeObj_Spread * CParticleeObj_Spread::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CParticleeObj_Spread* pInstance = new CParticleeObj_Spread(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Fail to Create CParticleeObj_Spread");
		Safe_Release(pInstance);

	}


	return pInstance;
}

CGameObject * CParticleeObj_Spread::Clone(void * pArg)
{
	CParticleeObj_Spread* pInstance = new CParticleeObj_Spread(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Fail to Create CParticleeObj_Spread");
		Safe_Release(pInstance);
	}

	return pInstance;
}




