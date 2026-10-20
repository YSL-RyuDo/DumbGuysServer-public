# DumbGuys / Server

<!-- portfolio-languages -->
[![English](https://ysl-ryudo.github.io/assets/diagrams/language-en.svg)](README.md) [![한국어 (현재 언어)](https://ysl-ryudo.github.io/assets/diagrams/language-ko-current.svg)](README.ko.md) [![日本語](https://ysl-ryudo.github.io/assets/diagrams/language-ja.svg)](README.ja.md)
<!-- /portfolio-languages -->

계정과 로비, 경기, 외형 데이터가 연결되는 멀티플레이 팀 게임입니다.

| 프로젝트 정보 | 내용 |
|---|---|
| 개발 상태 | 개발 기록 |
| 사용 기술 | C++, C#, Unity |

[개발 사례 보기](https://ysl-ryudo.github.io/ko/work/dumbguys/), [Portfolio](https://ysl-ryudo.github.io/ko/)

[![DumbGuys](https://i.ytimg.com/vi/Z4tffAvRjJs/hqdefault.jpg)](https://www.youtube.com/watch?v=Z4tffAvRjJs)

[DumbGuys full play](https://www.youtube.com/watch?v=Z4tffAvRjJs)

## 게임에서 보는 구현

[![계정 생성과 로비 이동을 거쳐 튜토리얼에서 실제 이동과 점프 경로를 실행한 화면.](https://ysl-ryudo.github.io/assets/gameplay/dumbguys-movement.webp)](https://ysl-ryudo.github.io/assets/gameplay/dumbguys-movement.webp)

계정 생성과 로비 이동을 거쳐 튜토리얼에서 실제 이동과 점프 경로를 실행한 화면.

## 게임 전체에서 맡은 구현

Unity에서 이동, 장애물, 튜토리얼, 외형과 상점 흐름을 개발하고 C++ 서버에서 계정, 매칭과 경기 통신을 구현했습니다.

[![담당 구현 흐름](https://ysl-ryudo.github.io/assets/diagrams/dumbguys-flow-ko.svg)](https://ysl-ryudo.github.io/assets/diagrams/dumbguys-flow-ko.svg)

### 이동 전략과 캐릭터 조작

입력과 이동 전략, 캐릭터 상태와 Rigidbody 모터를 나눴습니다. 일반, 미끄러짐과 대시 전략이 같은 모터를 이용해 이동을 바꾸고 애니메이션은 캐릭터 상태를 따릅니다. 이동 변형이 공통 물리 구현을 사용하도록 구성했습니다.

[CharacterMotor.cs (restore-1121)](https://github.com/YSL-RyuDo/DumbGuys/blob/6c30b670a68da9a89d8ad0445de376cd620a21a7/Assets/Scripts/Player/CharacterMotor.cs), [내 변경 27bfa58](https://github.com/YSL-RyuDo/DumbGuys/commit/27bfa58811079f0886733c49174ce9627cbae910), [IMovementStrategy.cs (restore-1121)](https://github.com/YSL-RyuDo/DumbGuys/blob/6c30b670a68da9a89d8ad0445de376cd620a21a7/Assets/Scripts/Player/IMovementStrategy.cs), [내 변경 e9d0b38](https://github.com/YSL-RyuDo/DumbGuys/commit/e9d0b380cfe1eb297fbf68c38a00f77ad37b53f2)

### 장애물과 스플라인 이동

밀기와 띄우기, 이동 발판 연결과 곡선 파이프 이동을 구현했습니다. 파이프에서는 Rigidbody 처리를 잠시 바꾸고 스플라인의 위치와 접선 방향을 따라 이동한 뒤 조작을 복구합니다. 경로 제작용 에디터 생성기도 만들었습니다.

[PipeTransportCurved.cs (restore-1121)](https://github.com/YSL-RyuDo/DumbGuys/blob/6c30b670a68da9a89d8ad0445de376cd620a21a7/Assets/Scripts/PipeTransportCurved.cs), [내 변경 359f276](https://github.com/YSL-RyuDo/DumbGuys/commit/359f27621811e1cdc667328cfb55d799340cb3e6), [PipeSplineGenerator.cs (restore-1121)](https://github.com/YSL-RyuDo/DumbGuys/blob/6c30b670a68da9a89d8ad0445de376cd620a21a7/Assets/Editor/PipeSplineGenerator.cs), [내 변경 27bfa58](https://github.com/YSL-RyuDo/DumbGuys/commit/27bfa58811079f0886733c49174ce9627cbae910)

### 경기 진입과 상대 표시

로딩 준비와 생성 위치, 외형 교환, 상대 위치와 회전 표시를 구현했습니다. GameSceneController에서 피격과 결과 메시지도 다룹니다. 네트워크 수신 스레드에서 씬 객체를 직접 바꾸지 않고 Unity 메인 스레드로 작업을 전달합니다.

[LoadingSceneController.cs (restore-1121)](https://github.com/YSL-RyuDo/DumbGuys/blob/6c30b670a68da9a89d8ad0445de376cd620a21a7/Assets/Scripts/LoadingScene/LoadingSceneController.cs), [내 변경 87533da](https://github.com/YSL-RyuDo/DumbGuys/commit/87533da63f475a3a306ffa38833e6c72ed8b378e), [GameSceneController.cs (restore-1121)](https://github.com/YSL-RyuDo/DumbGuys/blob/6c30b670a68da9a89d8ad0445de376cd620a21a7/Assets/Scripts/GameScene/GameSceneController.cs), [내 변경 6d033d0](https://github.com/YSL-RyuDo/DumbGuys/commit/6d033d0bfd08eaf765251653d99a5b85253ca821), [UnityMainThreadDispatcher.cs (restore-1121)](https://github.com/YSL-RyuDo/DumbGuys/blob/6c30b670a68da9a89d8ad0445de376cd620a21a7/Assets/Scripts/Network/UnityMainThreadDispatcher.cs), [내 변경 87533da](https://github.com/YSL-RyuDo/DumbGuys/commit/87533da63f475a3a306ffa38833e6c72ed8b378e)

### 튜토리얼과 코스 종료

안내와 낙하, 도착 트리거를 갖춘 튜토리얼 매니저와 게임의 도착 및 낙하 처리를 구현했습니다. 캐릭터 이동뿐 아니라 조작을 익히는 과정과 코스의 종료 처리도 담당했습니다.

[Tutorial0Manager.cs (restore-1121)](https://github.com/YSL-RyuDo/DumbGuys/blob/6c30b670a68da9a89d8ad0445de376cd620a21a7/Assets/Scripts/Tutorial0Scene/Tutorial0Manager.cs), [내 변경 6d033d0](https://github.com/YSL-RyuDo/DumbGuys/commit/6d033d0bfd08eaf765251653d99a5b85253ca821), [Tutorial1Manager.cs (restore-1121)](https://github.com/YSL-RyuDo/DumbGuys/blob/6c30b670a68da9a89d8ad0445de376cd620a21a7/Assets/Scripts/Tutorial1Scene/Tutorial1Manager.cs), [내 변경 8b0ac0c](https://github.com/YSL-RyuDo/DumbGuys/commit/8b0ac0c91640a2154f89b77656c9f0daa07ca458), [WinZone.cs (restore-1121)](https://github.com/YSL-RyuDo/DumbGuys/blob/6c30b670a68da9a89d8ad0445de376cd620a21a7/Assets/Scripts/GameScene/WinZone.cs), [내 변경 87533da](https://github.com/YSL-RyuDo/DumbGuys/commit/87533da63f475a3a306ffa38833e6c72ed8b378e)

### 외형, 상점과 서버 상태

분류별 상점과 프로필 외형 화면을 구현하고 보유 아이템과 장착 데이터에 연결했습니다. 서버의 경기와 계정 처리는 로비 선택부터 플레이와 결과로 이어집니다. 매칭 정책은 아래에서 별도로 설명합니다.

[StoreManager.cs (restore-1121)](https://github.com/YSL-RyuDo/DumbGuys/blob/6c30b670a68da9a89d8ad0445de376cd620a21a7/Assets/Scripts/StoreScene/StoreManager.cs), [내 변경 edffcc8](https://github.com/YSL-RyuDo/DumbGuys/commit/edffcc86e34c4d817a000462dbbe19502d4843fe), [MyPageManager.cs (restore-1121)](https://github.com/YSL-RyuDo/DumbGuys/blob/6c30b670a68da9a89d8ad0445de376cd620a21a7/Assets/Scripts/MyPageScene/MyPageManager.cs), [내 변경 94edd35](https://github.com/YSL-RyuDo/DumbGuys/commit/94edd358e09412d75e7836ee8f7ff17519f206fe), [MatchManager.cpp (main)](https://github.com/YSL-RyuDo/DumbGuysServer/blob/75ad6626d21d7638d37240c85ff4a9aa481e5620/DumbGuysServer/MatchManager.cpp), [내 변경 bb103b5](https://github.com/YSL-RyuDo/DumbGuysServer/commit/bb103b5d1c143534126cf8dfb577812eef035d90)

## 기여 근거

[Source baseline / 코드 기준 / コード基準](https://github.com/YSL-RyuDo/DumbGuysServer/tree/2ba094126a0bb09ace4167dc3c2e956c4a39dec2)

[Development history / 개발 이력 / 開発履歴](https://github.com/YSL-RyuDo/DumbGuysServer/commits/main/?author=YSL-RyuDo)

## 구현과 설계

계정, 방, 플레이어 외형 정보를 로비와 경기 흐름으로 연결하는 작업입니다. 클라이언트와 서버를 서로 맞는 버전에서 살펴보는 것도 중요합니다.

계정, 연결 처리, 방, 플레이어 상태, 상점, 외형 데이터와 클라이언트 로비, 경기 연동을 작업했습니다.

## 코드의 동작과 구현 방식을 택한 이유

### 대기 시간에 따라 매칭 조건을 넓히는 이유

**구현 흐름**

MatchManager는 대기 중인 플레이어의 랭크, 점수, 레벨을 비교하고 대기 시간이 길어지면 일부 허용 범위를 넓힙니다. 매칭이 성립하면 방을 만들고 참가자와 시작 위치 정보를 보냅니다. 주변 로비 흐름에서는 보유 아이템과 외형 데이터도 클라이언트와 서버가 주고받습니다.

**이 구조가 필요한 이유**

조건이 비슷한 참가자가 적으면 실력 유사성과 짧은 대기 시간을 동시에 만족하기 어렵습니다. 시간에 따라 기준을 바꾸면 아무 상대나 즉시 선택하거나 같은 조건을 끝없이 기다리는 대신 절충을 규칙으로 표현할 수 있습니다. 보유 목록과 현재 외형을 구분하는 것도 가지고 있는 것과 지금 보여주는 것을 분리하기 위해서입니다.

**대안과 현재 구현의 절충**

도착 순 매칭은 빠르지만 실력 유사성을 반영하지 않습니다. 고정된 유사성 기준은 예측하기 쉽지만 조건이 드문 참가자가 오래 기다릴 수 있습니다. 시간에 따른 확장은 두 목표를 규칙으로 절충하지만 적절한 범위는 참가자 분포와 경기 품질 근거로 정해야 합니다.

**장단점과 검증 범위**

허용 범위는 참가자 수와 실력 분포에 따라 조정해야 합니다. 취소와 연결 종료 시 대기 항목을 제거하고, 경기 품질과 대기 시간을 측정해 기준을 조정해야 합니다.

[DumbGuysServer/MatchManager.cpp](https://github.com/YSL-RyuDo/DumbGuysServer/blob/6c9f9da948008ace439ebfc7a1aeb4b130da12f0/DumbGuysServer/MatchManager.cpp), [DumbGuysServer/OwnedItemManager.cpp](https://github.com/YSL-RyuDo/DumbGuysServer/blob/6c9f9da948008ace439ebfc7a1aeb4b130da12f0/DumbGuysServer/OwnedItemManager.cpp), [DumbGuys/ClientSession.cs](https://github.com/YSL-RyuDo/DumbGuys/blob/6c30b670a68da9a89d8ad0445de376cd620a21a7/Assets/Scripts/Network/ClientSession.cs)

### 이동 방식은 바꾸고 물리 처리는 공유한 이유

**구현 흐름**

IInputProvider가 방향과 점프 입력을 제공하고 이동 전략은 다른 배율로 CharacterMotor를 호출합니다. 모터는 수직 속도를 유지하면서 수평 속도를 보간하고 회전과 지면 점프를 처리합니다.

**이 구조가 필요한 이유**

미끄러짐이나 대시 효과를 바꿀 때 Rigidbody 처리 전체를 복사하지 않아도 됩니다. 평면 이동을 갱신할 때 Y 속도를 유지해야 중력이나 진행 중인 점프를 덮어쓰지 않습니다.

**대안과 현재 구현의 절충**

효과가 적으면 하나의 이동 스크립트와 플래그가 간단합니다. 전략을 나누면 변형이 명시적인 대신 호출 경로가 늘어납니다. 현재 전략은 작은 편이며, 물리 변경을 각 전략에 흩뜨리지 않고 공통 모터를 유지해야 분리의 의미가 있습니다.

**장단점과 검증 범위**

모터는 Unity 물리와 지면 검사에 의존합니다. 경사와 이동 발판, 점프 중 효과 교체와 네트워크 보정을 함께 다뤄야 합니다.

[DumbGuys/IInputProvider.cs (restore-1121)](https://github.com/YSL-RyuDo/DumbGuys/blob/6c30b670a68da9a89d8ad0445de376cd620a21a7/Assets/Scripts/Player/IInputProvider.cs), [DumbGuys/IMovementStrategy.cs (restore-1121)](https://github.com/YSL-RyuDo/DumbGuys/blob/6c30b670a68da9a89d8ad0445de376cd620a21a7/Assets/Scripts/Player/IMovementStrategy.cs), [DumbGuys/CharacterMotor.cs (restore-1121)](https://github.com/YSL-RyuDo/DumbGuys/blob/6c30b670a68da9a89d8ad0445de376cd620a21a7/Assets/Scripts/Player/CharacterMotor.cs)

## 실행, 코드 확인

Windows C++ 워크로드가 설치된 Visual Studio에서 [DumbGuysServer.sln](DumbGuysServer.sln)을 여세요. 구성, 데이터 경로, 의존성, 접속 설정을 확인한 뒤 대응하는 클라이언트를 실행하세요. 기존 계정 데이터를 보존하세요.

[DumbGuys](https://github.com/YSL-RyuDo/DumbGuys)

## 관련 저장소

[DumbGuys](https://github.com/YSL-RyuDo/DumbGuys), 비공개, 접근 권한 필요

[DumbGuysServer](https://github.com/YSL-RyuDo/DumbGuysServer), 비공개, 접근 권한 필요
