#pragma once
#include "MapObject.h"

BEGIN(Client)

class CShapeMemoryPad final :public CMapObject
{
public:
	typedef struct tagShapeMemoryDesc
	{
		_float3 vStartPos;
	}SHAPEMEMDESC;



	typedef struct tagShapePieceDesc
	{
		_float3 vPosition =_float3(0);
		_float3 vTurnAxis = _float3(0, 1, 0);
		_float	fAngle = 0;
		_float	fEasedAngle = 0;
		_uint	iKindsOfMesh = 0;
		_double PassedTime = 0;
		_double TotoalTime = 0;
		_bool	ReturnToMem = false;
	}SHAPEPIECEDESC;


private:
	CShapeMemoryPad(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CShapeMemoryPad(const CShapeMemoryPad& rhs);
	virtual ~CShapeMemoryPad() = default;

public:
	virtual HRESULT Initialize_Prototype(void* pArg)override;
	virtual HRESULT Initialize_Clone(void* pArg)override;

public:
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LightRender()override;

public:
	HRESULT Set_TerrainTileKinds();
	HRESULT Reset_TerrainTileKindsMovableNHeightZero();

	HRESULT Let_ReturntoShape(_bool bBool);

	
private:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_ModelArr[5] = {nullptr};


	
private:
	class CPlayer*		m_pPlayer= nullptr;
	CTransform*			m_pPlayerTransform = nullptr;
	_bool				m_bIsPlayerCloser = false;

	_double				m_PatternChangeTime = 0;

	SHAPEMEMDESC		m_tDesc;

	vector<SHAPEPIECEDESC>	m_vecPiece;

	_bool				m_bReturnToShape = false;

	_uint				m_iSoundCounter = 0;

	_float4				m_NevRectPoint[4];
private:
	HRESULT SetUp_Components();
	HRESULT Ready_Piece();
	HRESULT Update_PieceAngle(_double fDeltaTime);


public:
	static CShapeMemoryPad* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END