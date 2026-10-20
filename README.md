# DumbGuys / Server

[English](README.md), [Korean](README.ko.md), [Japanese](README.ja.md)

A multiplayer team game with connected account, lobby, match and appearance flows.

| Project | Details |
|---|---|
| Status | Development archive |
| Stack | C++, C#, Unity |

[Explore the case](https://ysl-ryudo.github.io/en/work/dumbguys/), [Portfolio](https://ysl-ryudo.github.io/en/)

[![DumbGuys](https://i.ytimg.com/vi/Z4tffAvRjJs/hqdefault.jpg)](https://www.youtube.com/watch?v=Z4tffAvRjJs)

[DumbGuys full play](https://www.youtube.com/watch?v=Z4tffAvRjJs)

## My contribution

Implemented account and connection handling for the game client.

Worked on room, match and player state, plus store and appearance-related data.

Integrated the server with the client-side lobby and match flow.

## Contribution evidence

[Source baseline / 코드 기준 / コード基準](https://github.com/YSL-RyuDo/DumbGuysServer/tree/2ba094126a0bb09ace4167dc3c2e956c4a39dec2)

[Development history / 개발 이력 / 開発履歴](https://github.com/YSL-RyuDo/DumbGuysServer/commits/main/?author=YSL-RyuDo)

Commits are evidence of the stated changes, not a percentage-of-authorship score. Third-party assets and teammates' work remain separately credited.

## Implementation and design

Accounts, rooms and player appearance need to carry through the lobby and match flow. A client and server must also be reviewed at compatible revisions.

I worked on account/connection handling, room and player state, store/appearance data and client-side lobby/match integration.

| Area | Implementation |
|---|---|
| Identity | Account and connection handling |
| Lobby to match | Room and player state linked to the Unity client |
| Player data | Store and appearance state |

## Open and inspect

Open [DumbGuysServer.sln](DumbGuysServer.sln) in Visual Studio with the Windows C++ workload. Inspect the selected configuration, data paths, dependencies and endpoint settings before starting the matching client. Preserve existing account data.

[DumbGuys](https://github.com/YSL-RyuDo/DumbGuys)

## Development and verification scope

Historical development server. This README makes no current performance or fault-tolerance claim. The client's later restore-1121 branch should be considered when selecting a compatible demo revision.

## Related repositories

[DumbGuys](https://github.com/YSL-RyuDo/DumbGuys), Private, access required

[DumbGuysServer](https://github.com/YSL-RyuDo/DumbGuysServer), Private, access required
