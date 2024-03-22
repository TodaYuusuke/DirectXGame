#pragma once
#include "../object/WorldTransform.h"
#include "../math/vector/Vector3.h"
#include "../math/vector/Vector2.h"
#include "../resources/texture/Texture.h"
#include "../resources/material/Material.h"
#include "../utility/Color.h"
#include "../utility/observers/ObserverPtr.h"
#include <wrl.h>
#include <d3d12.h>
#include <stdexcept>

// 前方宣言
namespace LWP::Base {
	class CommandManager;
}

namespace LWP::Primitive {
	struct Vertex {
		Math::Vector3 position = { 0.0f,0.0f,0.0f };// 座標
		Math::Vector2 texCoord = { 0.0f,0.0f };		// UV座標
		Math::Vector3 normal = { 0.0f,0.0f,0.0f };	// 法線
		Utility::Color color = { 255,255,255,255 };	// 色

		/// <summary>
		/// Observerクラス用のオペレーターオーバーロード
		/// </summary>
		bool operator==(const Vertex& other) const {
			return { position == other.position && texCoord == other.texCoord && normal == other.normal };
		}
	};


	class IPrimitive {
	public:
		// ** 共通の変数 ** //

		// 識別用の形状名
		std::string name = "primitive";

		
		std::vector<Vertex> vertices;	// 描画する頂点
		std::vector<uint32_t> indexes;	// インデックス

		// ワールドトランスフォーム
		Object::WorldTransform transform;

		// マテリアル
		Resource::Material material;
		// テクスチャ
		Utility::ObserverPtr<Resource::ITexture*> texture = nullptr;

		// 頂点共通のカラー
		// ・nullptrの場合は頂点ごとに色を参照する
		// ・そうでない場合は全ての頂点の色がこれになる
		Utility::Color* commonColor = nullptr;
		
		// UIとして描画するか
		bool isUI = false;
		// ワイヤーフレームで描画する
		bool isWireFrame = false;
		// アクティブ切り替え
		bool isActive = true;
		

	private: // ** プライベートな変数 ** //
		// ImGui用の変数
		int vertexNum_ = 0;

	public: // ** 共通関数 ** //
		
		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		IPrimitive();
		/// <summary>
		/// デフォルトデストラクタ
		/// </summary>
		~IPrimitive();

		/// <summary>
		/// Observerクラス用のオペレーターオーバーロード
		/// </summary>
		bool operator==(const IPrimitive& other) const = delete;
		bool operator==(IPrimitive& other) {
			return { vertices == other.vertices && indexes == other.indexes && transform == other.transform };
		}

		/// <summary>
		/// 初期化
		/// </summary>
		void Init();

		/// <summary>
		/// 頂点を生成する関数（ユーザ呼び出し禁止）
		/// </summary>
		virtual void CreateVertices();
		/// <summary>
		/// インデックスを生成する関数（ユーザ呼び出し禁止）
		/// </summary>
		virtual void CreateIndexes();

		/// <summary>
		/// 更新処理
		/// </summary>
		virtual void Update();

		/// <summary>
		/// 描画
		/// </summary>
		virtual void Draw(Base::CommandManager* manager);
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

		/// <summary>
		/// 値が変化したかを検知する関数
		/// </summary>
		virtual bool GetChanged();

	protected: // ** 派生クラス用の関数 ** //
		/// <summary>
		/// 派生クラスで追加のImGuiを実装するとき用の関数
		/// </summary>
		virtual void DerivedDebugGUI(const std::string& label = "Derived");

		/// <summary>
		/// 派生クラスで追加された変数の値が変化したかを検知する用の関数
		/// </summary>
		virtual bool DerivedGetChanged();
	};
}


/// <summary>
/// IPrimitiveを継承したクラスのみを選択できるテンプレート
/// </summary>
template<class PrimitiveT>
concept IsIPrimitive = std::is_base_of<LWP::Primitive::IPrimitive, PrimitiveT>::value;