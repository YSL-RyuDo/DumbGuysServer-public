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

| 領域 | 実装 |
|---|---|
| アカウント | アカウント・接続処理 |
| ロビーから試合へ | ルーム・プレイヤー状態とUnityの連携 |
| プレイヤーデータ | ショップ・外見状態 |

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

## 実行・コードの確認

Windows C++ワークロードを備えたVisual Studioで[DumbGuysServer.sln](DumbGuysServer.sln)を開いてください。構成・データパス・依存関係・接続設定を確認し、対応するクライアントを起動します。既存アカウントデータは維持してください。

[DumbGuys](https://github.com/YSL-RyuDo/DumbGuys)

## 開発・検証範囲

過去の開発用サーバーです。現行性能や耐障害性の検証完了は主張しません。互換性のあるデモ版を選ぶ際はクライアントのrestore-1121も確認してください。

## 関連リポジトリ

[DumbGuys](https://github.com/YSL-RyuDo/DumbGuys), 非公開・アクセス権が必要

[DumbGuysServer](https://github.com/YSL-RyuDo/DumbGuysServer), 非公開・アクセス権が必要
