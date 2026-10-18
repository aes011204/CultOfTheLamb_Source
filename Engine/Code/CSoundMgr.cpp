#include "CSoundMgr.h"

IMPLEMENT_SINGLETON(CSoundMgr)

CSoundMgr::CSoundMgr()
    : m_System(nullptr),
      m_Channel{},
      m_SoundGroups{}
{ }

CSoundMgr::~CSoundMgr()
{
    CSoundMgr::Free();
}

void CSoundMgr::Ready_SoundMgr()
{
    FMOD_System_Create(&m_System);
    FMOD_System_Init(m_System, 32, FMOD_INIT_NORMAL, nullptr);

    FMOD_System_CreateChannelGroup(m_System, "BGM", &m_SoundGroups[SOUND_BGM]);
    FMOD_System_CreateChannelGroup(m_System, "Effect", &m_SoundGroups[SOUND_EFFECT]);
    FMOD_System_CreateChannelGroup(m_System, "UI", &m_SoundGroups[SOUND_UI]);
    LoadSoundFile();
}

void CSoundMgr::Update(const _float& timeDelta)
{
    NULL_CHECK(m_System);

    FMOD_System_Update(m_System);

    // Fade Out 처리
    auto iter = m_FadeChannels.begin();
    while (iter != m_FadeChannels.end())
    {
        iter->currVolume -= iter->fadeSpeed * timeDelta;

        if (iter->currVolume <= 0.f)
        {
            FMOD_Channel_Stop(iter->channel);
            iter = m_FadeChannels.erase(iter);
        }
        else
        {
            FMOD_Channel_SetVolume(iter->channel, iter->currVolume);
            ++iter;
        }
    }
}

void CSoundMgr::Play_Once(const wstring& soundFile, SOUNDTYPE soundType, _float volume)
{
    NULL_CHECK_MSG(m_System, L"FMod System is Null");

    if (soundType < SOUND_BGM || soundType > SOUND_END)
        return;

    wstring key = soundFile;
    transform(key.begin(), key.end(), key.begin(), ::towlower);

    auto iter = m_Sounds.find(key);
    if (iter == m_Sounds.end())
        return;

    FMOD_RESULT fmodResult = FMOD_System_PlaySound(
        m_System,
        iter->second,
        m_SoundGroups[soundType],
        TRUE,
        &m_Channel[soundType]);

    // FMOD_LOOP_OFF 설정으로 1회만 재생
    if (fmodResult == FMOD_OK && m_Channel[soundType])
    {
        FMOD_Channel_SetMode(
            m_Channel[soundType],
            FMOD_LOOP_OFF);

        FMOD_Channel_SetVolume(
            m_Channel[soundType],
            Clamp(volume, 0.f, 1.f));

        FMOD_Channel_SetPaused(m_Channel[soundType], FALSE);
    }
}

void CSoundMgr::Play_BGM(const wstring& soundFile,
                          _float         volume,
                          _bool          stopPrevBGM,
                          _float         fadeOutDuration)
{
    NULL_CHECK_MSG(m_System, L"FMod System is Null");

    wstring key = soundFile;
    transform(key.begin(), key.end(), key.begin(), ::towlower);

    auto iter = m_Sounds.find(key);
    if (iter == m_Sounds.end())
        return;

    if (stopPrevBGM)
        Stop_SoundChannel(SOUND_BGM, fadeOutDuration);

    CleanupStoppedBGMChannels();

    FMOD_CHANNEL* fmodChannel = nullptr;

    FMOD_RESULT fmodResult = FMOD_System_PlaySound(
        m_System,
        iter->second,
        m_SoundGroups[SOUND_BGM],
        TRUE,
        &fmodChannel);

    // FMOD_LOOP_NORMAL 설정으로 반복 재생
    if (fmodResult == FMOD_OK && fmodChannel)
    {
        FMOD_Channel_SetMode(fmodChannel, FMOD_LOOP_NORMAL);
        FMOD_Channel_SetVolume(fmodChannel, Clamp(volume, 0.f, 1.f));
        FMOD_Channel_SetPaused(fmodChannel, FALSE);
        m_BGMChannels.push_back(fmodChannel);
        m_Channel[SOUND_BGM] = fmodChannel;
    }
}

void CSoundMgr::SetChannelVolume(SOUNDTYPE soundType, _float volume)
{
    NULL_CHECK(m_System);

    if (soundType < SOUND_BGM || soundType > SOUND_END)
        return;

    FMOD_ChannelGroup_SetVolume(m_SoundGroups[soundType], Clamp(volume, 0.f, 1.f));
}

void CSoundMgr::Stop_SoundChannel(SOUNDTYPE soundType, _float fadeOutDuration)
{
    NULL_CHECK(m_System);

    if (soundType < SOUND_BGM || soundType > SOUND_END)
        return;

    // BGM이면서 페이드 아웃 시간이 설정된 경우
    if (soundType == SOUND_BGM && fadeOutDuration > 0.f)
    {
        for (FMOD_CHANNEL* fmodChannel : m_BGMChannels)
        {
            if (!fmodChannel) continue;

            _float fCurrentVolume = 0.f;
            FMOD_Channel_GetVolume(fmodChannel, &fCurrentVolume);

            // 이미 볼륨이 0이면 그냥 정지
            if (fCurrentVolume <= 0.f)
            {
                FMOD_Channel_Stop(fmodChannel);
                continue;
            }

            FADE_INFO info;
            info.channel       = fmodChannel;
            info.currVolume = fCurrentVolume;
            info.fadeSpeed     = fCurrentVolume / fadeOutDuration;

            m_FadeChannels.push_back(info);
        }

        m_BGMChannels.clear();
        m_Channel[SOUND_BGM] = nullptr;
        return;
    }

    // 그 외의 경우 (즉시 정지)
    FMOD_ChannelGroup_Stop(m_SoundGroups[soundType]);

    if (soundType == SOUND_BGM)
    {
        m_BGMChannels.clear();
        m_Channel[SOUND_BGM] = nullptr;
    }
    else
        m_Channel[soundType] = nullptr;
}

