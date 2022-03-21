#pragma once

#include "Component.h"


BEGIN(Engine)

class CGameObject;

class ENGINE_DLL CRenderer final :public CComponent
{
public:
	enum RENDERGROUP 
	{
		RENDER_PRIORITY, RENDER_NONALPHA, RENDER_ALPHA, RENDER_AFTEROBJ, RENDER_UI, RENDER_END
	};

private:
	explicit CRenderer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext);
	virtual ~CRenderer() = default;

private:
	virtual HRESULT Initialize_Prototype(void * pArg)override;
	virtual HRESULT Initialize_Clone(void * pArg)override;


public:
	HRESULT Add_RenderGroup(RENDERGROUP eRenderID ,CGameObject* pGameObject);
	HRESULT Render_RenderGroup();



private:
	list<CGameObject*>			m_RenderObjectList[RENDER_END];
	typedef list<CGameObject*>		RENDEROBJECTS;
private:

	_Matrix						m_ProjMatrix;

private:
	HRESULT Render_Priority();
	HRESULT Render_NonAlpha();
	HRESULT Render_Alpha();
	HRESULT Render_AfterObj();
	HRESULT Render_UI();

public:
	static CRenderer* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void* pArg = nullptr);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END	