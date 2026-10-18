#include "pch.h"
#include "CBossChainAttack.h"
#include "CMonsterState.h"
#include "CBossPhase1.h"
#include "CBossStateMachine.h"
#include "CTransform.h"
#include "CAnimator.h"
#include "CSpriteAnimation.h"
#include "CBossChain.h"
#include "CCreateHelper.h"
#include "CWarningChain.h"

#include "CRandomMgr.h"


   int CBossChainAttack::m_CPatterCount = 0;

CBossChainAttack::CBossChainAttack(CBossPhase1* pOwner, CBossStateMachine* pStateMachine)
{
    m_pOwner = pOwner;
    m_pStateMachine = pStateMachine;
    ZeroMemory(&m_fElapsedTime, sizeof(m_fElapsedTime));


}

CBossChainAttack::~CBossChainAttack()
{
}

void CBossChainAttack::Enter()
{
    CMonsterState::Enter();
    m_pOwnerAnimatorCom->Play_Animation(L"BossAttack", ANIMSTATE::ONCE); // 지금 이게 안들어오거든 바로 상태안의 애니로 감

    int RandPhase = CRandomMgr::GetInstance()->Get_Int_Random(0, ((int)PHASE::PHASE_END)-1); // 페이즈 랜덤
    m_ePhase = (PHASE)RandPhase;

    // 순차재생을 위한 거
    m_CPatterCount %= (int)PHASE::PHASE_END;
    m_ePhase = (PHASE)m_CPatterCount;
    m_CPatterCount++;
    //m_ePhase = PHASE::RANDCHAIN;
    ////
}

void CBossChainAttack::Exit()
{
    CMonsterState::Exit();
    m_StartFlag = false;
}

HRESULT CBossChainAttack::Ready_State()
{
    m_fMinStateTime = 0.f;

    m_pOwnerTransformCom = static_cast<CTransform*>(m_pOwner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));
    m_pOwnerAnimatorCom = static_cast<CAnimator*>(m_pOwner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::ANIMATOR));

    m_CellSize = 2.5f;
    m_DirectionOffset = 2.5f;
    m_MaxLineCount = 7;
    m_Interval = 0.05;

    m_CircleRadius = 3;
    m_GroundY = 0.002f;
    m_CrossOffset=2.f;
    return S_OK;
}

