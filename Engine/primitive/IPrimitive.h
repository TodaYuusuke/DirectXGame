#pragma once
#include "../object/WorldTransform.h"
#include "../math/vector/Vector3.h"
#include "../math/vector/Vector2.h"
#include "../resources/texture/Texture.h"
#include "../resources/Material.h"
#include "../utility/Color.h"
#include <wrl.h>
#include <d3d12.h>
#include <stdexcept>

namespace LWP::Primitive {
	// インスタンスをコントローラに格納させるため、コンストラクタでポインタを受け取る
	class Manager;
	enum FillMode : int {
		WireFrame,	// ワイヤーフレーム
		Fill,		// 埋め立て
	};

	struct Vertex {
		Math::Vector3 position;	// 座標
		Math::Vector2 texCoord; // UV座標
		Math::Vector3 normal;	// 法線
		Utility::Color color = { 255,255,255,255 };	// 色
	};


	class IPrimitive {
	public:
		// ** 共通の変数 ** //
		Vertex* vertices = nullptr;	// 描画する頂点
		uint32_t* indexes = nullptr;	// インデックス

		// ワールドトランスフォーム
		Object::WorldTransform transform;

		// マテリアル
		Resource::Material* material = nullptr;
		// テクスチャ
		//Resource::Texture* texture = nullptr;

		// 頂点共通のカラー
		// ・nullptrの場合は頂点ごとに色を参照する
		// ・そうでない場合は全ての頂点の色がこれになる
		Utility::Color* commonColor = nullptr;

		// 面を埋め立てるか
		FillMode fillMode = FillMode::Fill;
		// UIとして描画するか
		bool isUI = false;

	protected:	// 描画用にマネージャーのポインタを保持する
		Manager* primitiveManager;
		
	private: // ** プライベートな変数 ** //
		// ImGui用の変数
		int vertexNum_ = 0;

	public: // ** 共通関数 ** //
		
		/// <summary>
		/// コンストラクタ（ユーザ呼び出し禁止）
		/// </summary>
		IPrimitive(Manager*);
		
		/// <summary>
		/// 頂点を生成する関数（ユーザ呼び出し禁止）
		/// </summary>
		virtual void InitializeVertices();

		/// <summary>
		/// 描画
		/// </summary>
		virtual void Draw(Resource::Texture* texture = nullptr);
		/// <summary>
		/// ImGui
		/// </summary>
		void DebugGUI(const std::string& label = "IPrimitive");

		/// <summary>
		/// 頂点数を返す関数
		/// </summary>
		virtual int GetVertexCount() const;
		/// <summary>
		/// インデックスの数を返す関数
		/// </summary>
		virtual int GetIndexCount() const;


	protected: // ** 派生クラス用の関数 ** //
		/// <summary>
		/// 派生クラスで追加のImGuiを実装するとき用の関数
		/// </summary>
		virtual void DerivedDebugGUI(const std::string& label = "Derived");
	};
}