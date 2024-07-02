#pragma once
#include <Adapter.h>

class IScene {
public:
	// 仮想デストラクタ
	virtual ~IScene() = default;

	// 共通の処理
	void PreInitialize() {
		// デフォルトのメインカメラを登録
		mainCamera.transform.translation = { 0.0f,0.0f,-10.0f };
		mainCamera.name = "MainCamera";
		mainCamera.isActive = false;	// 複数画面描画のときのみtrueにすればいいのでfalse
		SetMainRenderCamera(&mainCamera);
		// levelDataの適応対象にmainCameraをセット
		levelData.SetCameraPtr(&mainCamera);
	}

	//*** 純粋仮想関数 ***//

	// 初期化
	virtual void Initialize() = 0;
	// 更新
	virtual void Update() = 0;
	
	// 次のシーンを生成する関数を代入
	// 返り値：次のシーンの新しいインスタンス
	std::function<IScene*()> nextSceneFunction = nullptr;


protected:	// ** メンバ変数 ** //
	
	// シーンが持つデフォルトのカメラ
	LWP::Object::Camera mainCamera;
	// レベルエディタのデータ読み込み用クラス
	LWP::Resource::LevelData levelData;


private: // ** プロパティ変数 ** //
	// メイン画面の描画を担当するカメラのポインタ
	LWP::Object::Camera* mainRenderCamera_ = nullptr;
public:
	// メインのレンダリングに使うカメラをセットする関数
	void SetMainRenderCamera(LWP::Object::Camera* setCamera) { mainRenderCamera_ = setCamera; }
	// メインのレンダリングで使うカメラを渡す関数
	LWP::Object::Camera* GetMainRenderCamera() { return mainRenderCamera_; }
};
