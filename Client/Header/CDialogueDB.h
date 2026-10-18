#pragma once

#include "CBase.h"
#include "Client_Struct.h"

class CDialogueDB : public CBase
{
    DECLARE_SINGLETON(CDialogueDB)

private:
    explicit CDialogueDB();
    virtual ~CDialogueDB() override;

public:
    HRESULT     Ready_DialogueDB();
    HRESULT     LoadFromJson(const string& fileName);

    Dialogue*   GetDialogueById(const string& dialogueId);
    wstring     Utf8ToWstring(const string& str);

private:
    vector<Dialogue>            m_Dialogues;
    unordered_map<string, _int> m_DialogueIndex;

private:
    void Free() override;

};

