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
	public: // メンバ関数

		// 初期シーンを指定して初期化
		void Initialize(IScene* firstScene);

		// 更新
		void Update();
		// 描画
		void Draw();

		/// <summary>
		/// CommandManagerにビュープロジェクション行列を登録するため、カメラのポインタを返す関数
		/// </summary>
		/// <returns>カメラのポインタ</returns>
		const LWP::Object::Camera* GetMainCamera();

	private: // メンバ変数
		// 現在のシーン
		IScene* currentScene_;
	};
}