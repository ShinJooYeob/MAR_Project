#pragma once
#include "Base.h"

BEGIN(Engine)

class CSoundMgr :public CBase
{
	DECLARE_SINGLETON(CSoundMgr);

#define MaxChannelCount 102
#define BGMChannelCount	2

private:
	explicit CSoundMgr();
	virtual ~CSoundMgr() = default;

public:
	HRESULT Initialize_FMOD();
	HRESULT Update_FMOD(_double fTimeDelta);

public:
	_int  Channel_VolumeUp(CHANNELID eID, _float _vol);
	_int  Channel_VolumeDown(CHANNELID eID, _float _vol);
	_int  Channel_Pause(CHANNELID eID);

	HRESULT PlaySound(const _tchar* pSoundKey, CHANNELID eID, SOUNDDESC* tSoundDesc, SOUNDDESC** pOutDesc);
	HRESULT PlayBGM(const _tchar* pSoundKey, _float fLouderMultiple = 1.f);

	void Stop_ChannelSound(CHANNELID eID);
	void Stop_AllChannel();

	_float  Get_Channel_Volume(CHANNELID eID);
	_bool  Get_Channel_IsPaused(CHANNELID eID);

private:
	_double	m_fPassedTimeArr[MaxChannelCount];
	_float	m_VolumeArr[CHANNEL_MAXCHANNEL];
	_bool	m_PauseArr[CHANNEL_MAXCHANNEL];
	SOUNDDESC m_tSoundDesc[MaxChannelCount];
	_float3 m_ListenerPosition;

	_uint  m_iBGMIndex = 0;
	_bool  m_bChangingBGM = false;
	_double m_ChangingBGMTime = 0;

private:
	HRESULT LoadSoundFile();

private:
	// ���� ���ҽ� ������ ���� ��ü 
	map<TCHAR*, FMOD_SOUND*> m_mapSound;
	// FMOD_CHANNEL : ����ϰ� �ִ� ���带 ������ ��ü 
	FMOD_CHANNEL* m_pChannelArr[MaxChannelCount];
	// ���� ,ä�� ��ü �� ��ġ�� �����ϴ� ��ü 
	FMOD_SYSTEM* m_pSystem;
	const _uint		m_iNumOfEachChannel;
	class CPipeLineMgr*		m_pPipeLineMgr= nullptr;
	class CEasingMgr*		m_pEasingMgr = nullptr;
public:
	virtual void Free() override;
};


END

