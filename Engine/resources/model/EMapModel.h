#pragma once
#include "IModel.h"

namespace LWP::Resource {
	/// <summary>
	/// 環境マップ効果のある3Dモデルを扱うアダプタークラス
	/// </summary>
	class EMapModel : public IModel {
	public: // ** パブリックなメンバ変数 ** //

		// ワールドトランスフォーム
		Object::TransformQuat worldTF{};
		// マテリアル
		std::vector<Primitive::Material> materials;

		// モデルの中心座標（環境マップの中心）
		Math::Vector3 modelCenterPosition;

	public: // ** メンバ関数 ** //

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		EMapModel();
		EMapModel(const EMapModel& other);
		/// <summary>
		/// デフォルトデストラクタ
		/// </summary>
		~EMapModel() override;

		/// <summary>
		/// 3Dモデルのデータを読み込む（exeからのパス指定）
		/// </summary>
		/// <param name="filePath">読み込むファイルの名前</param>
		void LoadFullPath(const std::string& filePath) override;
		/// <summary>
		/// 標準モデルのデータを読み込む
		/// </summary>
		void LoadCube() { LoadFullPath("resources/system/model/standard/cube.gltf"); }
		void LoadSphere() { LoadFullPath("resources/system/model/standard/sphere.gltf"); }

		/// <summary>
		/// 更新（ユーザー呼び出し禁止）
		/// </summary>
		void Update() override;

		/// <summary>
		/// デバッグ用ImGui
		/// </summary>
		void DebugGUI() override;

		/// <summary>
		/// 全マテリアルのenableLightingを切り替え
		/// </summary>
		void SetAllMaterialLighting(bool flag);

		
	public:	// ** オペレータオーバーロード ** //

		// Observerクラス用のオペレーターオーバーロード
		bool operator==(const EMapModel& other) const = delete;
		bool operator==(EMapModel& other) {
			return worldTF == other.worldTF &&
				isActive == other.isActive;
		}

		// コピー演算子
		EMapModel& operator=(const Resource::EMapModel& other) {
			if (this != &other) {
				this->LoadFullPath(other.filePath);
				worldTF = other.worldTF;
				isActive = other.isActive;
			}
			return *this;
		}
	};
	

	// RigidModelのデータのみ（Observerクラス用）
	struct EMapModelStruct {
		Object::TransformQuat worldTF;

		EMapModelStruct& operator=(const EMapModel& other) {
			worldTF = other.worldTF;
			return *this;
		}
		bool operator==(EMapModel& other) {
			return worldTF == other.worldTF;
		}
	};
}