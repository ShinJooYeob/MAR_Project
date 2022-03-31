#include "stdafx.h"
#include "..\Public\Scene_Edit.h"
#include "Scene_Loading.h"
#include "Camera_Editor.h"


CScene_Edit::CScene_Edit(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CScene(pDevice, pDeviceContext)
{
}

HRESULT CScene_Edit::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	m_pGameInstance = GetSingle(CGameInstance);
	Safe_AddRef(m_pGameInstance);

	FAILED_CHECK(Ready_Layer_MainCamera(TAG_LAY(Layer_Camera_Main)));


#ifdef USE_IMGUI


	//터짐 방지용 빈 오브젝트
	OBJELEMENT ObjElement;


	FAILED_CHECK(m_pGameInstance->Add_GameObject_Out_of_Manager(&ObjElement.pObject, SCENE_EDIT, TAG_OP(Prototype_EditorCursor)));

	ZeroMemory(ObjElement.matSRT.m, sizeof(_float) * 16);


	ObjElement.matSRT.m[0][0] = 1.f;
	ObjElement.matSRT.m[0][1] = 1.f;
	ObjElement.matSRT.m[0][2] = 1.f;
	ObjElement.matSRT.m[1][0] = 1.f;
	ObjElement.matSRT.m[1][1] = 1.f;
	ObjElement.matSRT.m[1][2] = 1.f;

	RenewElenmetTransform(&ObjElement);

	m_vecBatchedObject.push_back(ObjElement);

	Prevent_Order = false;
	m_SelectedObjectSRT = &(m_vecBatchedObject[0].matSRT);
	m_iBatchedVecIndex = 0;
	m_iPassIndex = 0;

	ZeroMemory(m_ArrBuffer, sizeof(_float) * 4);
	ZeroMemory(bArrWindowFlag, sizeof(_bool) * 10);
	ZeroMemory(m_iSelectedObjectNMesh, sizeof(_uint) * 2);



	m_ArrBuffer[3] = 0.1f;







#endif // USE_IMGUI
	return S_OK;
}

_int CScene_Edit::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;

	if (m_bIsNeedToSceneChange)
		return Change_to_NextScene();

	if (m_iNowTab == 0)
	{
		for (auto& Element : m_vecBatchedObject)
		{
			if (Element.pObject->Update(fDeltaTime) < 0)
			{
				__debugbreak();
				return -1;
			}
		}
	}

#ifdef USE_IMGUI

	FAILED_CHECK(Input_KeyBoard(fDeltaTime));

	if (GETIMGUI->Update_ImguiMgr(fDeltaTime) >= 0)
	{



		FAILED_CHECK(Update_First_Frame(fDeltaTime, "Editor"));


		Prevent_Order = true;
	}

#endif // USE_IMGUI
	return 0;
}

_int CScene_Edit::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;

	if (m_bIsNeedToSceneChange)
		return Change_to_NextScene();


	if (m_iNowTab == 0)
	{
		for (auto& Element : m_vecBatchedObject)
		{
			if (Element.pObject->LateUpdate(fDeltaTime) < 0)
			{
				__debugbreak();
				return -1;
			}
		}
	}

	return 0;
}

_int CScene_Edit::Render()
{
	if (__super::Render() < 0)
		return -1;




#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("MapEditScene"));
#endif // _DEBUG


	return 0;
}

_int CScene_Edit::LateRender()
{
	if (__super::LateRender() < 0)
		return -1;

#ifdef USE_IMGUI
	if (Prevent_Order)
	{
		GETIMGUI->Render_ImguiMgr();
		Prevent_Order = false;
	}
#endif // USE_IMGUI


	return 0;
}

_int CScene_Edit::Change_to_NextScene()
{

	switch (m_eNextScene)
	{

	case SCENEID::SCENE_LOBY:
	{
		FAILED_CHECK(g_pGameInstance->Scene_Change(CScene_Loading::Create(m_pDevice, m_pDeviceContext, SCENEID::SCENE_LOBY), SCENEID::SCENE_LOADING));
		break;
	}


	}
	return _int();
}

#ifdef USE_IMGUI

