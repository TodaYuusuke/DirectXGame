#pragma once
#include "../model/RigidModel.h"
#include "../model/SkinningModel.h"
#include "../model/StaticModel.h"
#include "object/core/Terrain.h"

#include "object/core/Camera.h"

#include "object/collider/Collider.h"

#include "../Externals/nlohmann/json.hpp"


namespace LWP::Resource {
	/// <summary>
	/// Blenderで作成したシーンを読み込むクラス
	/// </summary>
	class LevelData {
	public: // ** パブリックなメンバ変数 ** //

		// リキッドモデル
		std::map<std::string, RigidModel> rigidModels;
		// スキンモデル
		std::map<std::string, SkinningModel> skinModels;
		// スタティックモデル
		std::map<std::string, StaticModel> staticModels;
		// 当たり判定
		std::map<std::string, Object::Collider::Collider> colliders;

		// 地形
		std::unique_ptr<Object::Terrain> terrain;

	public: // ** メンバ関数 ** //

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		LevelData() = default;
		/// <summary>
		/// デストラクタ
		/// </summary>
		~LevelData();

		/// <summary>
		/// シーンのファイルを読み込む（ファイル名から検索）
		/// </summary>
		/// <param name="fileName">読み込むファイルの名前</param>
		void Load(const std::string& fileName);
		/// <summary>
		/// シーンのファイルを読み込む（resources/model/を短縮ver）
		/// </summary>
		/// <param name="filePath">読み込むファイルの名前</param>
		void LoadShortPath(const std::string& filePath);
		/// <summary>
		/// シーンのファイルを読み込む（exeからのパス指定）
		/// </summary>
		/// <param name="filePath">読み込むファイルの名前</param>
		void LoadFullPath(const std::string& filePath);

		/// <summary>
		/// シーンのファイルを再読み込み
		/// </summary>
		void HotReload();


		/// <summary>
		/// デバッグ用ImGui
		/// </summary>
		void DebugGUI();

		/// <summary>
		/// 読み込んだカメラのデータを適応するカメラクラスのポインタをセット
		/// </summary>
		/// <param name="camera">カメラクラスのポインタ</param>
		void SetCameraPtr(LWP::Object::Camera* camera) { cameraPtr = camera; }
		/// <summary>
		/// 読み込み済みのパスを取得
		/// </summary>
		std::string LoadedFilePath() { return filePath; }

	private: // ** メンバ変数 ** //

		// カメラのポインタ
		LWP::Object::Camera* cameraPtr = nullptr;

		// 短縮用パス
		const static std::string kDirectoryPath;
		// リソースマネージャー上のモデルを指す為の変数（パスの予定）
		std::string filePath = "";


	private: // ** プライベートなメンバ関数 ** //

		/// <summary>
		/// ワールドトランスフォームを解凍
		/// </summary>
		/// <param name="data"></param>
		Object::TransformQuat LoadWorldTF(const nlohmann::json& data);

		/// <summary>
		/// RigidModel用ImGui
		/// </summary>
		void RigidDebugGUI();
		/// <summary>
		/// SkinModel用ImGui
		/// </summary>
		void SkinDebugGUI();
		/// <summary>
		/// StaticModel用ImGui
		/// </summary>
		void StaticDebugGUI();
		/// <summary>
		/// Collider用ImGui
		/// </summary>
		void ColliderDebugGUI();
	};
}