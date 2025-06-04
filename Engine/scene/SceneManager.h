#pragma once

// カメラの前方宣言
namespace LWP::Object {
	class Camera;
};

// ISceneは前方宣言する
class IScene;

namespace LWP::Scene {
	/// <summary>
	/// シーン管理クラス
	/// </summary>
	class Manager final {
	public: // ** メンバ関数 ** //

		/// <summary>
		/// コンストラクタ
		/// </summary>
		Manager() = default;
		/// <summary>
		/// デストラクタ
		/// </summary>
		~Manager();

		// 初期シーンを指定して初期化
		void Init(IScene* firstScene);
		// 更新
		void Update();

		// シーンをクリアする関数
		void SceneClear();
		
		/// <summary>
		/// Debug用ImGui
		/// </summary>
		void DebugGUI();

		/// <summary>
		/// CommandManagerにビュープロジェクション行列を登録するため、カメラのポインタを返す関数
		/// </summary>
		/// <returns>カメラのポインタ</returns>
		LWP::Object::Camera* GetMainCamera();

	private: // メンバ変数
		// 現在のシーン
		IScene* currentScene_;
	};
}