_int CBossChainAttack::Update_State(const _float& timeDelta)
{
    CMonsterState::Update_State(timeDelta);

    Find_Player();

    m_fCurStateElapsedTime += timeDelta;

    if (m_pOwnerAnimatorCom->Get_CurAnimation() == nullptr)
        return CBossPhase1::BOSSPHASE1::CHAINATTACK;

    // 처음 애니는 무조건 공격모션 이어야 함
    if (m_pOwnerAnimatorCom->Get_CurFrame() == m_pOwnerAnimatorCom->Get_MaxFrame() - 1 && m_StartFlag == false)
    {
        m_StartFlag = true;
    }

    if (m_StartFlag == false)
        return CBossPhase1::BOSSPHASE1::CHAINATTACK;

    switch (m_ePhase)
    {
    case PHASE::PLAYERLINE:

            m_pOwnerAnimatorCom->Play_Animation(L"BossIdle", ANIMSTATE::LOOP);
        


        if (PlayerLineUpdate(timeDelta, m_MaxLineCount, m_DirectionOffset, .2f) == true)
        {
            LineCount++;
        }
        //경고이팩트다 나가면 -> 다음 플레이어 위치 캐싱 -> 



        if (/*m_RandCount*/3 <= LineCount)
        {
            // 나가기 
            m_StartFlag = false; 
            m_fCurStateElapsedTime = 0;
            m_Count = 0;
            m_LineFlag = false;
            m_fElapsedTime = 0.f;
            LineCount = 0;
            return CBossPhase1::BOSSPHASE1::BOSSIDLE;

        }

        break;


    case PHASE::CROSS:
    {
        m_pOwnerAnimatorCom->Play_Animation(L"BossIdle", ANIMSTATE::ONCE);

        m_fElapsedTime+= timeDelta;

        int offset = m_CrossOffset;// TOOD:: 나중에 맴버 변수로 오프셋 조정 (사슬과 사슬 사이의 거리)


        if (m_fElapsedTime > .2f && m_Count < 10)
        {
            for (int i = 0;i < 4; i++)
            {

                CWarningChain* WarningChain = CWarningChain::Create(m_pOwner->Get_GraphicDev(), m_pOwner);
                const auto ChainTransform =
                    dynamic_cast<CTransform*>(WarningChain->Get_Component(
                        ID_DYNAMIC,
                        COMPONENTTYPE::TRANSFORM));

                _vec3 OwnerPos = m_pOwnerTransformCom->Get_Pos();
                float distance = ((float)m_Count * offset);

                _vec3 AddPos;
                switch (i)
                {
                case 0: //오
                    AddPos = { distance, 0.f, -easeInQuint10(m_Count) * offset * 5 };
                    break;
                case 1://왼
                    AddPos = { -distance, 0.f,  easeInQuint10(m_Count) * offset * 5 };
                    break;
                case 2://앞
                    AddPos = { easeInQuint10(m_Count) * offset * 5, 0.f,distance };
                    break;
                case 3://뒤
                    AddPos = { -easeInQuint10(m_Count) * offset * 5, 0.f, -distance };
                    break;
                }
                _vec3 TR = OwnerPos + AddPos;
                ChainTransform->Set_Pos({ TR.x, m_GroundY, TR.z });
                
                SpawnObjectOnLayer(WarningChain);

            }

            m_fElapsedTime= 0;
            m_Count++;

        }

        if (m_Count >= 10)
        {
            // 나가기 
            m_fElapsedTime = 0;
            m_Count = 0;
            return CBossPhase1::BOSSPHASE1::BOSSIDLE;
        }
        break;
    }
    case PHASE::RANDCHAIN://움직임 + idle

        if(m_StartPhase == false)
            m_pOwnerAnimatorCom->Play_Animation(L"BossAttack", ANIMSTATE::ONCE);

 /*       cout << m_pOwnerAnimatorCom->Get_CurFrame() << endl;*/
        if (m_pOwnerAnimatorCom->Get_CurFrame() == m_pOwnerAnimatorCom->Get_MaxFrame() - 1 || m_StartPhase == false)
        {
            m_StartPhase = true;
            m_pOwnerAnimatorCom->Play_Animation(L"BossIdle", ANIMSTATE::ONCE);

        }// TODO:: 이걸 공통으로 뺴면 될듯???

        if (m_StartPhase == true)
            m_fElapsedTime+= timeDelta;


        if (m_fElapsedTime < 5.f && m_StartPhase == true)
        {
            _vec3 BaseCenter = m_pOwner->Get_Center();
            int BaseLength = m_pOwner->Get_Length();
            float interval = 0.5f; //  이건 시간
            RandomChain(timeDelta, BaseCenter, BaseLength, interval);
        }
        else if (m_fElapsedTime >= 5.f && m_StartPhase == true)
        {
            // 나가기
            m_StartFlag = false;

            m_StartPhase = false;
            m_fElapsedTime = 0;
            return CBossPhase1::BOSSPHASE1::BOSSIDLE;
        }

        break;

    case PHASE::CIRCLE://움직임 + idle

        m_pOwnerAnimatorCom->Play_Animation(L"BossIdle", ANIMSTATE::ONCE);

        if (m_StartPhase == true)
            m_fElapsedTime += timeDelta;

        
        if (m_pOwnerAnimatorCom->Get_CurFrame() == m_pOwnerAnimatorCom->Get_MaxFrame() - 1 && m_StartPhase == false)
        {
            m_StartPhase = true;
        }

        if (m_StartPhase == true)
        {
            if (CircleChain(timeDelta, 7, m_CircleRadius, 0.1) == true)
            {
                // 나가기
                m_StartFlag = false;

                m_Count = 0;
                m_StartPhase = false;
                m_fElapsedTime = 0;
                return CBossPhase1::BOSSPHASE1::BOSSIDLE;
            }
        }
        break;
    }

    return CBossPhase1::BOSSPHASE1::CHAINATTACK;
}



bool CBossChainAttack::PlayerLineUpdate(const _float& timeDelta, _int  MaxCount, _float m_Offset, float Speed)
{
    m_fElapsedTime += timeDelta;

    if (m_LineFlag == false)
    {
        // 이렇게 하면 y 값이 들어가서 플레이어의 위치에 따라 달라짐 (x,y,z) 다 합쳐서 1 이니까
      //  _vec3 tmp = m_pPlayerTransformCom->Get_Pos() - m_pOwnerTransformCom->Get_Pos();
        _vec3 tmpPlayer = { m_pPlayerTransformCom->Get_Pos().x, 0, m_pPlayerTransformCom->Get_Pos().z };
        _vec3 tmpOwner = {m_pOwnerTransformCom->Get_Pos().x, 0, m_pOwnerTransformCom->Get_Pos().z };

        _vec3 tmp = tmpPlayer - tmpOwner;
        D3DXVec3Normalize(&m_PlayerDir, &tmp);
        m_LineFlag = true;
    }

    if (m_fElapsedTime > Speed && m_Count < MaxCount)
    {

        CWarningChain* WarningChain = CWarningChain::Create(m_pOwner->Get_GraphicDev(), m_pOwner);

        const auto ChainTransform =
            dynamic_cast<CTransform*>(WarningChain->Get_Component(
                ID_DYNAMIC,
                COMPONENTTYPE::TRANSFORM));

        _vec3 OwnerPos = m_pOwnerTransformCom->Get_Pos(); // 오너 기준
        _vec3 AddPos = { (m_PlayerDir.x * m_Count) * m_Offset, 0.f, (m_PlayerDir.z * m_Count) * m_Offset };

        _vec3 TR = OwnerPos + AddPos;
        ChainTransform->Set_Pos({ TR.x, m_GroundY, TR.z });
     
        SpawnObjectOnLayer(WarningChain);

        m_fElapsedTime = 0;

        m_Count++;

    }

    if (m_Count >= MaxCount)
    {
        // 다음 패턴을 위해 초기화
        m_Count = 0;
        m_LineFlag = false;
        m_fElapsedTime = 0.f;

        return true;  // 패턴 1회 완료
    }
    return false;

}