void CSoundMgr::Stop_Sound(const wstring& soundFile)
{
    NULL_CHECK(m_System);

    wstring key = soundFile;
    transform(key.begin(), key.end(), key.begin(), ::towlower);

    auto iter = m_Sounds.find(key);
    if (iter == m_Sounds.end())
        return;

    FMOD_SOUND* pTargetSound = iter->second;
    vector<FMOD_CHANNEL*> channelsToStop;

    // 모든 채널 그룹(BGM/EFFECT)을 순회하며 해당 사운드를 재생 중인 채널 검색
    for (_int i = 0; i < SOUND_END; ++i)
    {
        int iNumChannels = 0;
        FMOD_ChannelGroup_GetNumChannels(m_SoundGroups[i], &iNumChannels);

        for (int j = 0; j < iNumChannels; ++j)
        {
            FMOD_CHANNEL* pChannel = nullptr;
            FMOD_ChannelGroup_GetChannel(m_SoundGroups[i], j, &pChannel);

            if (!pChannel) continue;

            FMOD_BOOL bIsPlaying = FALSE;
            FMOD_Channel_IsPlaying(pChannel, &bIsPlaying);

            if (!bIsPlaying) continue;

            FMOD_SOUND* pCurrentSound = nullptr;
            FMOD_Channel_GetCurrentSound(pChannel, &pCurrentSound);

            if (pCurrentSound == pTargetSound)
            {
                channelsToStop.push_back(pChannel);
            }
        }
    }

    // 수집된 채널들 정지
    for (auto& pChannel : channelsToStop)
    {
        FMOD_Channel_Stop(pChannel);
    }
}

void CSoundMgr::Stop_All()
{
    NULL_CHECK(m_System);

    for (_int i = 0; i < SOUND_END; ++i)
    {
        Stop_SoundChannel(static_cast<SOUNDTYPE>(i));
    }
}

void CSoundMgr::LoadSoundFile()
{
    USING(filesystem);

    const path root = L"../Bin/Resource/Sound/";

    if (!exists(root))
        return;

    for (const auto& directoryEntry : recursive_directory_iterator(root))
    {
        if (!directoryEntry.is_regular_file())
            continue;

        // 파일 확장자 체크
        auto ext = directoryEntry
                   .path()
                   .extension()
                   .wstring();

        // 안전하게 소문자로 변환
        transform(ext.begin(), ext.end(), ext.begin(), ::towlower);

        if (ext != L".wav" && ext != L".ogg" && ext != L".mp3")
            continue;

        wstring key = relative(directoryEntry.path(), root).generic_wstring();
        transform(key.begin(), key.end(), key.begin(), ::towlower);

        FMOD_MODE mode = FMOD_DEFAULT;

        // 파일 상대 경로 체크
        auto rel = relative(directoryEntry.path(), root);

        // 상대 경로의 첫번째가 "BGM"이면 FMOD_CREATESTREAM 설정
        if (rel.begin() != rel.end())
        {
            wstring top = rel.begin()->generic_wstring();

            // 안전하게 대문자로 변환
            transform(top.begin(), top.end(), top.begin(), ::toupper);

            if (top == L"BGM")
                mode |= FMOD_CREATESTREAM;
        }

        FMOD_SOUND* fmodSound = nullptr;

        string entryToUTF8 = directoryEntry.path().u8string();

        if (FMOD_System_CreateSound(
                m_System,
                entryToUTF8.c_str(),
                mode,
                nullptr,
                &fmodSound) == FMOD_OK
            && fmodSound)
            m_Sounds.emplace(move(key), fmodSound);
    }
}

void CSoundMgr::CleanupStoppedBGMChannels()
{
    if (m_BGMChannels.empty())
    {
        m_Channel[SOUND_BGM] = nullptr;
        return;
    }

    auto iter = m_BGMChannels.begin();
    while (iter != m_BGMChannels.end())
    {
        FMOD_CHANNEL* fmodChannel = *iter;
        FMOD_BOOL     isPlaying   = FALSE;

        if (fmodChannel
            && FMOD_Channel_IsPlaying(fmodChannel, &isPlaying) == FMOD_OK
            && isPlaying)
            ++iter;
        else
            iter = m_BGMChannels.erase(iter);
    }

    m_Channel[SOUND_BGM] = m_BGMChannels.empty() ? nullptr : m_BGMChannels.back();
}

void CSoundMgr::Free()
{
    for (const auto& pair : m_Sounds)
    {
        if (pair.second)
            FMOD_Sound_Release(pair.second);
    }
    m_Sounds.clear();

    if (m_System)
    {
        FMOD_System_Close(m_System);
        FMOD_System_Release(m_System);
        m_System = nullptr;
    }
}
