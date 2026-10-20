# DumbGuys — Game Server

[English](README.md) · [Korean](README.ko.md) · [Japanese](README.ja.md)

DumbGuys Unity 클라이언트와 연결되는 Windows C++ 서버입니다.

## 내 기여

- 게임 클라이언트의 계정·연결 처리를 구현했습니다.
- 방·매치·플레이어 상태와 상점·외형 관련 데이터를 작업했습니다.
- 클라이언트의 로비·경기 흐름과 서버를 연동했습니다.

## 기여 근거

- [Source baseline / 코드 기준 / コード基準](https://github.com/YSL-RyuDo/DumbGuysServer/tree/2ba094126a0bb09ace4167dc3c2e956c4a39dec2)
- [Development history / 개발 이력 / 開発履歴](https://github.com/YSL-RyuDo/DumbGuysServer/commits/main/?author=YSL-RyuDo)

커밋은 설명한 변경의 근거이며 기여율 점수가 아닙니다. 외부 에셋과 팀원 작업의 크레딧은 구분합니다.

## 실행·코드 확인

Windows C++ 워크로드가 설치된 Visual Studio에서 [DumbGuysServer.sln](DumbGuysServer.sln)을 여세요. 구성·데이터 경로·의존성·접속 설정을 확인한 뒤 대응하는 클라이언트를 실행하세요. 기존 계정 데이터를 보존하세요.

[DumbGuys](https://github.com/YSL-RyuDo/DumbGuys)

## 개발·검증 범위

과거 개발 서버입니다. 현재 성능이나 장애 내성에 대한 검증 완료를 주장하지 않습니다. 시연 버전을 정할 때 클라이언트의 후속 restore-1121 브랜치를 함께 확인해야 합니다.
