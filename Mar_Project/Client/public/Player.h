#pragma once

#include "GameObject.h"

BEGIN(Engine)
class CCamera;
END


BEGIN(Client)

class CPlayer final : public CGameObject
{
private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype(void* pArg)override;
	virtual HRESULT Initialize_Clone(void* pArg)override;

public:
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LateRender()override;



private:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CVIBuffer_Cube*		m_pVIBufferCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;

private:
	CCamera*			m_pMainCamera = nullptr;
	_float3				m_CamDegreeAngle = _float3(20.f,0,-5.f);

	/*For. Jump*/
	_float				m_fJumpPower = 0;
	_double				m_LevitationTime = 0;

private:
	HRESULT SetUp_Components();
	
	HRESULT Input_Keyboard(_double fDeltaTime);

	HRESULT Set_Player_On_Terrain();
	HRESULT Set_Camera_On_Player(_double fDeltaTime);

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext,void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END