# DumbGuys / Server

[English](README.md), [Korean](README.ko.md), [Japanese](README.ja.md)

계정과 로비, 경기, 외형 데이터가 연결되는 멀티플레이 팀 게임입니다.

| 프로젝트 정보 | 내용 |
|---|---|
| 개발 상태 | 개발 기록 |
| 사용 기술 | C++, C#, Unity |

[개발 사례 보기](https://ysl-ryudo.github.io/ko/work/dumbguys/), [Portfolio](https://ysl-ryudo.github.io/ko/)

[![DumbGuys](https://i.ytimg.com/vi/Z4tffAvRjJs/hqdefault.jpg)](https://www.youtube.com/watch?v=Z4tffAvRjJs)

[DumbGuys full play](https://www.youtube.com/watch?v=Z4tffAvRjJs)

## 내 기여

게임 클라이언트의 계정, 연결 처리를 구현했습니다.

방, 매치, 플레이어 상태와 상점, 외형 관련 데이터를 작업했습니다.

클라이언트의 로비, 경기 흐름과 서버를 연동했습니다.

## 기여 근거

[Source baseline / 코드 기준 / コード基準](https://github.com/YSL-RyuDo/DumbGuysServer/tree/2ba094126a0bb09ace4167dc3c2e956c4a39dec2)

[Development history / 개발 이력 / 開発履歴](https://github.com/YSL-RyuDo/DumbGuysServer/commits/main/?author=YSL-RyuDo)

커밋은 설명한 변경의 근거이며 기여율 점수가 아닙니다. 외부 에셋과 팀원 작업의 크레딧은 구분합니다.

## 구현과 설계

계정, 방, 플레이어 외형 정보를 로비와 경기 흐름으로 연결하는 작업입니다. 클라이언트와 서버를 서로 맞는 버전에서 살펴보는 것도 중요합니다.

계정, 연결 처리, 방, 플레이어 상태, 상점, 외형 데이터와 클라이언트 로비, 경기 연동을 작업했습니다.

| 영역 | 구현 |
|---|---|
| 계정 | 계정, 연결 처리 |
| 로비에서 경기로 | 방, 플레이어 상태와 Unity 클라이언트 연결 |
| 플레이어 데이터 | 상점, 외형 상태 |

## 코드의 동작과 구현 방식을 택한 이유

확인한 구현이 어떤 제약을 다루며 어떤 장단점이 생기는지 설명합니다. 설계 효과에 대한 설명은 코드 분석이며, 비교한 모든 대안을 당시 직접 구현했다는 뜻은 아닙니다. 기여 링크는 공동 프로젝트 안의 담당 변경을 가리키고 소스 링크는 기존 접근 권한을 유지합니다.

### 대기 시간에 따라 매칭 조건을 넓히는 이유

**구현 흐름**

MatchManager는 대기 중인 플레이어의 랭크, 점수, 레벨을 비교하고 대기 시간이 길어지면 일부 허용 범위를 넓힙니다. 매칭이 성립하면 방을 만들고 참가자와 시작 위치 정보를 보냅니다. 주변 로비 흐름에서는 보유 아이템과 외형 데이터도 클라이언트와 서버가 주고받습니다.

**이 구조가 필요한 이유**

조건이 비슷한 참가자가 적으면 실력 유사성과 짧은 대기 시간을 동시에 만족하기 어렵습니다. 시간에 따라 기준을 바꾸면 아무 상대나 즉시 선택하거나 같은 조건을 끝없이 기다리는 대신 절충을 규칙으로 표현할 수 있습니다. 보유 목록과 현재 외형을 구분하는 것도 가지고 있는 것과 지금 보여주는 것을 분리하기 위해서입니다.

**대안과 현재 구현의 절충**

도착 순 매칭은 빠르지만 실력 유사성을 반영하지 않습니다. 고정된 유사성 기준은 예측하기 쉽지만 조건이 드문 참가자가 오래 기다릴 수 있습니다. 시간에 따른 확장은 두 목표를 규칙으로 절충하지만 적절한 범위는 참가자 분포와 경기 품질 근거로 정해야 합니다.

**장단점과 검증 범위**

허용 범위는 구현 정책이며 공정한 매칭이나 실제 대기 시간 단축이 측정되었다는 뜻은 아닙니다. 참가자 분포와 취소, 연결 종료 처리는 별도로 평가해야 합니다. 아래 클라이언트 근거는 restore-1121 브랜치로, 기본 브랜치와 같은 연동 상태라고 가정하지 않습니다.

[DumbGuysServer/MatchManager.cpp](https://github.com/YSL-RyuDo/DumbGuysServer/blob/6c9f9da948008ace439ebfc7a1aeb4b130da12f0/DumbGuysServer/MatchManager.cpp), [DumbGuysServer/OwnedItemManager.cpp](https://github.com/YSL-RyuDo/DumbGuysServer/blob/6c9f9da948008ace439ebfc7a1aeb4b130da12f0/DumbGuysServer/OwnedItemManager.cpp), [DumbGuys/ClientSession.cs](https://github.com/YSL-RyuDo/DumbGuys/blob/6c30b670a68da9a89d8ad0445de376cd620a21a7/Assets/Scripts/Network/ClientSession.cs)

## 실행, 코드 확인

Windows C++ 워크로드가 설치된 Visual Studio에서 [DumbGuysServer.sln](DumbGuysServer.sln)을 여세요. 구성, 데이터 경로, 의존성, 접속 설정을 확인한 뒤 대응하는 클라이언트를 실행하세요. 기존 계정 데이터를 보존하세요.

[DumbGuys](https://github.com/YSL-RyuDo/DumbGuys)

## 개발, 검증 범위

과거 개발 서버입니다. 현재 성능이나 장애 내성에 대한 검증 완료를 주장하지 않습니다. 시연 버전을 정할 때 클라이언트의 후속 restore-1121 브랜치를 함께 확인해야 합니다.

## 관련 저장소

[DumbGuys](https://github.com/YSL-RyuDo/DumbGuys), 비공개, 접근 권한 필요

[DumbGuysServer](https://github.com/YSL-RyuDo/DumbGuysServer), 비공개, 접근 권한 필요
