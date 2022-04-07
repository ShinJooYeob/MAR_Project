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
	//이 매쉬가 사용하는 머테리얼의 인덱스
	_uint		m_MaterialIndex = 0;
	//이 매쉬에 영향을 미치는 뼈의 총 개수
	_uint		m_iNumAffectingBones = 0;

private:
	HRESULT Ready_NonAnimMeshContainer(aiMesh* pAIMesh, _fMatrix TransformMatrix);
	HRESULT Ready_AnimMeshContainer(aiMesh* pAIMesh);
	HRESULT Ready_SkinnedInfo(aiMesh* pAIMesh, VTXANIMMODEL* pVertices);

public:
	static CMeshContainer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, 
		CModel::MODELTYPE eMeshtype, aiMesh* pAIMesh, _fMatrix TransformMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END