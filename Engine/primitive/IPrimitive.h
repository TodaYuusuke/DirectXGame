#pragma once
#include "object/TransformQuat.h"

#include "component/Vertex.h"
#include "component/Material.h"

namespace LWP::Primitive {
	/// <summary>
	/// 全Primitiveの基底クラス
	/// </summary>
	class IPrimitive {
	public: // ** パブリックなメンバ変数 ** //

		// 識別用の形状名
		std::string name = "primitive";
		
		// 頂点（頂点は固定、可変長のパターンがあるので派生先で定義させる）
		// std::vector<Vertex> vertices;
		
		// ワールドトランスフォーム
		Object::TransformQuat worldTF;
		// マテリアル
		Material material;
		
		// アクティブ切り替え
		bool isActive = true;
		

	public: // ** メンバ関数 ** //
		
		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		IPrimitive() = default;
		/// <summary>
		/// デストラクタ
		/// </summary>
		virtual ~IPrimitive() = default;

		/// <summary>
		/// 初期化（ユーザー呼び出し不要）
		/// </summary>
		virtual void Init();
		/// <summary>
		/// 更新（ユーザー呼び出し不要）
		/// </summary>
		virtual void Update() = 0;

		/// <summary>
		/// Debug用のImGui
		/// </summary>
		virtual void DebugGUI() = 0;


	protected: // ** プロテクトなメンバ関数 ** //

		/// <summary>
		/// 名前のImGui
		/// </summary>
		void ShowNameGUI();
		/// <summary>
		/// 頂点のImGui
		/// </summary>
		void ShowVertexGUI(Vertex* v, int size);
		/// <summary>
		/// トランスフォームのImGui
		/// </summary>
		void ShowTransformGUI();
		/// <summary>
		/// マテリアルのImGui
		/// </summary>
		void ShowMaterialGUI();
		/// <summary>
		/// isActiveのImGui
		/// </summary>
		void ShowIsActiveGUI();
		/// <summary>
		/// Init関数を呼び出すImGui
		/// </summary>
		void CallInitGUI();
	};
}

/// <summary>
/// IPrimitiveを継承したクラスのみを選択できるテンプレート
/// </summary>
template<class PrimitiveT>
concept IsIPrimitive = std::is_base_of<LWP::Primitive::IPrimitive, PrimitiveT>::value;