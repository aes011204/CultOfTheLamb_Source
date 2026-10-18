#pragma once

USING(std);
USING(Engine);

typedef struct tagBossData
{
    _vec3 startpos;
}         BossConfig;

struct SpriteBounceDesc
{
    // 일단 바운스 기준으로 필요 한거
    _vec3 pos;// 위치
    _vec3 velocity;// 초기 힘
    float groundY            = 0.f;// 땅 위치
    float gravity            = -980.f;
    float restitution        = 0.5f; // 튕김 계수
    float damping            = 0.9f; // 수평 속도 감쇠
    float m_DampingPerSecond = 0.98f; // 공기저항
};

struct RecipeInst
{
    int RecipeID;
    int StockNum;
    int ResultItemID;
};

// 선택지 구조체
struct DialogueChoice
{
    string leftText;           // 왼쪽 선택지 ("살해")
    string rightText;          // 오른쪽 선택지 ("노예 ㄱㄱ")
    string leftNextId;
    string rightNextId;
};

// 대사 한 줄
struct DialogueLine
{
    _int   lineIndex;           // 라인 순서 (0, 1, 2...)
    string speaker;             // 말하는 놈 ("보스", "플레이어", "")
    string text;                // 대사 내용
    _bool  hasChoices;          // 선택지가 있는 라인인지

    DialogueChoice choice;           // 선택지가 있을 때
};

// 대화 묶음 (하나의 대화 시퀀스)
struct Dialogue
{
    string dialogueId;               // 대화 ID ("boss1_intro", "boss1_battle")
    string speaker;                  // 말하는 놈

    vector<DialogueLine> lines;      // 대사 라인들
};

struct SavedBuilding
{
    _vec3         pos;
    BUILDING_TYPE type;
    _bool         isBuilt = false;
};

struct SavedInteractive
{
    _vec3  pos    = { 0.f, 0.f, 0.f };
    _float hp     = 0.f;
    _bool  isDead = false;
};

struct SavedNpc
{
    wstring name;
    _bool   isActive             = false;
    _bool   firstSpawnAnimPlayed = false;
};

struct SavedSceneState
{
    vector<SavedBuilding>    savedBuildings;
    vector<SavedInteractive> savedInteractives;
    map<wstring, SavedNpc>   savedNPCs;
};


