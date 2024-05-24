#pragma once
#include "IModel.h"

// 前方宣言
namespace LWP::Resource {
	class RigidModel;
}

// モデル別のデータの構造体（AS実装のときに役立つ予定）
namespace LWP::Base {
	struct InstanceRigidData {
		Base::WTFStruct wtf;
		int32_t enableLighting;

		InstanceRigidData() = default;
		InstanceRigidData(const Resource::RigidModel& value);
		// Materialクラスのデータを代入する演算子をオーバーロード
		InstanceRigidData& operator=(const Resource::RigidModel& value);
	};
}

namespace LWP::Resource {
	/// <summary>
	/// 3Dモデルを扱うアダプタークラス
	/// </summary>
	class RigidModel final : public IModel {
	public: // ** パブリックなメンバ変数 ** //

		// マテリアル
		std::vector<Primitive::Material> materials;

		// トランスフォームのバッファー
		Base::ConstantBuffer<Base::InstanceRigidData> buffer;
		// マテリアルバッファー
		std::unique_ptr<Base::StructuredBuffer<Base::MaterialStruct>> mBuffers;

	public: // ** メンバ関数 ** //

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		RigidModel();
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


	public: // ** 標準的な形状を使いたいとき用のデータ ** //
		/// <summary>
		/// 標準形状一覧
		/// </summary>
		enum class Standard {
			Cube,	// 立方体
			Sphere,	// スフィア
		};
	private: // **  ** //
	};
}