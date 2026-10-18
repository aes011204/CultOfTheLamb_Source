#include "pch.h"

#ifdef _DEBUG
#ifdef new
#undef new
#endif
#endif

#include <fstream>
#include <nlohmann/json.hpp>
#include "CDialogueDB.h"

#ifdef _DEBUG
#define new DBG_NEW
#endif

IMPLEMENT_SINGLETON(CDialogueDB)

CDialogueDB::CDialogueDB()
{
}

CDialogueDB::~CDialogueDB()
{
    CDialogueDB::Free();
}

HRESULT CDialogueDB::Ready_DialogueDB()
{

    return S_OK;
}

HRESULT CDialogueDB::LoadFromJson(const string& fileName)
{
    ifstream file(fileName, ios_base::in);

    if (!file.is_open())
    {
        MSG_BOX("Dialogue Json File Load Failed");
        return E_FAIL;
    }

    nlohmann::json j;

    // BOM 제거 처리
    char c;
    file.read(&c, 1);
    if (static_cast<unsigned char>(c) != 0xEF)
        file.seekg(0);
    else
    {
        char c2, c3;
        file.read(&c2, 1);
        file.read(&c3, 1);
    }

    try
    {
        file >> j;
    }
    catch (const nlohmann::json::parse_error& e)
    {
        MessageBoxA(nullptr, e.what(), "JSON Parse Error", MB_OK);
        return E_FAIL;
    }

    // 대화 파싱
    for (auto& node : j["dialogues"])
    {
        Dialogue dialogue;
        dialogue.dialogueId = node["dialogueId"];
        dialogue.speaker = node.value("speaker", "");

        // 라인 파싱
        for (auto& lineNode : node["lines"])
        {
            DialogueLine line;
            line.lineIndex = lineNode["lineIndex"];
            line.speaker = lineNode.value("speaker", dialogue.speaker);
            line.text = lineNode["text"];
            line.hasChoices = lineNode.value("hasChoices", false);

            // 선택지 파싱 (좌/우 2개)
            if (line.hasChoices && lineNode.contains("choice"))
            {
                auto& choiceNode = lineNode["choice"];

                line.choice.leftText = choiceNode["leftText"];
                line.choice.rightText = choiceNode["rightText"];
                line.choice.leftNextId = choiceNode.value("leftNextId", "");
                line.choice.rightNextId = choiceNode.value("rightNextId", "");
            }

            dialogue.lines.push_back(line);
        }

        m_DialogueIndex[dialogue.dialogueId] = m_Dialogues.size();
        m_Dialogues.push_back(dialogue);
    }

    return S_OK;
}

Dialogue* CDialogueDB::GetDialogueById(const string& dialogueId)
{
    auto it = m_DialogueIndex.find(dialogueId);
    if (it == m_DialogueIndex.end())
        return nullptr;

    return &m_Dialogues[it->second];
}

wstring CDialogueDB::Utf8ToWstring(const string& str)
{
    if (str.empty())
        return wstring();

    _int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);

    wstring result(size_needed, 0);

    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &result[0], size_needed);

    if (!result.empty() && result.back() == L'\0')
        result.pop_back();

    return result;
}

void CDialogueDB::Free()
{

}