#pragma region Total
HRESULT CScene_Edit::Update_First_Frame(_double fDeltatime, const char * szFrameBarName)
{
	_bool demotrue = true;
	ImGui::ShowDemoWindow(&demotrue);

	ImGuiWindowFlags window_flags = 0;
	if (bArrWindowFlag[0])				window_flags |= ImGuiWindowFlags_NoBackground;
	if (bArrWindowFlag[1])				window_flags |= ImGuiWindowFlags_NoMove;
	/*
	if (bArrWindowFlag[2])				window_flags |= ImGuiWindowFlags_NoTitleBar;
	if (bArrWindowFlag[3])				window_flags |= ImGuiWindowFlags_NoScrollbar;
	if (!bArrWindowFlag[4])				window_flags |= ImGuiWindowFlags_MenuBar;
	if (bArrWindowFlag[5])				window_flags |= ImGuiWindowFlags_NoResize;
	if (bArrWindowFlag[6])				window_flags |= ImGuiWindowFlags_NoCollapse;
	if (bArrWindowFlag[7])				window_flags |= ImGuiWindowFlags_NoNav;
	if (bArrWindowFlag[8])				window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
	if (bArrWindowFlag[9])				window_flags |= ImGuiWindowFlags_UnsavedDocument;
	*/

	static _bool bFirstSetting = false;

	if (!bFirstSetting)
	{
		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowSize(ImVec2(center.x * 0.667f, center.y * 2.f), ImGuiCond_None);
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_None, ImVec2(0.0f, 0.0f));
		bFirstSetting = true;
	}


	GETIMGUI->Begin_Update_Frame(fDeltatime, szFrameBarName, window_flags);



	ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 0.0f);

	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
	{
		if (ImGui::BeginTabItem("Map Editor"))
		{
			m_iNowTab = 0;
			FAILED_CHECK(Update_MapTab(fDeltatime));
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("UI Editor"))
		{
			m_iNowTab = 1;
			FAILED_CHECK(Update_UITab(fDeltatime));
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Particle Editor"))
		{
			m_iNowTab = 2;
			FAILED_CHECK(Update_ParticleTab(fDeltatime));
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("CameraAction"))
		{
			m_iNowTab = 3;
			FAILED_CHECK(Update_CameraActionTab(fDeltatime));
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	ImGui::PopStyleVar();


	GETIMGUI->End_Update_Frame();
	return S_OK;


}

void CScene_Edit::Make_HelpWidget(const char * szString)
{

	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(szString);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}

}

void CScene_Edit::Make_VerticalSpacing(_uint count)
{
	for (_uint i = 0; i < count; i++)
	{
		ImGui::Spacing();
	}

}

HRESULT CScene_Edit::Sava_Data(const char* szFileName, eDATATYPE iKinds)
{
	switch (iKinds)
	{
	case Client::CScene_Edit::Data_Map:
	{
		//../bin/Resources/Data/Map/
		_tchar szFullPath[MAX_PATH] =L"../bin/Resources/Data/Map/";
		_tchar wFileName[MAX_PATH] = L"";

		MultiByteToWideChar(CP_UTF8, 0, szFileName, -1, wFileName, sizeof(wFileName));
		//WideCharToMultiByte(CP_UTF8, 0, fd.name, -1, szFilename, sizeof(szFilename), NULL, NULL);
		lstrcat(szFullPath, wFileName);


		//HANDLE hFile = CreateFileW(szFullPath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

		HANDLE hFile = ::CreateFileW(szFullPath, GENERIC_WRITE , FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, NULL);




		if (INVALID_HANDLE_VALUE == hFile)
			return E_FAIL;

		DWORD	dwByte = 0;

		//// 유니코드임을 알리는 BOM
		//BYTE wc = 0xEF;
		//WriteFile(hFile, &wc, 1, &dwByte, NULL);
		//wc = 0xBB;
		//WriteFile(hFile, &wc, 1, &dwByte, NULL);
		//wc = 0xBF;
		//WriteFile(hFile, &wc, 1, &dwByte, NULL);

		_int iIDLength = 0;

		for (auto& tObjectElement : m_vecBatchedObject)
		{
			// key 값 저장


			iIDLength = lstrlen(tObjectElement.ObjectID);
			WriteFile(hFile, &(iIDLength), sizeof(_int), &dwByte, nullptr);
			WriteFile(hFile, (tObjectElement.ObjectID), sizeof(_tchar) * iIDLength, &dwByte, nullptr);

			iIDLength = lstrlen(tObjectElement.MeshID);
			WriteFile(hFile, &(iIDLength), sizeof(_int), &dwByte, nullptr);
			WriteFile(hFile, (tObjectElement.MeshID), sizeof(_tchar) * iIDLength, &dwByte, nullptr);

			iIDLength = lstrlen(tObjectElement.TexturePath);
			WriteFile(hFile, &(iIDLength), sizeof(_int), &dwByte, nullptr);
			WriteFile(hFile, (tObjectElement.TexturePath), sizeof(_tchar) * iIDLength, &dwByte, nullptr);

			iIDLength = lstrlen(tObjectElement.TextureKey);
			WriteFile(hFile, &(iIDLength), sizeof(_int), &dwByte, nullptr);
			WriteFile(hFile, (tObjectElement.TextureKey), sizeof(_tchar) * iIDLength, &dwByte, nullptr);

			WriteFile(hFile, &(tObjectElement.PassIndex), sizeof(_uint), &dwByte, nullptr);
			WriteFile(hFile, &(tObjectElement.matSRT.m[0][0]), sizeof(_float) * 16, &dwByte, nullptr);
			WriteFile(hFile, &(tObjectElement.matTransform.m[0][0]), sizeof(_float) * 16, &dwByte, nullptr);



		}
		CloseHandle(hFile);

	}
		break;
	case Client::CScene_Edit::Data_UI:
	{

	}
		break;
	case Client::CScene_Edit::Data_Particle:
	{

	}
		break;
	case Client::CScene_Edit::Data_CameraAction:
	{

	}
		break;
	default:

		break;
	}



	return S_OK;
}

HRESULT CScene_Edit::Load_Data(const char * szFileName, eDATATYPE iKinds)
{
	switch (iKinds)
	{
	case Client::CScene_Edit::Data_Map:
	{
		{
			for (auto& tObjElement : m_vecBatchedObject)
			{
				//오브젝트 제대로 델리트delete 해주기
				Safe_Release(tObjElement.pObject);
			}
			m_vecBatchedObject.clear();

			ZeroMemory(m_ArrBuffer, sizeof(_float) * 4);
			m_ArrBuffer[3] = 0.1f;
			ZeroMemory(m_iSelectedObjectNMesh, sizeof(_uint) * 2);

			m_bIsModelMove = 0;
			m_iKindsOfMoving = 0;
			m_iSelectedXYZ = 0; 
			m_iPassIndex = 0;
		}



		//../bin/Resources/Data/Map/
		_tchar szFullPath[MAX_PATH] = L"../bin/Resources/Data/Map/";
		_tchar wFileName[MAX_PATH] = L"";

		MultiByteToWideChar(CP_UTF8, 0, szFileName, -1, wFileName, sizeof(wFileName));
		//WideCharToMultiByte(CP_UTF8, 0, fd.name, -1, szFilename, sizeof(szFilename), NULL, NULL);
		lstrcat(szFullPath, wFileName);



		//HANDLE hFile = CreateFileW(szFullPath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);


		HANDLE hFile = ::CreateFileW(szFullPath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, NULL);


		if (INVALID_HANDLE_VALUE == hFile)
			return E_FAIL;

		DWORD	dwByte = 0;

		CGameInstance* pInstance = g_pGameInstance;

		_uint iIDLength = 0;

		// 유니코드임을 알리는 BOM
		//DWORD wc = 0xFF;
		//ReadFile(hFile, &wc, 3, &dwByte, NULL);

		while (true)
		{
			OBJELEMENT	tData{};
			_tchar szBuffer[MAX_PATH] = L"";
			// key 값 로드
			ReadFile(hFile, &(iIDLength), sizeof(_uint), &dwByte, nullptr);
			ReadFile(hFile, (szBuffer), sizeof(_tchar) * iIDLength, &dwByte, nullptr);
			lstrcpy(tData.ObjectID, szBuffer);

			lstrcpy(szBuffer, L"");
			ReadFile(hFile, &(iIDLength), sizeof(_uint), &dwByte, nullptr);
			//ReadFile(hFile, &(tData.MeshID), sizeof(_tchar) * iIDLength, &dwByte, nullptr);
			ReadFile(hFile, (szBuffer), sizeof(_tchar) * iIDLength, &dwByte, nullptr);
			lstrcpy(tData.MeshID, szBuffer);

			lstrcpy(szBuffer, L"");
			ReadFile(hFile, &(iIDLength), sizeof(_uint), &dwByte, nullptr);
			//ReadFile(hFile, &(tData.TexturePath), sizeof(_tchar) * iIDLength, &dwByte, nullptr);
			ReadFile(hFile, (szBuffer), sizeof(_tchar) * iIDLength, &dwByte, nullptr);
			lstrcpy(tData.TexturePath, szBuffer);

			lstrcpy(szBuffer, L"");
			ReadFile(hFile, &(iIDLength), sizeof(_uint), &dwByte, nullptr);
			//ReadFile(hFile, &(tData.TextureKey), sizeof(_tchar) * iIDLength, &dwByte, nullptr);
			ReadFile(hFile, (szBuffer), sizeof(_tchar) * iIDLength, &dwByte, nullptr);
			lstrcpy(tData.TextureKey, szBuffer);

			ReadFile(hFile, &(tData.PassIndex), sizeof(_uint), &dwByte, nullptr);
			ReadFile(hFile, &(tData.matSRT.m[0][0]), sizeof(_float) * 16, &dwByte, nullptr);
			ReadFile(hFile, &(tData.matTransform.m[0][0]), sizeof(_float) * 16, &dwByte, nullptr);

			if (0 == dwByte)
				break;
			
			//객채 생성해주기

			pInstance->Add_GameObject_Out_of_Manager(&(tData.pObject), SCENE_EDIT, tData.ObjectID);

			NULL_CHECK_RETURN(tData.pObject, E_FAIL);


			if (lstrcmp(tData.MeshID, TAG_MESH(Mesh_None)))
			{
				//매쉬 바꿔주기 
			}

			//트렌스폼
			CTransform* pTrans = (CTransform*)(tData.pObject->Get_Component(TAG_COM(Com_Transform)));
			NULL_CHECK_RETURN(pTrans, E_FAIL);
			pTrans->Set_Matrix(tData.matTransform);



			m_vecBatchedObject.push_back(tData);
		}


		m_iBatchedVecIndex = 0;
		m_SelectedObjectSRT = &(m_vecBatchedObject[m_iBatchedVecIndex].matSRT);

		CloseHandle(hFile);
	

	}
	break;
	case Client::CScene_Edit::Data_UI:
	{

	}
	break;
	case Client::CScene_Edit::Data_Particle:
	{

	}
	break;
	case Client::CScene_Edit::Data_CameraAction:
	{

	}
	break;
	default:

		break;
	}
	return S_OK;
}

HRESULT CScene_Edit::Input_KeyBoard(_double fDeltaTime)
{

	CGameInstance* pInstance = GetSingle(CGameInstance);

	if (pInstance->Get_DIKeyState(DIK_SPACE) & DIS_Down)
	{
		if (m_bIsModelMove) m_bIsModelMove = 0;
		else
		{

			// 해당 오브젝트의 메트릭스 받아오기
			m_SelectedObjectSRT = &(m_vecBatchedObject[m_iBatchedVecIndex].matSRT);
			ZeroMemory(m_ArrBuffer, sizeof(_float) * 4);
			m_ArrBuffer[3] = 0.1f;

			m_bIsModelMove = 1;

		}

	}


	if (m_bIsModelMove)
	{
		if (pInstance->Get_DIKeyState(DIK_1) & DIS_Down)		m_iKindsOfMoving = 0;
		else if (pInstance->Get_DIKeyState(DIK_2) & DIS_Down)	m_iKindsOfMoving = 1;
		else if (pInstance->Get_DIKeyState(DIK_3) & DIS_Down)	m_iKindsOfMoving = 2;



		if (pInstance->Get_DIMouseButtonState(CInput_Device::MBS_RBUTTON) & DIS_Down)
		{
			m_iSelectedXYZ++;
			if (m_iSelectedXYZ > 2)
				m_iSelectedXYZ = 0;
		}

		_long fWheelMove = pInstance->Get_DIMouseMoveState(CInput_Device::MMS_WHEEL);
		if (fWheelMove)
		{
			_float tempValue;
			memcpy(&tempValue, ((_float*)(&(m_SelectedObjectSRT->m[2 - m_iKindsOfMoving])) + m_iSelectedXYZ), sizeof(_float));

			if (fWheelMove > 0)
				tempValue += _float(m_ArrBuffer[3]);
			else
				tempValue += _float(-m_ArrBuffer[3]);
			//tempValue += _float(fWheelMove * m_ArrBuffer[3] * fDeltaTime);


			FAILED_CHECK(RenewElenmetTransform(&(m_vecBatchedObject[m_iBatchedVecIndex])));
			memcpy(((_float*)(&(m_SelectedObjectSRT->m[2 - m_iKindsOfMoving])) + m_iSelectedXYZ), &tempValue, sizeof(_float));

			int t = 0;
		}

	}


	return S_OK;
}

#pragma endregion Total


#pragma region MapTab
HRESULT CScene_Edit::Update_MapTab(_double fDeltatime)
{

	FAILED_CHECK(Widget_SRT(fDeltatime));

	Make_VerticalSpacing(2);
	ImGui::Separator();
	Make_VerticalSpacing(3);

	int open_action = -1;
	if (ImGui::Button("Open all about Object"))
		open_action = 1;
	ImGui::SameLine();
	if (ImGui::Button("Close all about Object"))
		open_action = 0;
	if (open_action != -1)
		ImGui::SetNextItemOpen(open_action != 0);
	FAILED_CHECK(Widget_BatchedObjectList(fDeltatime));

	if (open_action != -1)
		ImGui::SetNextItemOpen(open_action != 0);
	FAILED_CHECK(Widget_CreateDeleteObject(fDeltatime));


	Make_VerticalSpacing(2);
	ImGui::Separator();

	Make_VerticalSpacing(3);

	FAILED_CHECK(Widget_SaveLoadMapData(fDeltatime));

	return S_OK;
}

HRESULT CScene_Edit::Widget_SRT(_double fDeltatime)
{
	ibClickChecker = 0;
	ImGui::RadioButton("Camera Moving", &m_bIsModelMove, 0); ImGui::SameLine();
	ImGui::RadioButton("Model Moving      ", &m_bIsModelMove, 1); ImGui::SameLine();
	ibClickChecker += _uint(ImGui::IsItemClicked());

	//if (m_iBatchedVecIndex == 0)
	//{
	//	m_bIsModelMove = 0;
	//}
	//else
	if(m_bIsModelMove)
	{
		if (ibClickChecker)
		{
			/////////////////////////////////////모델 정보 받아오기

			m_SelectedObjectSRT = &(m_vecBatchedObject[m_iBatchedVecIndex].matSRT);

		}
	}




	ImGui::TableNextColumn(); ImGui::Checkbox("BG X", &bArrWindowFlag[0]); ImGui::SameLine();
	ImGui::TableNextColumn(); ImGui::Checkbox("Move X\n\n\n", &bArrWindowFlag[1]);
	/*
	ImGui::TableNextColumn(); ImGui::Checkbox("No titlebar", &bArrWindowFlag[2]);
	ImGui::TableNextColumn(); ImGui::Checkbox("No scrollbar", &bArrWindowFlag[3]);
	ImGui::TableNextColumn(); ImGui::Checkbox("No menu", &bArrWindowFlag[4]);
	ImGui::TableNextColumn(); ImGui::Checkbox("No resize", &bArrWindowFlag[5]);
	ImGui::TableNextColumn(); ImGui::Checkbox("No collapse", &bArrWindowFlag[6]);
	ImGui::TableNextColumn(); ImGui::Checkbox("No nav", &bArrWindowFlag[7]);
	ImGui::TableNextColumn(); ImGui::Checkbox("No bring to front", &bArrWindowFlag[8]);
	ImGui::TableNextColumn(); ImGui::Checkbox("Unsaved document", &bArrWindowFlag[9]);
	*/

	if (!m_bIsModelMove)
		ImGui::BeginDisabled(true);

	ibClickChecker = 0;
	ImGui::RadioButton("Position", &m_iKindsOfMoving, 0); ImGui::SameLine();
	ibClickChecker += _uint(ImGui::IsItemClicked());
	ImGui::RadioButton("Rotation", &m_iKindsOfMoving, 1); ImGui::SameLine();
	ibClickChecker += _uint(ImGui::IsItemClicked());
	ImGui::RadioButton("Scale\n\n\n", &m_iKindsOfMoving, 2);
	ibClickChecker += _uint(ImGui::IsItemClicked());

	if (ibClickChecker)
	{
		ZeroMemory(m_ArrBuffer, sizeof(_float) * 4);
		m_ArrBuffer[3] = 0.1f;
	}

	ImGui::DragFloat("Drag Speed", &(m_ArrBuffer[3]), 0.1f, FLT_MIN, FLT_MAX);
	{

		char Label[64];
		if (m_iBatchedVecIndex)
			sprintf_s(Label, "\n  Selected Object Index : %d", m_iBatchedVecIndex);
		else
			sprintf_s(Label, "\n  !!!!!!!!!!!Create New Object!!!!!!!!!");

		ImGui::Text(Label);
	}
	switch (m_iSelectedXYZ)
	{
	case 0:
		ImGui::Text("  Position		| Selected : X");
		break;

	case 1:
		ImGui::Text("  Rotation		| Selected : Y");
		break;

	case 2:
		ImGui::Text("  Scale			| Selected : Z");
		break;

	default:
		break;
	}


	switch (m_iKindsOfMoving)
	{
	case 0://포지션
	{
		memcpy(m_ArrBuffer, &(m_SelectedObjectSRT->m[2]), sizeof(_float) * 3);
		ImGui::DragFloat3(" X Y Z", m_ArrBuffer, m_ArrBuffer[3], -FLT_MAX, FLT_MAX);

		memcpy(&(m_SelectedObjectSRT->m[2]), m_ArrBuffer, sizeof(_float) * 3);
	}
	break;

	case 1://로테이션
	{
		memcpy(m_ArrBuffer, &(m_SelectedObjectSRT->m[1]), sizeof(_float) * 3);
		ImGui::DragFloat3(" X Y Z", m_ArrBuffer, m_ArrBuffer[3], -FLT_MAX, FLT_MAX);

		memcpy(&(m_SelectedObjectSRT->m[1]), m_ArrBuffer, sizeof(_float) * 3);

	}
	break;


	case 2://Scale
	{
		memcpy(m_ArrBuffer, &(m_SelectedObjectSRT->m[0]), sizeof(_float) * 3);
		ImGui::DragFloat3(" X Y Z", m_ArrBuffer, m_ArrBuffer[3], -FLT_MAX, FLT_MAX);

		memcpy(&(m_SelectedObjectSRT->m[0]), m_ArrBuffer, sizeof(_float) * 3);
	}
	break;


	default:
		break;
	}

	if (m_bIsModelMove)
		FAILED_CHECK(RenewElenmetTransform(&(m_vecBatchedObject[m_iBatchedVecIndex])));

	/*회전값 용

	D3DXQUATERNION quat_x, quat_y, quat_z, quat_1, quat_2;
	_float4x4 matrix;
	D3DXVECTOR3 axis_x(1.0f, 0.0f, 0.0f);
	D3DXVECTOR3 axis_y(0.0f, 1.0f, 0.0f);
	D3DXVECTOR3 axis_z(0.0f, 0.0f, 1.0f);
	D3DXQuaternionRotationAxis(&quat_x, &axis_x, rot.x);
	D3DXQuaternionRotationAxis(&quat_y, &axis_y, rot.y);
	D3DXQuaternionRotationAxis(&quat_z, &axis_z, rot.z);

	quat_1 = quat_y * quat_x * quat_z;
	D3DXQuaternionNormalize(&quat_2, &quat_1);
	D3DXMatrixRotationQuaternion(&matrix, &quat_2);

	m_ComTransform->Set_MatrixState(CTransform::STATE_RIGHT, *(_float3*)&matrix.m[0]);
	m_ComTransform->Set_MatrixState(CTransform::STATE_UP, *(_float3*)&matrix.m[1]);
	m_ComTransform->Set_MatrixState(CTransform::STATE_LOOK, *(_float3*)&matrix.m[2]);
	*/

	Make_VerticalSpacing(5);
	if (ImGui::TreeNode("Show All"))
	{

		static bool selected[4] = {};

		if (ImGui::BeginTable("split2", 4, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders))
		{
			char HederLabel[32];
			sprintf_s(HederLabel, "SRT Type");
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::Selectable(HederLabel, &selected[0], ImGuiSelectableFlags_SpanAllColumns);
			ImGui::TableNextColumn();
			ImGui::Text("X");
			ImGui::TableNextColumn();
			ImGui::Text("Y");
			ImGui::TableNextColumn();
			ImGui::Text("Z");

			for (int i = 0; i < 3; i++)
			{
				char label[32];
				switch (i)
				{
				case 0:
					sprintf_s(label, "Position");
					break;
				case 1:
					sprintf_s(label, "Rotation");
					break;
				case 2:
					sprintf_s(label, "Scale");
					break;
				default:
					sprintf_s(label, "ERROR");
					break;
				}

				_float3 vSRT = {};
				memcpy(&vSRT, &(m_SelectedObjectSRT->m[2 - i]), sizeof(_float) * 3);

				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::Selectable(label, &selected[i + 1], ImGuiSelectableFlags_SpanAllColumns);
				ImGui::TableNextColumn();
				ImGui::Text("%f", vSRT.x);
				ImGui::TableNextColumn();
				ImGui::Text("%f", vSRT.y);
				ImGui::TableNextColumn();
				ImGui::Text("%f", vSRT.z);
			}
			ImGui::EndTable();
		}


		ImGui::TreePop();
	}

	if (!m_bIsModelMove)
		ImGui::EndDisabled();

	return S_OK;
}

HRESULT CScene_Edit::Widget_BatchedObjectList(_double fDeltatime)
{
	Make_VerticalSpacing(2);
	if (ImGui::TreeNode("Batched Object List"))
	{
		vector<string> ObjectLabelIist;

		for (_uint i = 0; i < m_vecBatchedObject.size(); i++)
		{
			char HeaderLabel[64];

			sprintf_s(HeaderLabel, "%d. %ws (Mesh : %ws) (PassIndex : %d)", i,
				m_vecBatchedObject[i].ObjectID,
				m_vecBatchedObject[i].MeshID ,  
				m_vecBatchedObject[i].PassIndex
			);
			//sprintf_s(HederLabel, "%d. %ws (%d)",i, m_vecBatchedObject[i].pObject->Get_NameTag(), m_vecBatchedObject[i].ObjectID);

			ObjectLabelIist.push_back({ HeaderLabel });
		}


		{

			char Label[64];
			sprintf_s(Label, "Selected Index : %d  | Total Object Count : %zd", m_iBatchedVecIndex, ObjectLabelIist.size() - 1);

			ImGui::Text(Label);
		}
		if (ImGui::BeginListBox("##listbox 2", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
		{


			for (int n = 1; n < ObjectLabelIist.size(); n++)
			{
				const bool is_selected = (m_iBatchedVecIndex == n);
				if (ImGui::Selectable(ObjectLabelIist[n].c_str(), is_selected))
				{


					m_iBatchedVecIndex = n;
					m_SelectedObjectSRT = &(m_vecBatchedObject[m_iBatchedVecIndex].matSRT);
				}


				if (m_bIsModelMove != 0 && ImGui::IsItemHovered())
				{
					m_bIsModelMove = 0;
				}

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndListBox();
		}



		Make_VerticalSpacing(2);

		if (ImGui::Button("Delete Object", ImVec2(-FLT_MIN, 0.0f)) && m_iBatchedVecIndex)
		{

			auto iter = m_vecBatchedObject.begin();

			for (_uint i = 0; i < m_iBatchedVecIndex; i++)
				iter++;

			////실제로 죽이기 없애기 delete//////////////////////////////////////////////////////////////////////
			Safe_Release(iter->pObject);

			m_vecBatchedObject.erase(iter);

			m_iBatchedVecIndex = 0;
			m_SelectedObjectSRT = &(m_vecBatchedObject[m_iBatchedVecIndex].matSRT);


			m_iBatchedVecIndex = _uint(m_vecBatchedObject.size() - 1);
			m_SelectedObjectSRT = &(m_vecBatchedObject[m_iBatchedVecIndex].matSRT);
		}



		ImGui::TreePop();
	}

	return S_OK;
}

HRESULT CScene_Edit::Widget_CreateDeleteObject(_double fDeltatime)
{

	Make_VerticalSpacing(5);
	if (ImGui::TreeNode("Create Object"))
	{


		ImGui::Text("Selected\n\n");

		{
			char buf[128];
			sprintf_s(buf, "%ws (Mesh : %ws) (Pass : %d)",
				TAG_OP(OBJECTPROTOTYPEID(m_iSelectedObjectNMesh[0])),
				TAG_MESH(MESHTYPEID(m_iSelectedObjectNMesh[1])),
				m_iPassIndex
			);

			ImGui::Text(buf);
		}


		{
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_None | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_HorizontalScrollbar;
			{

				ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
				ImGui::BeginChild("ChildL", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 130), true, window_flags);


				if (ImGui::BeginMenuBar())
				{
					if (ImGui::BeginMenu("ObjectList"))
					{
						ImGui::EndMenu();
					}
					ImGui::EndMenuBar();

					//char buf[128];
					//sprintf_s(buf, "%ws\n", TAG_OP(OBJECTPROTOTYPEID(m_iSelectedObjectNMesh[0])));
					//ImGui::Text(buf);

				}

				if (ImGui::BeginTable("split", 1, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings))
				{
					for (int i = 0; i < Object_Prototype_End; i++)
					{
						char buf[128];
						sprintf_s(buf, "%ws", TAG_OP(OBJECTPROTOTYPEID(i)));
						ImGui::TableNextColumn();

						if (ImGui::Button(buf, ImVec2(-FLT_MIN, 0.0f)))
							m_iSelectedObjectNMesh[0] = i;

						if (m_iBatchedVecIndex != 0 && ImGui::IsItemHovered())
						{
							m_iBatchedVecIndex = 0;
							m_bIsModelMove = 0;
							m_SelectedObjectSRT = &(m_vecBatchedObject[m_iBatchedVecIndex].matSRT);
							ZeroMemory(m_iSelectedObjectNMesh, sizeof(_uint) * 2);
						}
					}
					ImGui::EndTable();
				}
				ImGui::EndChild();
				ImGui::PopStyleVar();
			}

			ImGui::SameLine();

			{

				ImGui::BeginChild("ChildR", ImVec2(0, 130), true, window_flags);


				if (ImGui::BeginMenuBar())
				{
					if (ImGui::BeginMenu("Mesh List"))
					{
						ImGui::EndMenu();
					}
					ImGui::EndMenuBar();

					//char buf[128];
					//sprintf_s(buf, "%ws\n", TAG_MESH(MESHTYPEID(m_iSelectedObjectNMesh[1])));
					//ImGui::Text(buf);
				}

				if (ImGui::BeginTable("split", 1, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings))
				{



					for (int i = 0; i < MeshID_End; i++)
					{
						char buf[128];
						sprintf_s(buf, "%ws", TAG_MESH(MESHTYPEID(i)));
						ImGui::TableNextColumn();
						if (ImGui::Button(buf, ImVec2(-FLT_MIN, 0.0f)))
							m_iSelectedObjectNMesh[1] = i;

						if (m_iBatchedVecIndex != 0 && ImGui::IsItemHovered())
						{
							m_iBatchedVecIndex = 0; m_bIsModelMove = 0;
							m_SelectedObjectSRT = &(m_vecBatchedObject[m_iBatchedVecIndex].matSRT);
							ZeroMemory(m_iSelectedObjectNMesh, sizeof(_uint) * 2);
						}
					}
					ImGui::EndTable();
				}

				Make_VerticalSpacing(1);

				ImGui::EndChild();
				ImGui::InputInt("PassIndex", &m_iPassIndex);
				Make_VerticalSpacing(1);
			}
		}

		Make_VerticalSpacing(2);
		if (ImGui::Button("Create Object", ImVec2(-FLT_MIN, 0.0f)))
		{
			///오브젝트 생성 코드 집어넣기

			OBJELEMENT ObjElement;




			ZeroMemory(ObjElement.matSRT.m, sizeof(_float) * 16);

			lstrcpy(ObjElement.ObjectID, TAG_OP((OBJECTPROTOTYPEID)m_iSelectedObjectNMesh[0]));
			//ObjElement.ObjectID = TAG_OP((OBJECTPROTOTYPEID) m_iSelectedObjectNMesh[0]);

			ObjElement.matSRT = m_vecBatchedObject[m_iBatchedVecIndex].matSRT;

			lstrcpy(ObjElement.MeshID, TAG_MESH((MESHTYPEID)m_iSelectedObjectNMesh[1]));
			//ObjElement.MeshID = TAG_MESH( (MESHTYPEID) m_iSelectedObjectNMesh[1]);
			ObjElement.PassIndex = m_iPassIndex;


			/////실제 생성하기

			FAILED_CHECK(m_pGameInstance->Add_GameObject_Out_of_Manager(&(ObjElement.pObject), SCENE_EDIT, ObjElement.ObjectID));

			FAILED_CHECK(RenewElenmetTransform(&ObjElement));

			if (lstrcmp(ObjElement.MeshID, TAG_MESH(Mesh_None)))
			{
				//매쉬 바꿔주기 
			}

			m_vecBatchedObject.push_back(ObjElement);

			m_iBatchedVecIndex = _uint(m_vecBatchedObject.size() - 1);
			m_SelectedObjectSRT = &(m_vecBatchedObject[m_iBatchedVecIndex].matSRT);


		}


		ImGui::TreePop();
	}


	return S_OK;
}

HRESULT CScene_Edit::Widget_SaveLoadMapData(_double fDeltatime)
{
	if (ImGui::Button("New Map"))
		ImGui::OpenPopup("New Map");
	ImGui::SameLine();
	if (ImGui::Button("Save Map"))
		ImGui::OpenPopup("Save Map");
	ImGui::SameLine();
	if (ImGui::Button("Laod Map"))
		ImGui::OpenPopup("Laod Map");
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();

	// Always center this window when appearing
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("New Map", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("!!!!!!!!!!!!!!!!Waring!!!!!!!!!!!!!!!!\n\n Delete Batched Object Without Save!!!\n\n	Please Check Save One more\n\n\n");
		ImGui::Separator();

		//static int unused_i = 0;
		//ImGui::Combo("Combo", &unused_i, "Delete\0Delete harder\0");

		//static bool dont_ask_me_next_time = false;
		//ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
		//ImGui::Checkbox("Don't ask me next time", &dont_ask_me_next_time);
		//ImGui::PopStyleVar();

		if (ImGui::Button("OK", ImVec2(130, 0))) { 

			for (auto& tObjElement : m_vecBatchedObject)
			{
				//오브젝트 제대로 델리트delete 해주기
				Safe_Release(tObjElement.pObject);
			}
			m_vecBatchedObject.clear();

			OBJELEMENT ObjElement;
			
			FAILED_CHECK(m_pGameInstance->Add_GameObject_Out_of_Manager(&ObjElement.pObject, SCENE_EDIT, TAG_OP(Prototype_EditorCursor)));

			ZeroMemory(ObjElement.matSRT.m, sizeof(_float) * 16);

			ObjElement.matSRT.m[0][0] = 1.f;
			ObjElement.matSRT.m[0][1] = 1.f;
			ObjElement.matSRT.m[0][2] = 1.f;
			ObjElement.matSRT.m[1][0] = 1.f;
			ObjElement.matSRT.m[1][1] = 1.f;
			ObjElement.matSRT.m[1][2] = 1.f;

			RenewElenmetTransform(&ObjElement);
			
		
			m_vecBatchedObject.push_back(ObjElement);



			m_SelectedObjectSRT = &(m_vecBatchedObject[0].matSRT);
			m_iBatchedVecIndex = 0;

			ZeroMemory(m_ArrBuffer, sizeof(_float) * 4);
			m_ArrBuffer[3] = 0.1f;
			ZeroMemory(m_iSelectedObjectNMesh, sizeof(_uint) * 2);

			m_bIsModelMove = 0;
			m_iKindsOfMoving = 0;
			m_iSelectedXYZ = 0;
			m_iPassIndex = 0;




			ImGui::CloseCurrentPopup(); 
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(130, 0))) { ImGui::CloseCurrentPopup(); }
		ImGui::EndPopup();
	}


	//////////////////////////////////////////////////////////////////////////


	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Save Map", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{

		if (m_FilePathList.size() == 0)
		{
			m_FilePathList.clear();
			_tfinddata64_t fd;
			__int64 handle = _tfindfirst64(TEXT("../bin/Resources/Data/Map/*.*"), &fd);
			if (handle == -1 || handle == 0)
				return E_FAIL;

			_int iResult = 0;

			//char szCurPath[128] = "../bin/Resources/Data/Map/";
			//char szFullPath[128] = "";
			char szFilename[MAX_PATH];

			while (iResult != -1)
			{
				if (!lstrcmp(fd.name, L".") || !lstrcmp(fd.name, L".."))
				{
					iResult = _tfindnext64(handle, &fd);
					continue;
				}


				WideCharToMultiByte(CP_UTF8, 0, fd.name, -1, szFilename, sizeof(szFilename), NULL, NULL);
				//strcpy_s(szFullPath, szCurPath);
				//strcat_s(szFullPath, szFilename);
				m_FilePathList.push_back({ szFilename });


				iResult = _tfindnext64(handle, &fd);
			}


			_findclose(handle);

		}

		ImGui::Text("Save Map!\n\nExist MapDataFiles");

		static ImGuiTextFilter filter;

		char	szCheckforSameFileName[256]="";

		if (ImGui::BeginListBox(" "))
		{
			auto iter = m_FilePathList.begin();


			for (; iter != m_FilePathList.end(); iter++)
			{
				const bool is_selected = false;

				if (filter.PassFilter(iter->c_str()))
				{
					if (ImGui::Selectable(iter->c_str(), is_selected))
					{
						strcpy_s(filter.InputBuf, iter->c_str());
					}

					if (!strcmp(iter->c_str(), filter.InputBuf))
						strcpy_s(szCheckforSameFileName, filter.InputBuf);
				}
			}
			ImGui::EndListBox();

		}

		filter.Draw("Input FileName");


		ImGui::Separator();
		if (ImGui::Button("Save", ImVec2(120, 0)))
		{

			if (strcmp(filter.InputBuf, ""))
			{

				if (!strcmp(szCheckforSameFileName, filter.InputBuf))
				{
					ImGui::OpenPopup("One More Check");
				}
				else 
				{
					//실제 저장

					Sava_Data(filter.InputBuf, Data_Map);

					ImGui::CloseCurrentPopup();
					m_FilePathList.clear();


				}
			}



		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup();
			m_FilePathList.clear();
		}


		//서브 팝업
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		if (ImGui::BeginPopupModal("One More Check", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{

			ImGui::Text("Mapdata Already Exist\nDo you want to Override on it?");

			if (ImGui::Button("Ok", ImVec2(130, 0)))
			{

				//실제 저장
				Sava_Data(filter.InputBuf, Data_Map);

				ImGui::CloseCurrentPopup();
				m_FilePathList.clear();
			}

			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(130, 0)))
			{
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}


		ImGui::EndPopup();
	}


	//////////////////////////////////////////////////////////////////////////

	// Always center this window when appearing
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Laod Map", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Laod Map!\n\n");
		ImGui::Separator();


		if (m_FilePathList.size() == 0)
		{
			m_FilePathList.clear();
			_tfinddata64_t fd;
			__int64 handle = _tfindfirst64(TEXT("../bin/Resources/Data/Map/*.*"), &fd);
			if (handle == -1 || handle == 0)
				return E_FAIL;

			_int iResult = 0;

			//char szCurPath[128] = "../bin/Resources/Data/Map/";
			//char szFullPath[128] = "";
			char szFilename[MAX_PATH];

			while (iResult != -1)
			{
				if (!lstrcmp(fd.name, L".") || !lstrcmp(fd.name, L".."))
				{
					iResult = _tfindnext64(handle, &fd);
					continue;
				}


				WideCharToMultiByte(CP_UTF8, 0, fd.name, -1, szFilename, sizeof(szFilename), NULL, NULL);
				//strcpy_s(szFullPath, szCurPath);
				//strcat_s(szFullPath, szFilename);
				m_FilePathList.push_back({ szFilename });


				iResult = _tfindnext64(handle, &fd);
			}


			_findclose(handle);

		}



		static ImGuiTextFilter filter;

		char	szCheckforSameFileName[256] = "";

		if (ImGui::BeginListBox(" "))
		{
			auto iter = m_FilePathList.begin();


			for (; iter != m_FilePathList.end(); iter++)
			{
				const bool is_selected = false;

				if (filter.PassFilter(iter->c_str()))
				{
					if (ImGui::Selectable(iter->c_str(), is_selected))
					{
						strcpy_s(filter.InputBuf, iter->c_str());
					}

					if (!strcmp(iter->c_str(), filter.InputBuf))
						strcpy_s(szCheckforSameFileName, filter.InputBuf);
				}
			}
			ImGui::EndListBox();

		}

		filter.Draw("Input FileName");




		if (ImGui::Button("OK", ImVec2(120, 0)))
		{ 

			if (strcmp(filter.InputBuf, ""))
			{
				if (!strcmp(szCheckforSameFileName, filter.InputBuf))
				{
					Load_Data(filter.InputBuf, Data_Map);
					m_FilePathList.clear();
					ImGui::CloseCurrentPopup();
				}
			}
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0))) {
			m_FilePathList.clear();
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
	return S_OK;
}

