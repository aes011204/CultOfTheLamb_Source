# CultOfTheLamb-DX9-Source

**C++ / DirectX 9 기반 Cult of the Lamb 모작 팀 프로젝트의 소스 코드입니다.**

게임의 전투, 보스 패턴, 아이템, 채집, 요리 등 게임플레이를 구현한 프로젝트입니다. 이 저장소는 팀 프로젝트의 전체 소스 구조를 공개하며, 아래에 `aes011204`의 주요 담당 및 기여 내용을 별도로 정리했습니다.

> 게임 이미지·음원 등 실행 리소스는 포함하지 않습니다. 기존 Git 이력, IDE 캐시, 빌드 결과물 및 외부 라이브러리의 DLL/LIB도 제외되어 있어, 저장소만 내려받아 바로 실행할 수는 없습니다.

## 개발 환경

| 항목 | 기술 |
| --- | --- |
| 언어 | C++ |
| 플랫폼 | Windows |
| 그래픽 / 입력 | DirectX 9, D3DX9, DirectInput |
| 개발 도구 | Visual Studio 2022, MSVC v143, Windows SDK 10 |
| UI 및 편집 도구 | Dear ImGui |
| 사운드 | FMOD |
| 데이터 | JSON, nlohmann/json |

## 담당 및 기여 — aes011204

원본 저장소의 작성자별 커밋과 `feature/Item-main`, `feature/inventory-test`, `feature-boss-sub` 관련 작업 기록을 기준으로 정리했습니다. 아래 링크는 현재 공개한 소스 기준이며, 연결된 파일에는 팀원의 수정 및 연동 작업도 포함될 수 있습니다.

### 1. 보스 1페이즈와 공격 패턴

- 상태 머신을 사용해 등장, 대기, 이동, 공격, 그로기, 대화, 사망 상태를 구성했습니다.
- 탄막 공격을 원형, 회전 십자 확산, 확산 패턴으로 나누어 구현했습니다.
- 사슬 공격을 플레이어 방향 직선, 십자, 무작위, 원형 패턴으로 구성했습니다.
- 공격 경고 표시와 실제 사슬·탄환 오브젝트를 연결하고, 상태별 시간 및 애니메이션에 따라 공격 흐름을 제어했습니다.

**관련 코드:** [보스 객체](Client/Code/CBossPhase1.cpp) · [상태 머신](Client/Code/CBossStateMachine.cpp) · [탄막 패턴](Client/Code/CBossBulletAttack.cpp) · [사슬 패턴](Client/Code/CBossChainAttack.cpp) · [공격 경고](Client/Code/CWarningChain.cpp)

### 2. 인벤토리와 아이템 드롭

- 슬롯 기반 인벤토리에서 아이템 추가, 제거, 수량 조회 및 슬롯별 드롭 기능을 구현했습니다.
- 슬롯 목록은 `vector`, 아이템 ID별 수량은 `unordered_map`으로 관리합니다.
- 드롭 시스템에서 월드에 아이템을 생성하고, 드롭된 아이템 목록을 관리하도록 구성했습니다.
- 아이템 획득 알림을 콜백으로 연결하고, 드롭 연출 및 아이템 관련 오류를 수정했습니다.

**관련 코드:** [인벤토리](Client/Code/CInventory.cpp) · [아이템 데이터](Client/Code/CItemDB.cpp) · [드롭 시스템](Client/Code/CDropSystem.cpp) · [드롭 아이템](Client/Code/CDroppedItem.cpp) · [상자](Client/Code/CChest.cpp)

### 3. 채집과 월드 상호작용

- 나무, 바위, 채집물 등 상호작용 오브젝트를 구현했습니다.
- 공통 상호작용 클래스에서 플레이어 접근, 호버링 및 피격 처리를 다룹니다.
- 채집 대상의 반응과 아이템 드롭을 연결하고, 피킹 및 외곽선 표시 관련 작업을 진행했습니다.

