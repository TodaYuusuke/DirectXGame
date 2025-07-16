#pragma once
#include <Adapter.h>

class IScene {
public:
	// 仮想デストラクタ
	virtual ~IScene() = default;

	// 共通の処理
	void PreInitialize() {
		// デフォルトのメインカメラを登録
		mainCamera.worldTF.translation = { 0.0f,0.0f,-10.0f };
		mainCamera.name = "MainCamera";
		mainCamera.SetMainCamera();
		// levelDataの適応対象にmainCameraをセット
		levelData.SetCameraPtr(&mainCamera);
	}

	//*** 純粋仮想関数 ***//

	// 初期化
	virtual void Initialize() = 0;
	// 更新
	virtual void Update() = 0;

	// LevelDataのDebugGUI呼び出し用関数
	void DebugGUI() { levelData.DebugGUI(); }
	
	// 次のシーンを生成する関数を代入
	// 返り値：次のシーンの新しいインスタンス
	std::function<IScene*()> nextSceneFunction = nullptr;


protected:	// ** メンバ変数 ** //
	
	// シーンが持つデフォルトのカメラ
	LWP::Object::Camera mainCamera;
	// レベルエディタのデータ読み込み用クラス
	LWP::Resource::LevelData levelData;
};

/// <summary>
/// ISceneを継承したクラスのみを選択できるテンプレート
/// </summary>
template<class SceneT>
concept IsIScene = std::is_base_of<IScene, SceneT>::value;