HRESULT CScene_Edit::RenewElenmetTransform(OBJELEMENT * pObjElement)
{
	if (pObjElement == nullptr)
		return E_FAIL;

	_Matrix Trans = XMMatrixTranslation(pObjElement->matSRT.m[2][0], pObjElement->matSRT.m[2][1], pObjElement->matSRT.m[2][2]);
	_Matrix RotX = XMMatrixRotationX(XMConvertToRadians(pObjElement->matSRT.m[1][0]));
	_Matrix RotY = XMMatrixRotationY(XMConvertToRadians(pObjElement->matSRT.m[1][1]));
	_Matrix RotZ = XMMatrixRotationZ(XMConvertToRadians(pObjElement->matSRT.m[1][2]));
	_Matrix Scale = XMMatrixScaling((pObjElement->matSRT.m[0][0]), (pObjElement->matSRT.m[0][1]), (pObjElement->matSRT.m[0][2]));


	
	//pObjElement->matTransform = XMMatrixMultiply(XMMatrixMultiply(XMMatrixMultiply((XMMatrixMultiply(Scale, RotX)), RotY), RotZ), Trans);
	pObjElement->matTransform = Scale* RotX *RotY* RotZ* Trans;

	if (pObjElement->pObject != nullptr)
	{
		CTransform* pTrans = (CTransform*)(pObjElement->pObject->Get_Component(TAG_COM(Com_Transform)));
		pTrans->Set_Matrix(pObjElement->matTransform);
	}

	return S_OK;
}