**관련 코드:** [공통 상호작용](Client/Code/CWorldInteractive.cpp) · [나무](Client/Code/CTree.cpp) · [바위](Client/Code/CRock.cpp) · [채집물](Client/Code/CHarvestableHerb.cpp)

### 4. 요리 로직과 미니게임

- 인벤토리 재료를 확인하고 레시피를 요리 대기열에 등록하는 로직을 구현했습니다.
- 선택한 레시피와 실제 조리 작업을 관리하고, 취소 및 요리 결과물 드롭을 연결했습니다.
- 요리 미니게임 UI 및 입력 처리를 구현하고 인벤토리·드롭 시스템과 연동했습니다.

**관련 코드:** [요리 컨트롤러](Client/Code/CCookingController.cpp) · [요리 미니게임](Client/Code/CUICookingGame.cpp)

### 5. 이펙트와 메인 메뉴

- 스프라이트 이펙트 및 이동 궤적을 표현하는 트레일 이펙트를 구현했습니다.
- 트레일 노드의 위치, 수명, 폭 등을 관리하며 카메라 방향을 고려한 렌더링을 구성했습니다.
- 트레일 렌더 상태와 폰트 렌더링 간 충돌 문제를 수정했습니다.
- 메인 메뉴 화면 및 관련 메뉴 요소를 구현했습니다.

**관련 코드:** [스프라이트 이펙트](Client/Code/CSpriteEff.cpp) · [트레일 이펙트](Client/Code/CTrailEff.cpp) · [메인 메뉴](Client/Code/CMainMenu.cpp)

## 프로젝트 구조

```text
SR_Cult.sln        Visual Studio 솔루션
Base/              공통 기반 클래스
Engine/            게임 오브젝트, 컴포넌트, 씬, 렌더링 등 엔진 코드
Client/            게임플레이, 캐릭터, 보스, 아이템, UI 코드
EditorTool/        ImGui 기반 편집 도구 코드
Reference/Header/  프로젝트 간 공유·참조 헤더
Libs/              외부 라이브러리 헤더 및 소스
Copy.bat           엔진 빌드 결과물 및 헤더 복사 스크립트
```

씬은 레이어를 관리하고, 각 레이어는 오브젝트 타입별 `vector<CGameObject*>`를 보관합니다. 렌더러는 렌더 그룹별 `list<CGameObject*>`를 사용합니다. 게임플레이 코드는 이 구조 위에서 오브젝트와 컴포넌트를 조합합니다.

## 빌드 및 실행 안내

1. Windows에서 Visual Studio 2022의 C++ 개발 환경과 Windows SDK를 준비합니다.
2. 프로젝트가 참조하는 DirectX 9 / D3DX9 SDK 및 FMOD 라이브러리를 별도로 준비합니다.
3. `SR_Cult.sln`을 열고 SDK와 라이브러리 경로, 대상 플랫폼을 환경에 맞게 설정합니다.
4. `Engine`, `EditorTool`의 빌드 결과물이 `Client`와 `Reference/Library`에서 참조될 수 있도록 확인한 뒤 Client를 빌드합니다.
5. 실제 게임 실행에는 원본 프로젝트에서 사용하는 리소스와 런타임 DLL이 필요합니다.

이 공개본에서는 빌드·실행 검증을 수행하지 않았습니다. 리소스와 사전 빌드 라이브러리가 제외되어 있으므로 별도 환경 구성이 필요합니다.

## 공개 범위

- **포함:** 팀 프로젝트 소스, 헤더, Visual Studio 프로젝트·솔루션, 외부 라이브러리의 일부 헤더·소스, 작은 프로젝트 아이콘 및 Windows 리소스 정의.
- **제외:** `Client/Bin/Resource`의 게임 리소스, 기존 `.git` 이력, `.vs` 캐시, 중간 빌드 파일, 실행 파일, DLL 및 LIB.

원작을 바탕으로 한 학습·포트폴리오용 팀 프로젝트입니다. 원작의 명칭 및 리소스에 대한 권리는 해당 권리자에게 있습니다. 포함된 외부 라이브러리의 소스와 헤더에는 각각의 라이선스가 적용됩니다.
