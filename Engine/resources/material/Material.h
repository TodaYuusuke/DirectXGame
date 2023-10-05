#pragma once
#include "../../math/matrix/Matrix4x4.h"
#include "../../Object/WorldTransform.h"

// 前方宣言
namespace LWP::Base {
	class CommandManager;
}

/// <summary>
/// マテリアル
/// </summary>
namespace LWP::Resource {
	enum class FillMode : int {
		WireFrame,	// ワイヤーフレーム
		Fill,		// 埋め立て
	};

	class Material final {
	public:
		// uvTransform
		Object::WorldTransform uvTransform;
		// ライティングを行うかどうか
		bool enableLighting = false;
		// ブレンドモード
		// 
		// 形状を描画する際の設定
		FillMode fillMode = FillMode::Fill;

		// コンストラクタ
		Material() = delete;
		Material(Base::CommandManager* manager);
		// デストラクタ
		~Material() = default;
		
		/// <summary>
		/// ImGui
		/// </summary>
		void DebugGUI(const std::string& label = "Material");


	private: // ** プロパティ変数 ** //
		// CommandManager上でのインデックス
		int index_;
	public: // アクセッサ
		// CommandManager上でのインデックスを読みとる
		int GetIndex() const { return index_; }
		// 別のMaterialと同期する
		void SetIndex(int value) { index_ = value; }
	};
}