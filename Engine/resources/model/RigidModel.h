#pragma once
#include "IModel.h"

// 前方宣言
namespace LWP::Resource {
	class RigidModel;
}

namespace LWP::Resource {
	/// <summary>
	/// 3Dモデルを扱うアダプタークラス
	/// </summary>
	class RigidModel : public IModel {
	public: // ** パブリックなメンバ変数 ** //

		// マテリアル
		std::vector<Primitive::Material> materials;

		// トランスフォームのバッファー
		//Base::ConstantBuffer<Base::InstanceRigidData> buffer;
		// マテリアルバッファー
		//std::unique_ptr<Base::StructuredBuffer<Base::MaterialStruct>> mBuffers;

	public: // ** メンバ関数 ** //

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		RigidModel();
		RigidModel(const RigidModel& other);
		/// <summary>
		/// デフォルトデストラクタ
		/// </summary>
		~RigidModel() override;

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


		public:	// ** オペレータオーバーロード ** //
		
		// コピー演算子
		RigidModel& operator=(const Resource::RigidModel& other) {
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

/// <summary>
/// RigidModelを継承したクラスのみを選択できるテンプレート
/// </summary>
template<class RigidModelT>
concept IsRigidModel = std::is_base_of<LWP::Resource::RigidModel, RigidModelT>::value;