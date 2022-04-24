#pragma once

#include "GameObject.h"


BEGIN(Client)

// 파티클 속성
typedef struct tag_ParticleAttribute
{
	tag_ParticleAttribute()
	{
		_lifeTime = 0.0f;
		_age = 0.0f;
		_isAlive = true;
	}

	// 입차의 위치 / 방향 / 힘 
	_float3 _position;
	_float3 _velocity;
	_float	_force;

	// 시간 크기 색
	_float       _lifeTime;								
	_float       _age;	
	_bool        _isAlive;			

	_float3		_NowparantPos;
	class CParticleObject* SubParticle = nullptr;
}PARTICLEATT;





// 파티클 오브젝트 하나에 여러개의 오브젝트 정보를 들고 있어야한다.
class CParticleObject abstract : public CGameObject
{

protected:
	explicit CParticleObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CParticleObject(const CParticleObject&  rhs);
	virtual ~CParticleObject() = default;


public:
	virtual HRESULT Initialize_Prototype(void* pArg)override;
	virtual HRESULT Initialize_Clone(void* pArg)override;


	// 부모 파티클 데이터 재정의
	virtual HRESULT Initialize_Child_Clone()PURE;

	virtual _int Update(_double fTimeDelta)override;
	virtual _int LateUpdate(_double fTimeDelta)override;
	virtual _int Render()override;
	virtual _int LateRender()override;


	virtual void ResetParticle(PARTICLEATT* attribute);

	virtual void Reset_Velocity(_float3& fAttVlocity)PURE;
	virtual void Update_Position_by_Velocity(PARTICLEATT* tParticleAtt, _float fTimeDelta)PURE;
	virtual void Update_SizeChange(PARTICLEATT* tParticleAtt, _float fTimeDelta);
	virtual void Update_ColorChange(PARTICLEATT* tParticleAtt, _float fTimeDelta);


	virtual _float Get_RenderSortValue() override { return m_ParticleDesc.m_bUIDepth; }

	//bool isEmpty() { return m_list_Particles.empty(); }

	//HRESULT Reset_FixedPostion(_float3 vPos);
	//_float3 Get_ParentPos();

protected:
	HRESULT SetUp_Components();
	HRESULT SetUp_ParticleDesc(void* pArg);
	// 랜더스테이트 재정의 할 수 있게 
	virtual HRESULT SetUp_ConstTable();
	virtual HRESULT SetUp_OnShader();

protected:
	CTransform*				m_pParentTransformCom = nullptr;
	CTransform*				m_pParticleTransformCom = nullptr;

	CRenderer*				m_ComRendererCom = nullptr;
	CTexture*				m_ComTextureCom = nullptr;
	CVIBuffer_Rect*			m_ComVIBufferCom = nullptr;
	CShader*				m_pShaderCom = nullptr;


	PARTICLEDESC			m_ParticleDesc;

	_float3					m_vRight = _float3(1, 0, 0);
	_float3					m_vLook = _float3(0, 0, 1);
	_float3					m_vUp = _float3(0, 1, 0);



	list<PARTICLEATT>		m_ParticleList;

	_double					m_PassedLifeTime = 0;


public:
	virtual CGameObject* Clone(void* pArg)PURE;
	virtual void Free() override;
};


END