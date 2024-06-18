#pragma once
#include "IModel.h"

namespace LWP::Resource {
	/// <summary>
	/// 実行中に動かすことのない3Dモデル
	/// </summary>
	class StaticModel : public IModel {
	public: // ** パブリックなメンバ変数 ** //

		// マテリアル
		std::vector<Primitive::Material> materials;


	public: // ** メンバ関数 ** //

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		StaticModel();
		StaticModel(const StaticModel& other);
		/// <summary>
		/// デフォルトデストラクタ
		/// </summary>
		~StaticModel() override;

		/// <summary>
		/// 3Dモデルのデータを読み込む（exeからのパス指定）
		/// </summary>
		/// <param name="filePath">読み込むファイルの名前</param>
		void LoadFullPath(const std::string& filePath) override;

		/// <summary>
		/// 更新（ユーザー呼び出し禁止）
		/// </summary>
		void Update() override;

		/// <summary>
		/// デバッグ用ImGui
		/// </summary>
		void DebugGUI() override;

		/// <summary>
		/// 埋め立てかワイヤーフレームで描画するかを切り替える
		/// </summary>
		void ChangeFillMode();


	public:	// ** オペレータオーバーロード ** //

		// Observerクラス用のオペレーターオーバーロード
		bool operator==(const StaticModel& other) const = delete;
		bool operator==(StaticModel& other) {
			return worldTF == other.worldTF &&
				enableLighting == other.enableLighting &&
				isActive == other.isActive;
		}

		// コピー演算子
		StaticModel& operator=(const Resource::StaticModel& other) {
			if (this != &other) {
				this->LoadFullPath(other.filePath);
				worldTF = other.worldTF;
				enableLighting = other.enableLighting;
				isActive = other.isActive;
			}
			return *this;
		}
	};
}