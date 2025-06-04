#pragma once
#include "../model/RigidModel.h"
#include "../model/SkinningModel.h"
#include "../model/StaticModel.h"
#include "object/core/Terrain.h"

#include "object/core/Camera.h"

#include "object/core/collision/Collision.h"

#include "utility/CatmullRom.h"

#pragma warning(push)
#pragma warning(disable:26819)
#include "../Externals/nlohmann/json.hpp"
#pragma warning(pop)


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
		std::map<std::string, Object::Collision> collisions;
		// CatmullRom曲線
		std::map<std::string, Utility::CatmullRom> catmullRomCurves;

		// プレイヤーのスポーン地点
		std::map<std::string, Math::Vector3> playerSpawnPoint;
		// エネミーのスポーン地点
		std::map<std::string, Math::Vector3> enemySpawnPoint;

		// 地形
		std::unique_ptr<Object::Terrain> terrain;	// ない場合もあるのでユニークポインタ

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
		/// 全体のスケールにかける倍率（読み込み時に適応）
		/// </summary>
		void SetScaleMultiply(float s);


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
		// 全体のスケール倍率
		float scale = 1.0f;

		// 短縮用パス
		const static std::string kDirectoryPath;
		// リソースマネージャー上のモデルを指す為の変数（パスの予定）
		std::string filePath = "";


	private: // ** プライベートなメンバ関数 ** //

		/// <summary>
		/// Objectを解凍
		/// </summary>
		/// <param name="data"></param>
		void LoadObject(const nlohmann::json& data);

		/// <summary>
		/// Vector3を解凍
		/// </summary>
		/// <param name="data"></param>
		Math::Vector3 LoadVector3(const nlohmann::json& data);
		/// <summary>
		/// Quaternionを解凍
		/// </summary>
		/// <param name="data"></param>
		Math::Quaternion LoadQuaternion(const nlohmann::json& data);
		/// <summary>
		/// ワールドトランスフォームを解凍
		/// </summary>
		/// <param name="data"></param>
		Object::TransformQuat LoadWorldTF(const nlohmann::json& data);
		/// <summary>
		/// タイプ：メッシュを解凍
		/// </summary>
		/// <param name="data"></param>
		void LoadMesh(const nlohmann::json& data, const std::string& name);
		/// <summary>
		/// タイプ：コライダーを解凍
		/// </summary>
		/// <param name="data"></param>
		void LoadCollider(const nlohmann::json& data, const std::string& name, const Object::TransformQuat wtf);
		/// <summary>
		/// タイプ：Curveを解凍
		/// </summary>
		/// <param name="data"></param>
		void LoadCurve(const nlohmann::json& data, const std::string& name);
		/// <summary>
		/// Terrainを解凍
		/// </summary>
		/// <param name="data"></param>
		void LoadTerrain(const nlohmann::json& data);
		/// <summary>
		/// タイプ：PlayerSpawnを解凍
		/// </summary>
		/// <param name="data"></param>
		void LoadPlayerSpawn(const nlohmann::json& data, const std::string& name);
		/// <summary>
		/// タイプ：EnemySpawnを解凍
		/// </summary>
		/// <param name="data"></param>
		void LoadEnemySpawn(const nlohmann::json& data, const std::string& name);


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
		/// <summary>
		/// CatmullRom用ImGui
		/// </summary>
		void CatmullRomDebugGUI();

	};
}