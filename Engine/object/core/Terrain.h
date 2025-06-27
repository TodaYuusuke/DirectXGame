#pragma once
#include "../IObject.h"
#include "collision/Collision.h"
#include "resources/model/StaticModel.h"

#include <map>

namespace LWP::Object {
	/// <summary>
	/// 地形クラス
	/// </summary>
	class Terrain final : public IObject {
	public: // ** メンバ関数 ** //
		
		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		Terrain();
		/// <summary>
		/// デストラクタ
		/// </summary>
		~Terrain();

		// 初期化
		void Initialize() override;
		// 更新
		void Update(Base::RendererManager* manager) override;

		// 地形に使うモデルデータを読み込む
		void LoadModel(std::string filePath, const TransformQuat& wtf);
		// デバッグ用GUI
		void DebugGUI() override;

	public: // ** パブリックなメンバ変数 ** //
		// コライダー
		Object::Collision collision;

	private: // ** メンバ変数 ** //

		// 地形のモデル
		Resource::StaticModel model_;
		// Terrainコライダーの参照
		Object::Collider::Terrain& collider_;
	};
}
