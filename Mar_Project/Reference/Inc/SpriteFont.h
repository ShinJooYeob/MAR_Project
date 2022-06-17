#pragma once

#include "Base.h"

BEGIN(Engine)

class CSpriteFont final : public CBase
{
private:
	CSpriteFont(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CSpriteFont() = default;

public:
	HRESULT Initialize(const _tchar* pFontFilePath);
	HRESULT Render(const _tchar* pText, _float2 vPosition, _Vector vColor);

private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;

private:
	SpriteBatch*			m_pBatch = nullptr;
	SpriteFont*				m_pFont = nullptr;


public:
	static CSpriteFont* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pFontFilePath);
	virtual void Free() override;
};

END