#pragma once
#include "object/TransformEuler.h"

#include <string>

// 前方宣言
namespace LWP::Base {
	class RendererManager;
}
namespace LWP::Resource {
	class Manager;
}

namespace LWP::Resource {
	/// <summary>
	/// 3Dモデルを扱うアダプタークラス
	/// </summary>
	class Model final {
	public: // ** パブリックなメンバ変数 ** //

		// ワールドトランスフォーム
		Object::TransformEuler worldTF{};

		// ライティングを行うかどうか
		bool enableLighting = false;
		// ワイヤーフレームで描画する
		//bool isWireFrame = false;
		// アクティブ切り替え
		bool isActive = true;


	public: // ** メンバ関数 ** //

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		Model();
		/// <summary>
		/// デフォルトデストラクタ
		/// </summary>
		~Model();

		/// <summary>
		/// 3Dモデルのデータを読み込む
		/// </summary>
		/// <param name="fileName">読み込むファイルの名前</param>
		void Load(const std::string& fileName);
		/// <summary>
		/// 3Dモデルのデータを読み込む（resources/model/を短縮ver）
		/// </summary>
		/// <param name="filePath">読み込むファイルの名前</param>
		void LoadShortPath(const std::string& filePath);
		/// <summary>
		/// 3Dモデルのデータを読み込む（exeからのパス指定）
		/// </summary>
		/// <param name="filePath">読み込むファイルの名前</param>
		void LoadFullPath(const std::string& filePath);

		/// <summary>
		/// 描画（ユーザー呼び出し禁止）
		/// </summary>
		void Draw(Base::RendererManager* render, Resource::Manager* resource);

		/// <summary>
		/// デバッグ用ImGui
		/// </summary>
		void DebugGUI();


	private: // ** メンバ変数 ** //

		// 短縮用パス
		const static std::string kDirectoryPath;
		// リソースマネージャー上のモデルを指す為の変数（パスの予定）
		std::string filePath = "";

	};
}