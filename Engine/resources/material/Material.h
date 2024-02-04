#pragma once
#include "../../math/matrix/Matrix4x4.h"
#include "../../Object/WorldTransform.h"

/// <summary>
/// マテリアル
/// </summary>
namespace LWP::Resource {
	enum class FillMode : bool {
		WireFrame,	// ワイヤーフレーム
		Fill,		// 埋め立て
	};

	class Material final {
	public:
		// uvTransform
		Object::WorldTransform uvTransform;
		// ライティングを行うかどうか
		bool enableLighting = false;
		// 光沢度
		float shininess = 100000.0f;	// この値にするとほぼ見えない
		// ブレンドモード
		// 
		// 形状を描画する際の設定
		FillMode fillMode = FillMode::Fill;

		// コンストラクタ
		Material();
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