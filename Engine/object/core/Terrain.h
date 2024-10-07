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

		// デフォルトコンストラクタ
		Terrain();

		// 初期化
		void Initialize() override;
		// 更新
		void Update(Base::RendererManager* manager) override;

		// 地形に使うモデルデータを読み込む
		void LoadModel(std::string filePath, const TransformQuat& wtf);
		// デバッグ用GUI
		void DebugGUI() override;


	private: // ** メンバ変数 ** //

		// 地形のモデル
		Resource::StaticModel model_;
		// コライダー
		Object::Collision collision_;
		// テレインコライダーの参照
		Object::Collider::Terrain& collider_;
	};
}
