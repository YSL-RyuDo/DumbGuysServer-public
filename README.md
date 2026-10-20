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

## How the code works and why it is structured this way

These notes connect the inspected implementation to the constraints it addresses and its tradeoffs. Explanations of design consequences are code analysis, not a claim that every alternative was prototyped. Contribution links identify my changes within team projects; source links retain their original access permissions.

### Match criteria change with time spent waiting

**Implementation**

MatchManager compares queued players by rank, score and level, then widens selected thresholds as waiting time grows. On a match it creates a room and sends the participant/spawn information. Client and server also exchange owned items and appearance data for the surrounding lobby flow.

**Design rationale**

Strict similarity and short waiting time can conflict when few compatible players are queued. Time-dependent thresholds make that tradeoff explicit instead of accepting any opponent immediately or waiting indefinitely for an exact match. Keeping ownership and appearance distinct also separates what a player has from what they currently display.

**Alternatives and the current tradeoff**

First-come pairing favors quick matches while ignoring skill similarity. Fixed similarity limits are predictable but can leave outliers waiting. Widening thresholds over time balances those aims explicitly, but choosing the thresholds still requires population and match-quality evidence.

**Tradeoffs and verification**

The thresholds are implementation policy, not proof of fair matchmaking or shorter observed queues. Player population and cancellation/disconnect behavior need separate evaluation. The client reference below is the restore-1121 branch, so its integration state should not be assumed to equal the default branch.

[DumbGuysServer/MatchManager.cpp](https://github.com/YSL-RyuDo/DumbGuysServer/blob/6c9f9da948008ace439ebfc7a1aeb4b130da12f0/DumbGuysServer/MatchManager.cpp), [DumbGuysServer/OwnedItemManager.cpp](https://github.com/YSL-RyuDo/DumbGuysServer/blob/6c9f9da948008ace439ebfc7a1aeb4b130da12f0/DumbGuysServer/OwnedItemManager.cpp), [DumbGuys/ClientSession.cs](https://github.com/YSL-RyuDo/DumbGuys/blob/6c30b670a68da9a89d8ad0445de376cd620a21a7/Assets/Scripts/Network/ClientSession.cs)

## Open and inspect

Open [DumbGuysServer.sln](DumbGuysServer.sln) in Visual Studio with the Windows C++ workload. Inspect the selected configuration, data paths, dependencies and endpoint settings before starting the matching client. Preserve existing account data.

[DumbGuys](https://github.com/YSL-RyuDo/DumbGuys)

## Development and verification scope

Historical development server. This README makes no current performance or fault-tolerance claim. The client's later restore-1121 branch should be considered when selecting a compatible demo revision.

## Related repositories

[DumbGuys](https://github.com/YSL-RyuDo/DumbGuys), Private, access required

[DumbGuysServer](https://github.com/YSL-RyuDo/DumbGuysServer), Private, access required
