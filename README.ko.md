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

## 실행, 코드 확인

Windows C++ 워크로드가 설치된 Visual Studio에서 [DumbGuysServer.sln](DumbGuysServer.sln)을 여세요. 구성, 데이터 경로, 의존성, 접속 설정을 확인한 뒤 대응하는 클라이언트를 실행하세요. 기존 계정 데이터를 보존하세요.

[DumbGuys](https://github.com/YSL-RyuDo/DumbGuys)

## 개발, 검증 범위

과거 개발 서버입니다. 현재 성능이나 장애 내성에 대한 검증 완료를 주장하지 않습니다. 시연 버전을 정할 때 클라이언트의 후속 restore-1121 브랜치를 함께 확인해야 합니다.

## 관련 저장소

[DumbGuys](https://github.com/YSL-RyuDo/DumbGuys), 비공개, 접근 권한 필요

[DumbGuysServer](https://github.com/YSL-RyuDo/DumbGuysServer), 비공개, 접근 권한 필요
