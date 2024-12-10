# LWP Engine
[![DebugBuild](https://github.com/TodaYuusuke/DirectXGame/actions/workflows/DebugBuild.yml/badge.svg)](https://github.com/TodaYuusuke/DirectXGame/actions/workflows/DebugBuild.yml)  
LWPエンジンは、2023年4月より制作が始まった、3Dゲームを作成するための自作エンジンです。  
技術力を高めるため、日々開発に励んでいます。  

## エンジン機能
エンジンの機能は分類ごとに名前空間にまとめられており、フォルダも分けられています。  
以下に分類ごとに機能解説を記します。  

### - [Base](Engine/base)  
DirectX12、ウィンドウ管理など、エンジンの基礎部分となるクラスが入っています。  

---
### - [Info](Engine/info)  
フレームレートなど、エンジンのデバッグに必要な情報系を管理するクラスが入っています。  

---
### - [Input](Engine/input)  
キーボード、コントローラーの入力を受け付けるクラスが入っています。  

  + **Cameraクラス**  
レンダリングの視点とポストプロセスの設定をメンバにもつクラスです。  
ウィンドウに表示されるのはISceneクラスのSetMainRenderCamera関数で登録されたカメラからの視点です。  
GetTexture関数でこのカメラ視点のレンダリング結果のテクスチャを取得できます。  

  + **Particleクラス**  
RigidModelをパーティクルトして、複数のインスタンスを管理するクラスです。  
このクラスを継承し、パーティクルを生成する関数、更新する関数を定義して使用します。

---
### - [Math](Engine/math)  
ベクトル、行列、クォータニオンなどの数学的なクラスが入っています。  

  + **Vector**  
2次元、3次元、4次元のベクトルが用意されいます。  
四則演算のオペレーターオーバーロード、Normalize、Length、Dotなどの基本的な関数も用意されているので、そのまま使用できます。

  + **Matrix**  
3x3、4x4の行列が用意されいます。  
アフィン変換行列の生成や、特殊な行列の生成も静的な関数で定義されています。  
Vectorとの乗算も定義されているのでそのまま使用できます。  

---
### - [Object](Engine/object)  
カメラ、パーティクル、当たり判定など、ゲーム作りに使用するオブジェクトのクラスが入っています。  
Object内のクラスは、コンストラクタにて自動的にエンジンにポインタが登録されるため、ユーザはメンバのUpdate関数などを呼び出す必要はありません。  

  + **Cameraクラス**  
レンダリングの視点とポストプロセスの設定をメンバにもつクラスです。  
ウィンドウに表示されるのはISceneクラスのSetMainRenderCamera関数で登録されたカメラからの視点です。  
GetTexture関数でこのカメラ視点のレンダリング結果のテクスチャを取得できます。  

  + **Particleクラス**  
パーティクルを作るのに使うクラスです。このクラスを継承してパーティクルを作成してください。  
生成と更新に使う純粋仮想関数を定義すれば、任意のモデルをパーティクルに使用することができます。  

---
### - [Primitive](Engine/primitive)  
VertexShaderを使って描画される、キューブ、スフィア、スプライトなどの基本的な形状のクラスが入っています。  
Primitive内のクラスは、コンストラクタにて自動的にエンジンにポインタが登録されるため、ユーザはメンバのUpdate関数などを呼び出す必要はありません。  

Primitiveの描画は、独自の処理でDrawCallを圧縮し、軽量化されています。
![スクリーンショット 2024-11-12 101602](https://github.com/user-attachments/assets/7a402d4c-cb0d-432f-b58d-d3649aa2676b)
![スクリーンショット 2024-11-12 101552](https://github.com/user-attachments/assets/a5a599c4-d29c-4919-a6e0-f799a3be3e05)

---
### - [Resources](Engine/resources)  
3Dモデル、オーディオ、テクスチャ、アニメーションなど、ゲームで使用するリソース系を管理するクラスが入っています。  
読み込まれた3DモデルはMeshShaderで描画され、同一のモデルを複数描画する場合はAmpShaderによって自動的に増幅されて描画されます。  
Resources内のクラスは、コンストラクタにて自動的にエンジンにポインタが登録されるため、ユーザはメンバのUpdate関数などを呼び出す必要はありません。  

  + **モデル系**  
    + **RigidModel**  
一番標準的なクラスです。固体としてモデルを読み込みます。  
    + **SkinningModel**  
スキニングをするとしてモデルを読み込みます。後述するAnimationクラスを併用してアニメーションを再生できます。  
    + **StaticModel**  
動的な移動ができないモデルとして読み込みます。ApplyWorldTransform関数を使用しモデルの配置を決定します。
これはレンダリング時に頂点位置を計算するのではなく、計算された頂点位置を保存しているため描画不可が多少抑えられています。  
    + **EMapModel**  
動的な環境マッピングが行われるモデルとして読み込みます。モデルの中心から6方向にレンダリングし、
ライティングがonのマテリアルに反射が映り込みます。
---
### - [Scene](Engine/scene)  
ゲームのシーンを管理するクラスが入っています。  
シーンを作成する際はISceneクラスを継承して実装してください。  

---
### - [Utility](Engine/utility)  
その他便利な関数群が入っています。各種関数の詳細はコメント文に記載されています。  

---
### - [外部ライブラリ](Externals)  
  + [DirectXTex](https://github.com/microsoft/DirectXTex.git)  
  + [DirectXMesh](https://github.com/microsoft/DirectXMesh.git)  
  + [ImGui](https://github.com/ocornut/imgui.git)  
  + [Assimp](https://github.com/assimp/assimp.git)  
  + [nlohmann - json](https://github.com/nlohmann/json.git)  


## サンプルゲーム
このエンジンを使って作られたサンプルゲームの[プロジェクト](SampleGame)が含まれています。  
[最新の進捗動画](https://youtu.be/grzkSSoFcx4)  

また、このエンジンを使用して制作されたゲームのプロジェクトを以下にまとめてあります。  

---
### アンダーガンナー
![SS_UnderGunner_Title](https://github.com/user-attachments/assets/6c6ad8c5-d32d-45f8-953e-b86e73319850)
![SS_UnderGunner_Play](https://github.com/user-attachments/assets/c04bacbc-218c-4d64-9364-bde4ae5b67f9)  
開発期間：10日　開発人数：3人  
[Githubリンク](https://github.com/TodaYuusuke/GJ1_LE3.git)

---
### 切り切り舞
![SS_kirikirimai_Title](https://github.com/user-attachments/assets/8b3387fc-7fba-4acd-aaf4-b7ed94bc751d)
![SS_kirikirimai_Play](https://github.com/user-attachments/assets/09aab4f1-4cdc-4bf2-9ea8-fd4b3218fe3e)  
開発期間：4ヶ月　開発人数：4人  
[Githubリンク](https://github.com/TodaYuusuke/TD3.git)
