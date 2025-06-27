#pragma once
#include "object/TransformEuler.h"

#include "model/Vertex.h"
#include "model/Material.h"
#include "utility/observers/Observer.h"
#include "utility/observers/ObserverPtr.h"

#include "model/Node.h"
#include <wrl.h>
#include <d3d12.h>
#include <stdexcept>

#include <vector>

// 前方宣言
namespace LWP::Base {
	class RendererManager;
}

namespace LWP::Primitive {
	class IPrimitive {
	public:
		// ** 共通の変数 ** //

		// 識別用の形状名
		std::string name = "primitive";

		
		std::vector<Vertex> vertices;	// 描画する頂点
		std::vector<uint32_t> indexes;	// インデックス

		// ワールドトランスフォーム
		Object::TransformQuat worldTF;

		// マテリアル
		Material material;


		// UIとして描画するか
		bool isUI = false;
		// ワイヤーフレームで描画する
		bool isWireFrame = false;
		// アクティブ切り替え
		bool isActive = true;
		

	public: // ** 共通関数 ** //
		
		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		IPrimitive();
		/// <summary>
		/// コピーコンストラクタ
		/// </summary>
		IPrimitive(const IPrimitive& other);
		/// <summary>
		/// デストラクタ
		/// </summary>
		~IPrimitive();

		/// <summary>
		/// Observerクラス用のオペレーターオーバーロード
		/// </summary>
		bool operator==(const IPrimitive& other) const = delete;
		bool operator==(IPrimitive& other) {
			return { vertices == other.vertices && indexes == other.indexes && worldTF == other.worldTF };
		}

		/// <summary>
		/// 初期化
		/// </summary>
		virtual void Init();

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
		virtual void Draw();
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


	// IPrimitiveのデータのみ（Observerクラス用）
	struct IPrimitiveStruct {
		std::string name;
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indexes;
		Object::TransformQuat worldTF;
		Material material;

		IPrimitiveStruct& operator=(const IPrimitive& other) {
			name = other.name;
			vertices = other.vertices;
			indexes = other.indexes;
			worldTF = other.worldTF;
			material = other.material;
			return *this;
		}
		bool operator==(IPrimitive& other) {
			return {
				name == other.name &&
				vertices == other.vertices &&
				indexes == other.indexes &&
				worldTF == other.worldTF &&
				material == other.material
			};
		}
	};
}


/// <summary>
/// IPrimitiveを継承したクラスのみを選択できるテンプレート
/// </summary>
template<class PrimitiveT>
concept IsIPrimitive = std::is_base_of<LWP::Primitive::IPrimitive, PrimitiveT>::value;