#pragma once

#include "GameObject.h"


BEGIN(Client)

class CInstance_Particle final : public CGameObject
{
public:
	typedef struct tagInstanceParticleAttribute
	{

		tagInstanceParticleAttribute()
		{
			_lifeTime = 0.0f;
			_age = 0.0f;
			_isAlive = true;
		}

		// 입차의 위치 / 방향 / 힘 
		_float4x4 _LocalMatirx;
		_float3 _velocity;
		_float _NowForce;
		_float	_Defaultforce;

		// 시간 크기 색
		_double       _lifeTime;
		_double       _age;
		_bool        _isAlive;
		_float4		_color = _float4(1.f, 1.f, 1.f, 1.f);
		_float3		_size = _float3(1, 1, 1);
		_float2		_TextureUV = _float2(1, 1);

	}INSTPARTICLEATT;


private:
	CInstance_Particle(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CInstance_Particle(const CInstance_Particle& rhs);
	virtual ~CInstance_Particle() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual _int Update(_double TimeDelta)override;
	virtual _int LateUpdate(_double TimeDelta)override;
	virtual _int Render() override;
	virtual _int LateRender() override;



private:
	CShader*					m_pShaderCom = nullptr;
	CRenderer*					m_pRendererCom = nullptr;
	CTexture*					m_pTextureCom = nullptr;
	CVIBuffer_Point_Instance*	m_pVIBufferCom = nullptr;
	CTransform*					m_pTransformCom = nullptr;

private:
	_uint						m_iNumInstance = 0;
	_double						m_TotalLifeTime = 0;
	_double						m_PassedTime = 0;
	vector<INSTPARTICLEATT>		m_vecParticleAttribute;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ParticleAttribute();
	HRESULT SetUp_ConstantTable();

	HRESULT Update_ParticleAttribute(_double fDeltaTime);

public:
	static CInstance_Particle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};


END