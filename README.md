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

## What I built across the game

I developed the Unity client and C++ server, including movement, obstacles, tutorials, appearance/store flows and match networking. The restore-1121 branch contains client work not present in the smaller main snapshot. Unity packages, sample code and imported art remain third-party work.

**Player flow**

Login → lobby and appearance → matching and loading → obstacle course → finish or fall → result

Source links pin the inspected revision; contribution links identify my non-merge changes. Shared-file links do not imply sole authorship. Private source requires repository access.

### Movement strategies and character control

Separated input, movement strategy, character state and the Rigidbody motor. Normal, slippery and dash strategies vary movement through the same motor; animation follows the character state. This gives movement variations a shared physics implementation.

[CharacterMotor.cs (restore-1121)](https://github.com/YSL-RyuDo/DumbGuys/blob/6c30b670a68da9a89d8ad0445de376cd620a21a7/Assets/Scripts/Player/CharacterMotor.cs), [My change 27bfa58](https://github.com/YSL-RyuDo/DumbGuys/commit/27bfa58811079f0886733c49174ce9627cbae910), [IMovementStrategy.cs (restore-1121)](https://github.com/YSL-RyuDo/DumbGuys/blob/6c30b670a68da9a89d8ad0445de376cd620a21a7/Assets/Scripts/Player/IMovementStrategy.cs), [My change e9d0b38](https://github.com/YSL-RyuDo/DumbGuys/commit/e9d0b380cfe1eb297fbf68c38a00f77ad37b53f2)

### Obstacles and spline transport

Implemented push/knock-up interactions, moving-platform attachment and curved pipe transport. Pipe travel temporarily changes Rigidbody handling, follows spline position and tangent, then restores control. An editor generator supports authoring the pipe path.

[PipeTransportCurved.cs (restore-1121)](https://github.com/YSL-RyuDo/DumbGuys/blob/6c30b670a68da9a89d8ad0445de376cd620a21a7/Assets/Scripts/PipeTransportCurved.cs), [My change 359f276](https://github.com/YSL-RyuDo/DumbGuys/commit/359f27621811e1cdc667328cfb55d799340cb3e6), [PipeSplineGenerator.cs (restore-1121)](https://github.com/YSL-RyuDo/DumbGuys/blob/6c30b670a68da9a89d8ad0445de376cd620a21a7/Assets/Editor/PipeSplineGenerator.cs), [My change 27bfa58](https://github.com/YSL-RyuDo/DumbGuys/commit/27bfa58811079f0886733c49174ce9627cbae910)

### Match entry and remote presentation

Implemented loading readiness, spawn and appearance exchange, and remote position/rotation presentation. GameSceneController also handles hit and result messages. The network layer dispatches work to Unity's main thread rather than manipulating scene objects from the receive thread.

[LoadingSceneController.cs (restore-1121)](https://github.com/YSL-RyuDo/DumbGuys/blob/6c30b670a68da9a89d8ad0445de376cd620a21a7/Assets/Scripts/LoadingScene/LoadingSceneController.cs), [My change 87533da](https://github.com/YSL-RyuDo/DumbGuys/commit/87533da63f475a3a306ffa38833e6c72ed8b378e), [GameSceneController.cs (restore-1121)](https://github.com/YSL-RyuDo/DumbGuys/blob/6c30b670a68da9a89d8ad0445de376cd620a21a7/Assets/Scripts/GameScene/GameSceneController.cs), [My change 6d033d0](https://github.com/YSL-RyuDo/DumbGuys/commit/6d033d0bfd08eaf765251653d99a5b85253ca821), [UnityMainThreadDispatcher.cs (restore-1121)](https://github.com/YSL-RyuDo/DumbGuys/blob/6c30b670a68da9a89d8ad0445de376cd620a21a7/Assets/Scripts/Network/UnityMainThreadDispatcher.cs), [My change 87533da](https://github.com/YSL-RyuDo/DumbGuys/commit/87533da63f475a3a306ffa38833e6c72ed8b378e)

### Tutorials and course completion

Built tutorial managers with hints, fall and win triggers, and gameplay finish/fall handling. This covers teaching controls and resolving the course outcome as well as moving the character.

[Tutorial0Manager.cs (restore-1121)](https://github.com/YSL-RyuDo/DumbGuys/blob/6c30b670a68da9a89d8ad0445de376cd620a21a7/Assets/Scripts/Tutorial0Scene/Tutorial0Manager.cs), [My change 6d033d0](https://github.com/YSL-RyuDo/DumbGuys/commit/6d033d0bfd08eaf765251653d99a5b85253ca821), [Tutorial1Manager.cs (restore-1121)](https://github.com/YSL-RyuDo/DumbGuys/blob/6c30b670a68da9a89d8ad0445de376cd620a21a7/Assets/Scripts/Tutorial1Scene/Tutorial1Manager.cs), [My change 8b0ac0c](https://github.com/YSL-RyuDo/DumbGuys/commit/8b0ac0c91640a2154f89b77656c9f0daa07ca458), [WinZone.cs (restore-1121)](https://github.com/YSL-RyuDo/DumbGuys/blob/6c30b670a68da9a89d8ad0445de376cd620a21a7/Assets/Scripts/GameScene/WinZone.cs), [My change 87533da](https://github.com/YSL-RyuDo/DumbGuys/commit/87533da63f475a3a306ffa38833e6c72ed8b378e)

### Appearance, store and server state

Built category-based store and profile appearance screens and connected them to owned-item and appearance data. The server's match and account processing supports the loop from lobby selection through gameplay and results; the matching policy is explained separately below.

[StoreManager.cs (restore-1121)](https://github.com/YSL-RyuDo/DumbGuys/blob/6c30b670a68da9a89d8ad0445de376cd620a21a7/Assets/Scripts/StoreScene/StoreManager.cs), [My change edffcc8](https://github.com/YSL-RyuDo/DumbGuys/commit/edffcc86e34c4d817a000462dbbe19502d4843fe), [MyPageManager.cs (restore-1121)](https://github.com/YSL-RyuDo/DumbGuys/blob/6c30b670a68da9a89d8ad0445de376cd620a21a7/Assets/Scripts/MyPageScene/MyPageManager.cs), [My change 94edd35](https://github.com/YSL-RyuDo/DumbGuys/commit/94edd358e09412d75e7836ee8f7ff17519f206fe), [MatchManager.cpp (main)](https://github.com/YSL-RyuDo/DumbGuysServer/blob/75ad6626d21d7638d37240c85ff4a9aa481e5620/DumbGuysServer/MatchManager.cpp), [My change bb103b5](https://github.com/YSL-RyuDo/DumbGuysServer/commit/bb103b5d1c143534126cf8dfb577812eef035d90)

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

### Different movement, one physics motor

**Implementation**

IInputProvider supplies direction and jump input; movement strategies call CharacterMotor with different movement multipliers. The motor preserves vertical velocity while blending horizontal velocity and handles turning and grounded jumps.

**Design rationale**

A slippery or dash effect changes movement behavior without requiring another copy of Rigidbody handling. Preserving Y velocity also avoids overwriting gravity or an existing jump when updating planar movement.

**Alternatives and the current tradeoff**

A single movement script with flags is economical for a small number of effects. Separate strategies make the variation explicit but add indirection. The current strategies are small; their value depends on maintaining one shared motor rather than scattering physics changes across them.

**Tradeoffs and verification**

The motor still depends on Unity physics and the game's ground query. Slope behavior, moving platforms, changing effects during a jump and network corrections need gameplay tests. A strategy interface alone does not demonstrate that those cases pass.

[DumbGuys/IInputProvider.cs (restore-1121)](https://github.com/YSL-RyuDo/DumbGuys/blob/6c30b670a68da9a89d8ad0445de376cd620a21a7/Assets/Scripts/Player/IInputProvider.cs), [DumbGuys/IMovementStrategy.cs (restore-1121)](https://github.com/YSL-RyuDo/DumbGuys/blob/6c30b670a68da9a89d8ad0445de376cd620a21a7/Assets/Scripts/Player/IMovementStrategy.cs), [DumbGuys/CharacterMotor.cs (restore-1121)](https://github.com/YSL-RyuDo/DumbGuys/blob/6c30b670a68da9a89d8ad0445de376cd620a21a7/Assets/Scripts/Player/CharacterMotor.cs)

## Open and inspect

Open [DumbGuysServer.sln](DumbGuysServer.sln) in Visual Studio with the Windows C++ workload. Inspect the selected configuration, data paths, dependencies and endpoint settings before starting the matching client. Preserve existing account data.

[DumbGuys](https://github.com/YSL-RyuDo/DumbGuys)

## Development and verification scope

Historical development server. This README makes no current performance or fault-tolerance claim. The client's later restore-1121 branch should be considered when selecting a compatible demo revision.

## Related repositories

[DumbGuys](https://github.com/YSL-RyuDo/DumbGuys), Private, access required

[DumbGuysServer](https://github.com/YSL-RyuDo/DumbGuysServer), Private, access required