void CBossChainAttack::RandomChain(const _float& timeDelta, _vec3 BaseCenter, int BaseLenght, float interval)
{
    _vec3 LT = { BaseCenter.x - BaseLenght * 0.5f, 0.f, BaseCenter.z + BaseLenght * 0.5f };
    _vec3 RB = { BaseCenter.x + BaseLenght * 0.5f,0.f,BaseCenter.z - BaseLenght * 0.5f };

    
    int xCell = BaseLenght / m_CellSize;
    int zCell = BaseLenght / m_CellSize;

    //float r = std::fmod((m_fElapsedTime[(int)PHASE::RANDCHAIN]), interval); // 나머지 구하는거, 소숫점에서도 가능

    m_RandChainTime += timeDelta;

    //if (fabs(r) < 1e-6f)// ㄱ의 절댓값(음수x) 가 0 근사값이면 실행
    //{
    int count = 0;
    if (m_RandChainTime > interval)
    {
        for (int i = 0; i < 10; i++)
        {
            CWarningChain* WarningChain = CWarningChain::Create(m_pOwner->Get_GraphicDev(), m_pOwner);
            const auto ChainTransform =
                dynamic_cast<CTransform*>(WarningChain->Get_Component(
                    ID_DYNAMIC,
                    COMPONENTTYPE::TRANSFORM));


            int tmpx = CRandomMgr::GetInstance()->Get_Int_Random(0, xCell); // x 몇번쨰 칸인지는 랜덤
            int tmpz = CRandomMgr::GetInstance()->Get_Int_Random(0, zCell); // x 몇번쨰 칸인지는 랜덤

            float xPos = tmpx * (m_CellSize );
            float zPos = tmpz * (m_CellSize );

            ChainTransform->Set_Pos(LT.x + xPos, m_GroundY, LT.z - zPos);

            SpawnObjectOnLayer(WarningChain);
  
        }
            count++;
            //cout << count << endl;
        m_RandChainTime = 0;
    }


    //}

}

bool CBossChainAttack::CircleChain(const _float& timeDelta, int num, int length, float interval)
{

    if (m_fElapsedTime >= interval)
    {

        //for (int i = 0; i < num; i++)
        //{
        CWarningChain* WarningChain = CWarningChain::Create(m_pOwner->Get_GraphicDev(), m_pOwner);
        const auto ChainTransform =
            dynamic_cast<CTransform*>(WarningChain->Get_Component(
                ID_DYNAMIC,
                COMPONENTTYPE::TRANSFORM));


        _vec3 ownerPos = m_pOwnerTransformCom->Get_Pos();

        //_vec3 baseDir = { 0,0,1 };

        float rad = D3DXToRadian(360.f / (num));

        float addRad = rad * m_Count;

        _vec3 circle = { cos(addRad) * length, 0.f, sin(addRad) * length };

        _vec3 TR = { ownerPos + circle };
        ChainTransform->Set_Pos({ TR.x, m_GroundY,TR.z });

        SpawnObjectOnLayer(WarningChain);

        
        m_Count++;
        m_fElapsedTime = 0;
        //}
    }
    if (m_Count == num)
    {
        m_Count = 0;
        return true;
    }
    return false;
}
void CBossChainAttack::LateUpdate_State(const _float& timeDelta)
{
}

void CBossChainAttack::Render_State()
{
}

float CBossChainAttack::easeInQuint10(float x)
{
    float tmp = x / 10.f;
    return pow(tmp, 4.f);
}


CBossChainAttack* CBossChainAttack::Create(CBossPhase1* pOwner, CBossStateMachine* pStateMachine)
{
    auto BossCA = new CBossChainAttack(pOwner, pStateMachine);

    if (FAILED(BossCA->Ready_State()))
    {
        MSG_BOX("BossCA Create Failed");
        Safe_Release(BossCA);
        return nullptr;
    }

    return BossCA;
}

void CBossChainAttack::Free()
{
}