#pragma endregion MapTab

#pragma region UITab
HRESULT CScene_Edit::Update_UITab(_double fDeltatime)
{




	return S_OK;
}


#pragma endregion UITab

#pragma region ParticleTab

HRESULT CScene_Edit::Update_ParticleTab(_double fDeltatime)
{
	return S_OK;
}

#pragma endregion ParticleTab


#pragma region CamTab

HRESULT CScene_Edit::Update_CameraActionTab(_double fDeltatime)
{
	return S_OK;
}

#pragma endregion CamTab



#endif // USE_IMGUI
HRESULT CScene_Edit::Ready_Layer_MainCamera(const _tchar * pLayerTag)
{

	CCamera::CAMERADESC CameraDesc;
	CameraDesc.vWorldRotAxis = _float3(0, 0, 0);
	CameraDesc.vEye = _float3(0, 5.f, -10.f);
	CameraDesc.vAt = _float3(0, 0.f, 0);
	CameraDesc.vAxisY = _float3(0, 1, 0);

	CameraDesc.fFovy = XMConvertToRadians(60.f);
	CameraDesc.fAspect = _float(g_iWinCX) / g_iWinCY;
	CameraDesc.fNear = 0.2f;
	CameraDesc.fFar = 300.f;

	CameraDesc.iWinCX = g_iWinCX;
	CameraDesc.iWinCY = g_iWinCY;

	CameraDesc.TransformDesc.fMovePerSec = 5.f;
	CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(60.0f);
	CameraDesc.TransformDesc.fScalingPerSec = 1.f;


	CGameInstance* pInstance = GetSingle(CGameInstance);

	list<CGameObject*>* MainCameraList = pInstance->Get_ObjectList_from_Layer(SCENE_STATIC, TAG_LAY(Layer_Camera_Main));

	if (MainCameraList != nullptr)
	{
		auto iter = MainCameraList->begin();
		for (; iter != MainCameraList->end();)
		{
			Safe_Release(*(iter));
			iter = MainCameraList->erase(iter);
		}
		MainCameraList->clear();
	}

	m_pEditorCam = (CCamera_Editor*)(pInstance->Get_GameObject_By_LayerIndex(SCENE_EDIT, TAG_LAY(Layer_Camera_Main)));

	if (m_pEditorCam == nullptr)
	{
		FAILED_CHECK(pInstance->Add_GameObject_To_Layer(SCENEID::SCENE_EDIT, pLayerTag, TAG_OP(Prototype_Camera_Editor), &CameraDesc));

		m_pEditorCam = (CCamera_Editor*)(pInstance->Get_GameObject_By_LayerIndex(SCENE_EDIT, TAG_LAY(Layer_Camera_Main)));

		NULL_CHECK_RETURN(m_pEditorCam, E_FAIL);

		// 초기 새팅
		//m_pEditorCam

		//
	}
	else
	{
		//리셋

		m_pEditorCam->Set_NowSceneNum(SCENE_EDIT);

	}



	return S_OK;
}


CScene_Edit * CScene_Edit::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CScene_Edit* pTempGame = new CScene_Edit(pDevice, pDeviceContext);

	if (FAILED(pTempGame->Initialize()))
	{
		Safe_Release(pTempGame);
		MSGBOX("Failed to Creating Scene_Stage1");
		return nullptr;
	}

	return pTempGame;
}

void CScene_Edit::Free()
{
	__super::Free();

	for (auto& iter : m_vecBatchedObject)
	{
		//객채 없애기 죽이기 비워주기
		Safe_Release(iter.pObject);
	}
	m_vecBatchedObject.clear();


	Safe_Release(m_pGameInstance);
}
