#pragma once
#include "VIBuffer.h"
#include "Model.h"

BEGIN(Engine)

class CMeshContainer final : public CVIBuffer
{
private:
	explicit CMeshContainer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CMeshContainer(const CMeshContainer& rhs);
	virtual ~CMeshContainer() = default;

public:
	virtual HRESULT Initialize_Prototype(CModel::MODELTYPE eMeshtype, aiMesh* pAIMesh, _fMatrix TransformMatrix);
	virtual HRESULT Initialize_Clone(void* pArg);

	_uint Get_MaterialIndex();

private:
	_uint		m_MaterialIndex = 0;

private:
	HRESULT Ready_NonAnimMeshContainer(aiMesh* pAIMesh, _fMatrix TransformMatrix);
	HRESULT Ready_AnimMeshContainer(aiMesh* pAIMesh);

public:
	static CMeshContainer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, 
		CModel::MODELTYPE eMeshtype, aiMesh* pAIMesh, _fMatrix TransformMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END