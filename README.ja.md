# DumbGuys / Server

[English](README.md), [Korean](README.ko.md), [Japanese](README.ja.md)

アカウント、ロビー、試合、外見データをつなぐマルチプレイヤーのチームゲームです。

| プロジェクト情報 | 内容 |
|---|---|
| 開発状況 | 開発記録 |
| 使用技術 | C++, C#, Unity |

[開発事例を見る](https://ysl-ryudo.github.io/ja/work/dumbguys/), [Portfolio](https://ysl-ryudo.github.io/ja/)

[![DumbGuys](https://i.ytimg.com/vi/Z4tffAvRjJs/hqdefault.jpg)](https://www.youtube.com/watch?v=Z4tffAvRjJs)

[DumbGuys full play](https://www.youtube.com/watch?v=Z4tffAvRjJs)

## ゲーム全体で担当した実装

UnityクライアントとC++サーバーで移動、障害物、チュートリアル、外見・ショップ、試合通信を開発しました。restore-1121には小さいmain版にないクライアント作業があります。Unityパッケージ・サンプル・外部画像は第三者の制作物です。

**プレイの流れ**

ログイン → ロビーと外見 → マッチングとロード → 障害物コース → 到達・落下 → 結果

ソースリンクは確認した版に固定し、担当リンクは自分のマージ以外の変更を示します。共同ファイルは単独著作を意味しません。非公開ソースには閲覧権限が必要です。

### 移動戦略とキャラクター操作

入力、移動戦略、キャラクター状態、Rigidbodyモーターを分離しました。通常・滑り・ダッシュが共通モーターで移動を変え、アニメーションは状態に従います。移動の違いが共通の物理実装を利用します。

[CharacterMotor.cs (restore-1121)](https://github.com/YSL-RyuDo/DumbGuys/blob/6c30b670a68da9a89d8ad0445de376cd620a21a7/Assets/Scripts/Player/CharacterMotor.cs), [担当変更 27bfa58](https://github.com/YSL-RyuDo/DumbGuys/commit/27bfa58811079f0886733c49174ce9627cbae910), [IMovementStrategy.cs (restore-1121)](https://github.com/YSL-RyuDo/DumbGuys/blob/6c30b670a68da9a89d8ad0445de376cd620a21a7/Assets/Scripts/Player/IMovementStrategy.cs), [担当変更 e9d0b38](https://github.com/YSL-RyuDo/DumbGuys/commit/e9d0b380cfe1eb297fbf68c38a00f77ad37b53f2)

### 障害物とスプライン移動

押し出し・打上げ、移動床への追従、曲線パイプ移動を実装しました。パイプ中はRigidbody処理を切り替え、スプライン位置と接線に沿って進み、最後に操作を戻します。経路用エディター生成器も制作しました。

[PipeTransportCurved.cs (restore-1121)](https://github.com/YSL-RyuDo/DumbGuys/blob/6c30b670a68da9a89d8ad0445de376cd620a21a7/Assets/Scripts/PipeTransportCurved.cs), [担当変更 359f276](https://github.com/YSL-RyuDo/DumbGuys/commit/359f27621811e1cdc667328cfb55d799340cb3e6), [PipeSplineGenerator.cs (restore-1121)](https://github.com/YSL-RyuDo/DumbGuys/blob/6c30b670a68da9a89d8ad0445de376cd620a21a7/Assets/Editor/PipeSplineGenerator.cs), [担当変更 27bfa58](https://github.com/YSL-RyuDo/DumbGuys/commit/27bfa58811079f0886733c49174ce9627cbae910)

### 試合開始と相手の表示

ロード準備、出現位置・外見の交換、相手位置・回転の表示を実装しました。GameSceneControllerは被弾・結果メッセージも処理します。受信スレッドからシーンを直接操作せずUnityメインスレッドへ渡します。

[LoadingSceneController.cs (restore-1121)](https://github.com/YSL-RyuDo/DumbGuys/blob/6c30b670a68da9a89d8ad0445de376cd620a21a7/Assets/Scripts/LoadingScene/LoadingSceneController.cs), [担当変更 87533da](https://github.com/YSL-RyuDo/DumbGuys/commit/87533da63f475a3a306ffa38833e6c72ed8b378e), [GameSceneController.cs (restore-1121)](https://github.com/YSL-RyuDo/DumbGuys/blob/6c30b670a68da9a89d8ad0445de376cd620a21a7/Assets/Scripts/GameScene/GameSceneController.cs), [担当変更 6d033d0](https://github.com/YSL-RyuDo/DumbGuys/commit/6d033d0bfd08eaf765251653d99a5b85253ca821), [UnityMainThreadDispatcher.cs (restore-1121)](https://github.com/YSL-RyuDo/DumbGuys/blob/6c30b670a68da9a89d8ad0445de376cd620a21a7/Assets/Scripts/Network/UnityMainThreadDispatcher.cs), [担当変更 87533da](https://github.com/YSL-RyuDo/DumbGuys/commit/87533da63f475a3a306ffa38833e6c72ed8b378e)

### チュートリアルとコース終了

案内・落下・到達トリガーを備えるチュートリアル管理と、ゲームの到達・落下処理を実装しました。移動だけでなく、操作習得とコース終了も担当しました。

[Tutorial0Manager.cs (restore-1121)](https://github.com/YSL-RyuDo/DumbGuys/blob/6c30b670a68da9a89d8ad0445de376cd620a21a7/Assets/Scripts/Tutorial0Scene/Tutorial0Manager.cs), [担当変更 6d033d0](https://github.com/YSL-RyuDo/DumbGuys/commit/6d033d0bfd08eaf765251653d99a5b85253ca821), [Tutorial1Manager.cs (restore-1121)](https://github.com/YSL-RyuDo/DumbGuys/blob/6c30b670a68da9a89d8ad0445de376cd620a21a7/Assets/Scripts/Tutorial1Scene/Tutorial1Manager.cs), [担当変更 8b0ac0c](https://github.com/YSL-RyuDo/DumbGuys/commit/8b0ac0c91640a2154f89b77656c9f0daa07ca458), [WinZone.cs (restore-1121)](https://github.com/YSL-RyuDo/DumbGuys/blob/6c30b670a68da9a89d8ad0445de376cd620a21a7/Assets/Scripts/GameScene/WinZone.cs), [担当変更 87533da](https://github.com/YSL-RyuDo/DumbGuys/commit/87533da63f475a3a306ffa38833e6c72ed8b378e)

### 外見、ショップとサーバー状態

カテゴリ別ショップと外見画面を実装し、所有アイテム・装備データに接続しました。サーバーの試合・アカウント処理はロビー選択からプレイと結果までつながります。マッチング方針は後述します。

[StoreManager.cs (restore-1121)](https://github.com/YSL-RyuDo/DumbGuys/blob/6c30b670a68da9a89d8ad0445de376cd620a21a7/Assets/Scripts/StoreScene/StoreManager.cs), [担当変更 edffcc8](https://github.com/YSL-RyuDo/DumbGuys/commit/edffcc86e34c4d817a000462dbbe19502d4843fe), [MyPageManager.cs (restore-1121)](https://github.com/YSL-RyuDo/DumbGuys/blob/6c30b670a68da9a89d8ad0445de376cd620a21a7/Assets/Scripts/MyPageScene/MyPageManager.cs), [担当変更 94edd35](https://github.com/YSL-RyuDo/DumbGuys/commit/94edd358e09412d75e7836ee8f7ff17519f206fe), [MatchManager.cpp (main)](https://github.com/YSL-RyuDo/DumbGuysServer/blob/75ad6626d21d7638d37240c85ff4a9aa481e5620/DumbGuysServer/MatchManager.cpp), [担当変更 bb103b5](https://github.com/YSL-RyuDo/DumbGuysServer/commit/bb103b5d1c143534126cf8dfb577812eef035d90)

## 担当した内容

ゲームクライアントのアカウント・接続処理を実装しました。

ルーム・試合・プレイヤー状態、ショップ・外見関連データを担当しました。

クライアントのロビー・試合フローにサーバーを接続しました。

## 貢献の根拠

[Source baseline / 코드 기준 / コード基準](https://github.com/YSL-RyuDo/DumbGuysServer/tree/2ba094126a0bb09ace4167dc3c2e956c4a39dec2)

[Development history / 개발 이력 / 開発履歴](https://github.com/YSL-RyuDo/DumbGuysServer/commits/main/?author=YSL-RyuDo)

コミットは記載した変更の根拠であり、貢献率の点数ではありません。外部素材・チームメンバーの貢献は区別します。

## 実装と設計

アカウント、ルーム、プレイヤー外見をロビーと試合の流れにつなぐ実装です。クライアントとサーバーの互換性がある版を選ぶことも重要です。

アカウント・接続処理、ルーム・プレイヤー状態、ショップ・外見データ、クライアントのロビー・試合連携を担当しました。

## コードの動作と実装構造の理由

確認した実装が扱う制約と利点・代償を説明します。設計効果の説明はコード分析であり、比較する全代案を当時試作したという意味ではありません。担当リンクは共同制作内の変更を示し、ソースの閲覧権限は維持します。

### 待ち時間に応じてマッチ条件を広げる理由

**実装の流れ**

MatchManagerは待機プレイヤーのランク、スコア、レベルを比較し、待機が長くなると一部の許容範囲を広げます。成立時はルームを作成して参加者と開始位置を送り、ロビーでは所有アイテムと外見情報も交換します。

**この構造が必要な理由**

近い条件の参加者が少ないと、実力の近さと短い待ち時間は両立しにくくなります。時間による基準変更は、即座に誰とでも組む場合や無期限に待つ場合との折合いを規則にします。所有一覧と現在の外見の区別も、持っている物と表示中の物を分離します。

**代案と現在の実装の折合い**

到着順は速さを優先し実力の近さを無視します。固定の近似基準は予測しやすい一方、外れた条件の参加者が待ち続けます。時間での拡張は折合いを明示しますが、適切な閾値には参加者分布と試合品質の根拠が必要です。

**利点・代償と検証範囲**

閾値は実装方針であり、公平性や実際の待ち時間短縮の測定結果ではありません。参加者分布、取消、切断は別途評価が必要です。以下のクライアントはrestore-1121ブランチで、既定ブランチと同じ連携状態とは限りません。

[DumbGuysServer/MatchManager.cpp](https://github.com/YSL-RyuDo/DumbGuysServer/blob/6c9f9da948008ace439ebfc7a1aeb4b130da12f0/DumbGuysServer/MatchManager.cpp), [DumbGuysServer/OwnedItemManager.cpp](https://github.com/YSL-RyuDo/DumbGuysServer/blob/6c9f9da948008ace439ebfc7a1aeb4b130da12f0/DumbGuysServer/OwnedItemManager.cpp), [DumbGuys/ClientSession.cs](https://github.com/YSL-RyuDo/DumbGuys/blob/6c30b670a68da9a89d8ad0445de376cd620a21a7/Assets/Scripts/Network/ClientSession.cs)

### 移動方式を変え、物理処理を共有する

**実装の流れ**

IInputProviderが方向・ジャンプ入力を提供し、移動戦略が異なる倍率でCharacterMotorを呼びます。モーターは垂直速度を維持して水平速度を補間し、回転と接地ジャンプを処理します。

**この構造が必要な理由**

滑りやダッシュを変えるためにRigidbody処理全体を複製する必要がありません。平面移動時にY速度を保つことで重力や進行中のジャンプを上書きしません。

**代案と現在の実装の折合い**

効果が少なければ単一スクリプトとフラグが簡単です。戦略分離は変種を明示する代わりに間接参照を増やします。現在の戦略は小さく、物理変更を各所へ分散せず共通モーターを維持することが重要です。

**利点・代償と検証範囲**

モーターはUnity物理と接地検査に依存します。斜面、移動床、ジャンプ中の効果変更、通信補正にはプレイ検証が必要です。戦略インターフェースだけで合格は示せません。

[DumbGuys/IInputProvider.cs (restore-1121)](https://github.com/YSL-RyuDo/DumbGuys/blob/6c30b670a68da9a89d8ad0445de376cd620a21a7/Assets/Scripts/Player/IInputProvider.cs), [DumbGuys/IMovementStrategy.cs (restore-1121)](https://github.com/YSL-RyuDo/DumbGuys/blob/6c30b670a68da9a89d8ad0445de376cd620a21a7/Assets/Scripts/Player/IMovementStrategy.cs), [DumbGuys/CharacterMotor.cs (restore-1121)](https://github.com/YSL-RyuDo/DumbGuys/blob/6c30b670a68da9a89d8ad0445de376cd620a21a7/Assets/Scripts/Player/CharacterMotor.cs)

## 実行・コードの確認

Windows C++ワークロードを備えたVisual Studioで[DumbGuysServer.sln](DumbGuysServer.sln)を開いてください。構成・データパス・依存関係・接続設定を確認し、対応するクライアントを起動します。既存アカウントデータは維持してください。

[DumbGuys](https://github.com/YSL-RyuDo/DumbGuys)

## 開発・検証範囲

過去の開発用サーバーです。現行性能や耐障害性の検証完了は主張しません。互換性のあるデモ版を選ぶ際はクライアントのrestore-1121も確認してください。

## 関連リポジトリ

[DumbGuys](https://github.com/YSL-RyuDo/DumbGuys), 非公開・アクセス権が必要

[DumbGuysServer](https://github.com/YSL-RyuDo/DumbGuysServer), 非公開・アクセス権が必要
