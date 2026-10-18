#pragma once
#include "CBase.h"
#include "Engine_Define.h"

BEGIN(Engine)
/// @brief FMod를 사용하여 다중채널 사운드 재생을 관리하는 싱글톤 매니저 클래스
class ENGINE_DLL CSoundMgr : public CBase
{
    DECLARE_SINGLETON(CSoundMgr)

private:
    explicit CSoundMgr();
    ~CSoundMgr() override;

public:
    void Ready_SoundMgr();
    void Update(const _float& timeDelta);

public:  // --- 주요 기능 ---

    /// @brief 일회성 사운드 재생 기능
    /// @param soundFile  Sound 폴더부터의 주소 (ex. Player/Player_Walk2.wav)
    /// @param soundType  사운드 채널 타입
    /// @param volume     볼륨. 0~1 사이로 조절
    /// @note 한번 조건에 한 번만 실행될 수 있도록 구성할 것
    void Play_Once(const wstring& soundFile,
                   SOUNDTYPE      soundType,
                   _float         volume);

    /// @brief BGM용 반복 재생 기능
    /// @param soundFile        Sound 폴더부터의 주소 (ex. BGM/TestBGM.wav)
    /// @param volume           볼륨. 0~1 사이로 조절
    /// @param stopPrevBGM      재생중이던 모든 BGM을 끄고 새로 재생할건지 여부. false를 사용하면 이전 BGM과 함께 재생
    /// @param fadeOutDuration  페이드 아웃 시간 (초). 0이면 즉시 정지
    void Play_BGM(const wstring& soundFile,
                  _float         volume,
                  _bool          stopPrevBGM     = true,
                  _float         fadeOutDuration = 0.f);

    /// @brief 사운드 채널 단위로 소리를 조절할 때만 사용
    /// @param soundType  사운드 채널 타입
    /// @param volume     볼륨. 0~1 사이로 조절
    void SetChannelVolume(SOUNDTYPE soundType, _float volume);

    /// @brief 특정 채널 사운드 정지 기능
    /// @param soundType        사운드 채널 타입
    /// @param fadeOutDuration  페이드 아웃 시간 (초). 0이면 즉시 정지
    /// @note BGM을 수동으로 끌 때 쓰면 됩니다.
    void Stop_SoundChannel(SOUNDTYPE soundType, _float fadeOutDuration = 0.f);

    /// @brief 특정 키 값(파일 경로)에 해당하는 사운드를 찾아 모든 채널에서 정지합니다.
    /// @param soundFile        정지할 사운드 파일 경로 (Play_Once 등에 사용한 키)
    void Stop_Sound(const wstring& soundFile);

    /// @brief 모든 채널 정지 기능
    /// @note 씬 넘어가는 상황 등 소리 다 끌때 쓰면 됩니다.
    void Stop_All();

private: // --- 내부 로직 ---
    void LoadSoundFile();
    void CleanupStoppedBGMChannels();

private:
    // BGM 소리 페이드아웃 기능을 위한 정보 구조체
    typedef struct FadeChannelInfo
    {
        FMOD_CHANNEL* channel;
        _float        currVolume;
        _float        fadeSpeed;
    }
    FADE_INFO;

private:
    FMOD_SYSTEM*       m_System;
    FMOD_CHANNEL*      m_Channel[SOUND_END];
    FMOD_CHANNELGROUP* m_SoundGroups[SOUND_END];

    unordered_map<wstring, FMOD_SOUND*> m_Sounds;
    vector<FMOD_CHANNEL*>               m_BGMChannels;
    list<FADE_INFO>                     m_FadeChannels;

private:
    void Free() override;
};

END
