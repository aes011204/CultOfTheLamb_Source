#pragma once
#include "CGameObject.h"
#include "Client_Enum.h"

BEGIN(Engine)
class CRcTex;
class CTransform;
class CTexture;
class CAnimator;
class CBoxCollider;
class CCombatStat;
class CAttackBox;
END

class CProjectile;
class CBossBulletAttack;
class CTrailEff;

class CBossBullet :
    public CGameObject
{
private:
    explicit CBossBullet(DEVICE graphicDev, CGameObject* owner, CGameObject* Target, PROJ_TYPE phase);
    explicit CBossBullet(const CBossBullet& rhs);
    virtual ~CBossBullet();

public:

    virtual HRESULT Ready_GameObject() override;
    virtual void    Initialize_GameObject(); // Ready보다 한 단계 아래 생명주기
    virtual _int    Update_GameObject(const _float& timeDelta) override;
    virtual void    LateUpdate_GameObject(const _float& timeDelta) override;
    virtual void    Render_GameObject() override;
    void Render_Trail();

    void Render_Core();


    virtual void OnBeginOverlap(CCollider* self, CCollider* other) override;
    virtual void OnEndOverlap(CCollider* self, CCollider* other) override;

    void Animation_Setting();
    HRESULT Add_Component();

    void SetPendingKill() { Mark_As_PendingKill(); }

public:

    static CBossBullet* Create(DEVICE graphicDev, CGameObject* owner, CGameObject* Target, PROJ_TYPE type);

    void Start_Shoot() { m_StartShoot = true; }
    void Set_TargetDir(_vec3 dir) { m_TargetDir = dir; }



protected:

    CRcTex* m_BufferCom;
    CTransform* m_TransformCom;
    CTexture* m_TextureCom;
    CAnimator* m_AnimatorCom;
    CBoxCollider* m_BoxColCom;
    CCombatStat* m_CombatStatCom;

    CGameObject* m_pOwner;
    CGameObject* m_pTarget;
    //CAttackBox* ATBox = nullptr;
    CTransform* m_ATKBoxTransformCom = nullptr;
    CProjectile* m_ProjectileCom;

    CTrailEff* m_TrailEff;

    PROJ_TYPE m_type;

    bool m_flag = false;

    _vec3 m_StartPos;
    _vec3 m_TargetDir;
    bool m_StartShoot = false;
    bool m_TargetFlag = false;
    _vec3 m_MoveLength ;

	float DieCountForSafe;
    float m_fAlpha = 0;

    /// 실제 움직이는 변수/ ///

    float m_Speed = 15;


private:
    void Free() override;


};

